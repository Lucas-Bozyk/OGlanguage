#include "afd.h"
#include "determinizacao.h"
#include "minimizacao.h"
#include "thompson.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

namespace {

ogl::AFN construirIrrigador() {
    using ogl::Expressao;
    const auto repeticoes = Expressao::estrela(Expressao::alternar(
        Expressao::simbolo("temperatura_alta"), Expressao::simbolo("chuva")));
    const auto padrao = Expressao::concatenar(
        Expressao::concatenar(Expressao::simbolo("solo_seco"), repeticoes),
        Expressao::simbolo("reservatorio_baixo"));
    return ogl::thompson::construir(padrao);
}

ogl::Expressao construirCanonica() {
    using ogl::Expressao;
    return Expressao::concatenar(
        Expressao::estrela(Expressao::alternar(Expressao::simbolo("a"),
                                              Expressao::simbolo("b"))),
        Expressao::simbolo("c"));
}

ogl::Expressao construirCrescimento(std::size_t n) {
    using ogl::Expressao;
    const auto letras = Expressao::alternar(Expressao::simbolo("a"), Expressao::simbolo("b"));
    auto padrao = Expressao::concatenar(Expressao::estrela(letras), Expressao::simbolo("a"));
    for (std::size_t i = 1; i < n; ++i) padrao = Expressao::concatenar(padrao, letras);
    return padrao;
}

void enumerarCadeias(const ogl::Alfabeto& alfabeto, ogl::Cadeia& prefixo,
                     std::size_t restante, std::vector<ogl::Cadeia>& destino) {
    destino.push_back(prefixo);
    if (restante == 0) return;
    for (const auto& simbolo : alfabeto) {
        prefixo.push_back(simbolo);
        enumerarCadeias(alfabeto, prefixo, restante - 1, destino);
        prefixo.pop_back();
    }
}

std::size_t contarTransicoesVazias(const ogl::AFN& afn) {
    std::size_t total = 0;
    for (ogl::Estado estado = 0; estado < afn.quantidadeEstados(); ++estado) {
        for (const auto& transicao : afn.saidas(estado)) {
            if (transicao.simbolo == ogl::EPSILON) ++total;
        }
    }
    return total;
}

bool pertenceAoPadraoCanonico(const ogl::Cadeia& cadeia) {
    return !cadeia.empty() && cadeia.back() == "c" &&
        std::all_of(cadeia.begin(), cadeia.end() - 1, [](const ogl::Simbolo& simbolo) {
            return simbolo == "a" || simbolo == "b";
        });
}

void executarEstudo() {
    const ogl::Alfabeto alfabeto{"a", "b", "c"};
    const auto afn = ogl::thompson::construir(construirCanonica(), alfabeto);
    const auto determinizado = ogl::determinizar(afn);
    const auto minimizado = ogl::minimizar(determinizado.automato);

    std::vector<ogl::Cadeia> cadeias;
    ogl::Cadeia prefixo;
    enumerarCadeias(alfabeto, prefixo, 4, cadeias);
    std::size_t aceitas = 0, divergencias = 0;
    for (const auto& cadeia : cadeias) {
        const bool esperado = pertenceAoPadraoCanonico(cadeia);
        const bool aceitaAFN = afn.aceita(cadeia);
        const bool aceitaAFD = determinizado.automato.aceita(cadeia);
        const bool aceitaMinimo = minimizado.automato.aceita(cadeia);
        if (esperado) ++aceitas;
        if (aceitaAFN != esperado || aceitaAFD != esperado || aceitaMinimo != esperado) ++divergencias;
    }

    std::cout << "Modulo 4 e inicio do modulo 5: (a|b)*c\n"
              << "Thompson: " << afn.quantidadeEstados() << " estados, "
              << afn.quantidadeTransicoes() << " transicoes, "
              << contarTransicoesVazias(afn) << " transicoes epsilon\n"
              << "AFD por subconjuntos: " << determinizado.automato.quantidadeEstados()
              << " estados completos\n"
              << "Minimizacao: " << minimizado.estadosAntes << " estados antes, "
              << minimizado.automato.quantidadeEstados() << " depois\n"
              << "Comparacao: " << cadeias.size() << " cadeias, " << aceitas
              << " aceitas, " << divergencias << " divergencias\n"
              << "Crescimento para o n-esimo simbolo do fim ser a (alfabeto {a,b}):\n"
              << "n | AFN | AFD | minimo\n";
    for (std::size_t n = 1; n <= 6; ++n) {
        const auto crescimento = ogl::thompson::construir(construirCrescimento(n), {"a", "b"});
        const auto afd = ogl::determinizar(crescimento);
        const auto minimo = ogl::minimizar(afd.automato);
        std::cout << n << " | " << crescimento.quantidadeEstados() << " | "
                  << afd.automato.quantidadeEstados() << " | "
                  << minimo.automato.quantidadeEstados() << '\n';
    }
}

// AFD manual do modulo 3: exatamente solo_seco, temperatura_alta, temperatura_alta.
ogl::AFD construirAFDManual() {
    std::vector<ogl::ConjuntoEstados> tabela(5, ogl::ConjuntoEstados(6, 4));
    tabela[0][0] = 1;
    tabela[1][1] = 2;
    tabela[2][1] = 3;
    return {ogl::ALFABETO_EVENTOS, tabela, 0, {false, false, false, true, false}};
}

ogl::Cadeia lerEventos(std::istream& entrada) {
    ogl::Cadeia cadeia;
    std::string evento;
    while (entrada >> evento) cadeia.push_back(evento);
    if (entrada.bad() || (entrada.fail() && !entrada.eof())) {
        throw std::runtime_error("Falha na leitura dos eventos.");
    }
    return cadeia;
}

void mostrarConjunto(const ogl::ConjuntoEstados& estados) {
    std::cout << '{';
    for (std::size_t i = 0; i < estados.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << 'q' << estados[i];
    }
    std::cout << '}';
}

void executarThompson(const ogl::Cadeia& cadeia) {
    const auto afn = construirIrrigador();
    std::cout << "Padrao demonstrado: solo_seco,(temperatura_alta|chuva)*,reservatorio_baixo\n"
              << "AFN: " << afn.quantidadeEstados() << " estados, "
              << afn.quantidadeTransicoes() << " transicoes\n";
    const auto resultado = afn.simular(cadeia);
    for (std::size_t i = 0; i < resultado.passos.size(); ++i) {
        const auto& passo = resultado.passos[i];
        if (i == 0) std::cout << "Inicio: ";
        else std::cout << "Apos " << passo.evento << ": ";
        mostrarConjunto(passo.ativos);
        std::cout << '\n';
    }
    std::cout << (resultado.aceitou ? "ACEITA\n" : "REJEITADA\n");
}

void executarAFD(const ogl::Cadeia& cadeia) {
    const auto afd = construirAFDManual();
    const auto resultado = afd.simular(cadeia);
    std::cout << "AFD manual: solo_seco,temperatura_alta,temperatura_alta\n"
              << "Inicio: q" << resultado.passos.front() << '\n';
    for (std::size_t i = 0; i < cadeia.size(); ++i) {
        std::cout << "Apos " << cadeia[i] << ": ";
        const auto estado = resultado.passos[i + 1];
        if (estado == ogl::SEM_ESTADO) std::cout << "fora do alfabeto";
        else if (estado == afd.estadoDeErro()) std::cout << "erro";
        else std::cout << 'q' << estado;
        std::cout << '\n';
    }
    std::cout << (resultado.aceitou ? "ACEITA\n" : "REJEITADA\n");
}

void mostrarUso(std::ostream& saida) {
    saida << "Uso: ogl [thompson | afd] [arquivo_de_eventos | -]\n"
          << "     ogl estudo\n"
          << "Sem arquivo, executa uma cadeia de demonstracao.\n"
          << "Eventos devem ser separados por espacos ou quebras de linha.\n"
          << "Novos padroes sao compostos pela API de thompson.h nesta etapa.\n";
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc == 2 && std::string(argv[1]) == "--help") {
            mostrarUso(std::cout);
            return 0;
        }
        const std::string modo = argc >= 2 ? argv[1] : "thompson";
        if (modo == "estudo") {
            if (argc != 2) {
                mostrarUso(std::cerr);
                return 1;
            }
            executarEstudo();
            return 0;
        }
        if (argc > 3 || (modo != "thompson" && modo != "afd")) {
            mostrarUso(std::cerr);
            return 1;
        }
        ogl::Cadeia cadeia;
        if (argc == 3) {
            const std::string caminho = argv[2];
            if (caminho == "-") cadeia = lerEventos(std::cin);
            else {
                std::ifstream entrada(caminho);
                if (!entrada) throw std::runtime_error("Nao foi possivel abrir " + caminho);
                cadeia = lerEventos(entrada);
            }
        } else if (modo == "afd") {
            cadeia = {"solo_seco", "temperatura_alta", "temperatura_alta"};
        } else {
            cadeia = {"solo_seco", "temperatura_alta", "chuva", "reservatorio_baixo"};
        }
        if (modo == "afd") executarAFD(cadeia);
        else executarThompson(cadeia);
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << '\n';
        return 1;
    }
}
