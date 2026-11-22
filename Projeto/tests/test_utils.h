#pragma once

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace teste {

inline std::size_t verificacoes = 0;

inline void exigir(bool condicao, const char* expressao, const char* arquivo, int linha) {
    ++verificacoes;
    if (!condicao) {
        throw std::runtime_error(std::string(arquivo) + ':' + std::to_string(linha) +
                                 ": falhou: " + expressao);
    }
}

template <class Excecao, class Funcao>
void exigirExcecao(Funcao executar) {
    ++verificacoes;
    try {
        executar();
    } catch (const Excecao&) {
        return;
    }
    throw std::runtime_error("A operacao deveria lancar a excecao esperada.");
}

template <class Funcao>
int executar(Funcao testes) {
    try {
        testes();
        std::cout << verificacoes << " verificacoes passaram.\n";
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << erro.what() << '\n';
        return 1;
    }
}

} // namespace teste

// Nao usa assert: as verificacoes continuam ativas em builds Release.
#define EXIGIR(...) ::teste::exigir((__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__)
