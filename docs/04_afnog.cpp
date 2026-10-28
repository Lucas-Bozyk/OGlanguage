#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Cada nome inteiro representa um simbolo do alfabeto de eventos.
const std::string alfabeto[] = {
    "solo_seco", "temperatura_alta", "chuva",
    "reservatorio_baixo", "obstaculo_detectado", "cultura_madura"
};
const int EPSILON = -1; // Rotulo interno; nao pode ser escrito como evento.
const char* const padraoExemplo =
    "solo_seco,(temperatura_alta|chuva)*,reservatorio_baixo";

int colunaDoEvento(const std::string& evento) {
    for (int i = 0; i < 6; ++i) {
        if (evento == alfabeto[i]) return i;
    }
    return -1;
}

// Apenas os cinco nos do nucleo definido no modulo 2.
enum class Tipo { Simbolo, Epsilon, Concatenacao, Alternancia, Estrela };
struct No;
using Arvore = std::shared_ptr<const No>;
struct No {
    Tipo tipo;
    int evento;
    Arvore esquerda;
    Arvore direita;
};

Arvore no(Tipo tipo, Arvore esquerda = {}, Arvore direita = {}, int evento = EPSILON) {
    return std::make_shared<No>(No{tipo, evento, esquerda, direita});
}

// Leitor do padrao isolado: virgula explicita, um posfixo por primaria,
// espacos e comentarios //, conforme docs/01_gramatica.md.
class LeitorPadrao {
    const std::string& fonte;
    std::size_t posicao = 0;

    void ignorarSeparadores() {
        while (posicao < fonte.size()) {
            if (std::isspace(static_cast<unsigned char>(fonte[posicao]))) {
                ++posicao;
            } else if (fonte.compare(posicao, 2, "//") == 0) {
                while (posicao < fonte.size() && fonte[posicao] != '\n') ++posicao;
            } else {
                break;
            }
        }
    }

    char olhar() {
        ignorarSeparadores();
        return posicao < fonte.size() ? fonte[posicao] : '\0';
    }

    [[noreturn]] void erro(const std::string& mensagem) const {
        std::size_t linha = 1, coluna = 1;
        for (std::size_t i = 0; i < posicao; ++i) {
            if (fonte[i] == '\n') { ++linha; coluna = 1; }
            else ++coluna;
        }
        throw std::invalid_argument(mensagem + " (deslocamento " +
            std::to_string(posicao) + ", linha " + std::to_string(linha) +
            ", coluna " + std::to_string(coluna) + ")");
    }

    static bool letra(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    Arvore primaria() {
        const char c = olhar();
        if (c == '(') {
            ++posicao;
            if (olhar() == ')') erro("grupo vazio nao permitido");
            Arvore grupo = alternacao();
            if (olhar() != ')') erro("esperado ')'");
            ++posicao;
            return grupo;
        }
        if (!letra(c)) erro("esperado evento ou grupo");
        const std::size_t inicio = posicao++;
        while (posicao < fonte.size()) {
            const char atual = fonte[posicao];
            if (!letra(atual) && !(atual >= '0' && atual <= '9') && atual != '_') break;
            ++posicao;
        }
        const std::string evento = fonte.substr(inicio, posicao - inicio);
        const int coluna = colunaDoEvento(evento);
        if (coluna == -1) {
            posicao = inicio;
            erro("evento fora do alfabeto: " + evento);
        }
        return no(Tipo::Simbolo, {}, {}, coluna);
    }

    Arvore posfixa() {
        Arvore base = primaria();
        const char operador = olhar();
        if (operador == '*' || operador == '+' || operador == '?') {
            ++posicao;
            if (operador == '*') base = no(Tipo::Estrela, base);
            // Subarvore imutavel compartilhada; Thompson constroi cada ocorrencia.
            else if (operador == '+') base = no(Tipo::Concatenacao, base, no(Tipo::Estrela, base));
            else base = no(Tipo::Alternancia, base, no(Tipo::Epsilon));
            const char seguinte = olhar();
            if (seguinte == '*' || seguinte == '+' || seguinte == '?') {
                erro("operador pos-fixo repetido");
            }
        }
        return base;
    }

    Arvore concatenacao() {
        Arvore raiz = posfixa();
        while (olhar() == ',') {
            ++posicao;
            Arvore direita = posfixa();
            raiz = no(Tipo::Concatenacao, raiz, direita);
        }
        return raiz;
    }

    Arvore alternacao() {
        Arvore raiz = concatenacao();
        while (olhar() == '|') {
            ++posicao;
            Arvore direita = concatenacao();
            raiz = no(Tipo::Alternancia, raiz, direita);
        }
        return raiz;
    }

public:
    explicit LeitorPadrao(const std::string& texto) : fonte(texto) {}

    Arvore ler() {
        Arvore raiz = alternacao();
        ignorarSeparadores();
        if (posicao != fonte.size()) erro("simbolo inesperado; esperado fim do padrao");
        return raiz;
    }
};

std::string prefixa(const Arvore& raiz) {
    switch (raiz->tipo) {
        case Tipo::Simbolo: return "(simbolo " + alfabeto[raiz->evento] + ")";
        case Tipo::Epsilon: return "(epsilon)";
        case Tipo::Estrela: return "(estrela " + prefixa(raiz->esquerda) + ")";
        case Tipo::Concatenacao:
            return "(concat " + prefixa(raiz->esquerda) + " " + prefixa(raiz->direita) + ")";
        case Tipo::Alternancia:
            return "(alt " + prefixa(raiz->esquerda) + " " + prefixa(raiz->direita) + ")";
    }
    throw std::logic_error("tipo de no invalido");
}

using Estado = std::size_t;
using Conjunto = std::vector<unsigned char>; // Uma marca por estado; sem duplicatas.
struct Transicao { int evento; Estado destino; };
struct Bloco { Estado entrada; Estado saida; };

class AFN {
    std::vector<std::vector<Transicao>> transicoes;
    Estado inicial = 0;
    Estado final = 0;

    Estado novoEstado() {
        transicoes.emplace_back();
        return transicoes.size() - 1;
    }

    Bloco construir(const Arvore& raiz) {
        switch (raiz->tipo) {
            case Tipo::Simbolo:
            case Tipo::Epsilon: {
                const Estado entrada = novoEstado(), saida = novoEstado();
                transicoes[entrada].push_back({raiz->evento, saida});
                return {entrada, saida};
            }
            case Tipo::Concatenacao: {
                const Bloco a = construir(raiz->esquerda);
                const Bloco b = construir(raiz->direita);
                transicoes[a.saida].push_back({EPSILON, b.entrada});
                return {a.entrada, b.saida};
            }
            case Tipo::Alternancia: {
                const Bloco a = construir(raiz->esquerda);
                const Bloco b = construir(raiz->direita);
                const Estado entrada = novoEstado(), saida = novoEstado();
                transicoes[entrada].push_back({EPSILON, a.entrada});
                transicoes[entrada].push_back({EPSILON, b.entrada});
                transicoes[a.saida].push_back({EPSILON, saida});
                transicoes[b.saida].push_back({EPSILON, saida});
                return {entrada, saida};
            }
            case Tipo::Estrela: {
                const Bloco interior = construir(raiz->esquerda);
                const Estado entrada = novoEstado(), saida = novoEstado();
                transicoes[entrada].push_back({EPSILON, interior.entrada});
                transicoes[entrada].push_back({EPSILON, saida});
                transicoes[interior.saida].push_back({EPSILON, interior.entrada});
                transicoes[interior.saida].push_back({EPSILON, saida});
                return {entrada, saida};
            }
        }
        throw std::logic_error("tipo de no invalido");
    }

    void fechoVazio(Conjunto& conjunto) const {
        std::vector<Estado> pendentes;
        for (Estado q = 0; q < conjunto.size(); ++q) {
            if (conjunto[q]) pendentes.push_back(q);
        }
        while (!pendentes.empty()) {
            const Estado q = pendentes.back();
            pendentes.pop_back();
            for (const Transicao& t : transicoes[q]) {
                if (t.evento == EPSILON && !conjunto[t.destino]) {
                    // Marca ANTES de empilhar: ciclos epsilon tambem terminam.
                    conjunto[t.destino] = 1;
                    pendentes.push_back(t.destino);
                }
            }
        }
    }

    static void mostrarConjunto(const Conjunto& conjunto, std::ostream& saida) {
        saida << '{';
        bool primeiro = true;
        for (Estado q = 0; q < conjunto.size(); ++q) {
            if (!conjunto[q]) continue;
            if (!primeiro) saida << ", ";
            saida << 'q' << q;
            primeiro = false;
        }
        saida << '}';
    }

public:
    explicit AFN(const Arvore& raiz) {
        const Bloco bloco = construir(raiz);
        inicial = bloco.entrada;
        final = bloco.saida;
    }

    void mostrarResumo(std::ostream& saida) const {
        std::size_t total = 0, vazias = 0;
        for (const auto& lista : transicoes) {
            for (const Transicao& t : lista) {
                ++total;
                if (t.evento == EPSILON) ++vazias;
            }
        }
        saida << "AFN: " << transicoes.size() << " estados, " << total
              << " transicoes (" << vazias << " epsilon)\n"
              << "Estado inicial: q" << inicial << "; F = {q" << final << "}\n";
    }

    bool aceita(std::istream& entrada, std::ostream* traco = nullptr) const {
        Conjunto ativos(transicoes.size(), 0);
        ativos[inicial] = 1;
        fechoVazio(ativos); // Necessario tambem antes do PRIMEIRO evento.
        std::size_t maior = std::count(ativos.begin(), ativos.end(), 1);
        if (traco) {
            *traco << "Inicio: "; mostrarConjunto(ativos, *traco); *traco << '\n';
        }
        std::string evento;
        while (entrada >> evento) {
            const int coluna = colunaDoEvento(evento);
            Conjunto proximos(transicoes.size(), 0);
            if (coluna != -1) { // -1 nao pode ser confundido com EPSILON.
                for (Estado q = 0; q < ativos.size(); ++q) {
                    if (!ativos[q]) continue;
                    for (const Transicao& t : transicoes[q]) {
                        if (t.evento == coluna) proximos[t.destino] = 1;
                    }
                }
            }
            fechoVazio(proximos);
            ativos.swap(proximos);
            maior = std::max(maior, static_cast<std::size_t>(
                std::count(ativos.begin(), ativos.end(), 1)));
            if (traco) {
                *traco << "Apos " << evento << ": "; mostrarConjunto(ativos, *traco);
                if (coluna == -1) *traco << " (evento fora do alfabeto)";
                else if (std::none_of(ativos.begin(), ativos.end(), [](unsigned char marca) {
                    return marca != 0;
                })) *traco << " (nenhum caminho ativo)";
                *traco << '\n';
            }
            // Continua a leitura mesmo no vazio, expondo que nao ha reinicio.
        }
        if (entrada.bad() || (entrada.fail() && !entrada.eof())) {
            throw std::runtime_error("erro ao ler os eventos");
        }
        if (traco) *traco << "Maior conjunto ativo: " << maior << '\n';
        return ativos[final] != 0; // Basta UM caminho aceitar, ao final da entrada.
    }
};

bool mesmaArvore(const Arvore& a, const Arvore& b) {
    if (!a || !b) return a == b;
    return a->tipo == b->tipo && a->evento == b->evento &&
        mesmaArvore(a->esquerda, b->esquerda) && mesmaArvore(a->direita, b->direita);
}

int rodarTestes(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo) throw std::runtime_error("nao foi possivel abrir " + caminho);
    std::size_t linha = 0, total = 0, passaram = 0;
    std::string texto;
    while (std::getline(arquivo, texto)) {
        ++linha;
        if (!texto.empty() && texto.back() == '\r') texto.pop_back();
        if (texto.empty() || texto[0] == '#') continue;
        ++total;
        const std::size_t a = texto.find('\t');
        const std::size_t b = a == std::string::npos ? a : texto.find('\t', a + 1);
        if (b == std::string::npos || texto.find('\t', b + 1) != std::string::npos) {
            std::cerr << caminho << ':' << linha << ": esperados tres campos separados por TAB\n";
            continue;
        }
        const std::string padrao = texto.substr(0, a);
        const std::string eventos = texto.substr(a + 1, b - a - 1);
        const std::string esperado = texto.substr(b + 1);
        if (esperado != "ACEITA" && esperado != "REJEITADA" && esperado != "ERRO_PADRAO") {
            std::cerr << caminho << ':' << linha << ": veredito invalido\n";
            continue;
        }
        std::string obtido;
        try {
            const AFN afn(LeitorPadrao(padrao).ler());
            std::istringstream entrada(eventos);
            obtido = afn.aceita(entrada) ? "ACEITA" : "REJEITADA";
        } catch (const std::invalid_argument&) {
            obtido = "ERRO_PADRAO";
        }
        if (obtido == esperado) ++passaram;
        else std::cerr << caminho << ':' << linha << ": esperado " << esperado
                       << ", obtido " << obtido << '\n';
    }
    if (arquivo.bad()) throw std::runtime_error("erro ao ler " + caminho);
    if (total == 0) throw std::runtime_error("bateria sem casos: " + caminho);
    // Convergencia estrutural exigida pelo modulo 2, alem dos testes de linguagem.
    const std::string mais = "chuva+", expandido = "chuva,chuva*";
    const bool convergiu = mesmaArvore(LeitorPadrao(mais).ler(), LeitorPadrao(expandido).ler());
    std::cout << "Bateria: " << passaram << '/' << total << " casos passaram.\n"
              << "Normalizacao de chuva+: " << (convergiu ? "OK" : "FALHOU") << '\n';
    return passaram == total && convergiu ? 0 : 1;
}

int main(int argc, char* argv[]) {
    try {
        if (argc >= 2 && std::string(argv[1]) == "--testes" && argc <= 3) {
            return rodarTestes(argc == 3 ? argv[2] : "examples/04_casos.txt");
        }
        if (argc > 3) {
            std::cerr << "Uso: 04_afnog [arquivo_de_eventos | -] [padrao]\n"
                      << "     04_afnog --testes [arquivo_de_casos]\n";
            return 1;
        }
        const std::string padrao = argc == 3 ? argv[2] : padraoExemplo;
        const Arvore arvore = LeitorPadrao(padrao).ler();
        const AFN afn(arvore);
        const std::string caminho = argc >= 2 ? argv[1] : "examples/04_eventos.txt";
        std::ifstream arquivo;
        std::istream* entrada = &std::cin;
        if (caminho != "-") {
            arquivo.open(caminho);
            if (!arquivo) throw std::runtime_error("nao foi possivel abrir " + caminho);
            entrada = &arquivo;
        }
        std::cout << "Padrao: " << padrao << "\nAST: " << prefixa(arvore) << '\n';
        afn.mostrarResumo(std::cout);
        const bool resultado = afn.aceita(*entrada, &std::cout);
        std::cout << (resultado ? "ACEITA\n" : "REJEITADA\n");
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}
