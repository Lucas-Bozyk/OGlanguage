#include <fstream>
#include <iostream>
#include <string>

// AFD manual para o padrao: solo_seco, temperatura_alta, temperatura_alta.
// Reconhece uma sequencia completa; ainda nao interpreta arquivos .ogl.
// Q = {Q0, Q1, Q2, Q3, ERRO}, estado inicial = Q0, F = {Q3}.
enum Estado { Q0, Q1, Q2, Q3, ERRO };

// Cada nome inteiro e um simbolo, e nao cada caractere do nome.
const std::string alfabeto[] = {
    "solo_seco", "temperatura_alta", "chuva",
    "reservatorio_baixo", "obstaculo_detectado", "cultura_madura"
};

// Delta: linha = estado atual, coluna = posicao do evento no alfabeto.
// Todas as 5 x 6 transicoes estao definidas. ERRO e absorvente.
const Estado transicoes[5][6] = {
    // solo_seco, temperatura_alta, chuva, reservatorio, obstaculo, cultura
    {Q1,   ERRO, ERRO, ERRO, ERRO, ERRO}, // Q0: nada lido
    {ERRO, Q2,   ERRO, ERRO, ERRO, ERRO}, // Q1: leu solo_seco
    {ERRO, Q3,   ERRO, ERRO, ERRO, ERRO}, // Q2: leu a primeira temperatura_alta
    {ERRO, ERRO, ERRO, ERRO, ERRO, ERRO}, // Q3: padrao completo
    {ERRO, ERRO, ERRO, ERRO, ERRO, ERRO}  // ERRO: permanece no erro
};

const char* const nomes[] = {"q0", "q1", "q2", "q3", "erro"};

int colunaDoEvento(const std::string& evento) {
    for (int coluna = 0; coluna < 6; ++coluna) {
        if (evento == alfabeto[coluna]) {
            return coluna;
        }
    }
    return -1; // Evento fora do alfabeto.
}

bool aceita(std::istream& entrada) {
    Estado atual = Q0;
    std::string evento;
    std::cout << "Inicio: " << nomes[atual] << '\n';

    // Espacos e quebras de linha separam eventos, mas nao sao eventos.
    while (entrada >> evento) {
        const int coluna = colunaDoEvento(evento);
        const Estado proximo = coluna == -1 ? ERRO : transicoes[atual][coluna];

        std::cout << nomes[atual] << " -- " << evento
                  << " --> " << nomes[proximo];
        if (coluna == -1) {
            std::cout << " (evento fora do alfabeto)";
        } else if (atual != ERRO && proximo == ERRO) {
            std::cout << " (evento fora da sequencia esperada)";
        }
        std::cout << '\n';
        atual = proximo;
    }

    // Passar por Q3 durante a leitura nao basta: e preciso terminar nele.
    return atual == Q3;
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Uso: 03_afdog [arquivo_de_eventos | -]\n";
        return 1;
    }

    const std::string caminho = argc == 2 ? argv[1] : "examples/03_eventos.txt";
    std::ifstream arquivo;
    std::istream* entrada = &std::cin;
    if (caminho != "-") {
        arquivo.open(caminho);
        if (!arquivo.is_open()) {
            std::cerr << "Erro: nao foi possivel abrir " << caminho << '\n';
            return 1;
        }
        entrada = &arquivo;
    }

    std::cout << "Tabela: 5 x 6 = 30 entradas, "
              << sizeof(transicoes) << " bytes\n";
    const bool resultado = aceita(*entrada);
    if (entrada->bad() || (entrada->fail() && !entrada->eof())) {
        std::cerr << "Erro ao ler os eventos\n";
        return 1;
    }

    std::cout << (resultado ? "ACEITA\n" : "REJEITADA\n");
    return 0;
}
