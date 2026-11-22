#include "thompson.h"

namespace ogl::thompson {
namespace {

struct Copia {
    Estado entrada;
    ConjuntoEstados finais;
};

// Copia com renumeracao, sem modificar os operandos nem fundir estados.
Copia copiar(const AFN& origem, AFN& destino) {
    const Estado inicial = origem.inicial(); // Recusa operando nao configurado.
    ConjuntoEstados mapa;
    for (Estado q = 0; q < origem.quantidadeEstados(); ++q) {
        mapa.push_back(destino.adicionarEstado());
    }
    Copia copia{mapa[inicial], {}};
    for (Estado q = 0; q < origem.quantidadeEstados(); ++q) {
        if (origem.ehFinal(q)) copia.finais.push_back(mapa[q]);
        for (const auto& t : origem.saidas(q)) {
            destino.adicionarTransicao(mapa[q], t.simbolo, mapa[t.destino]);
        }
    }
    return copia;
}

void validarOperandos(const AFN& a, const AFN& b) {
    if (a.alfabeto() != b.alfabeto()) {
        throw std::invalid_argument("Operandos devem usar o mesmo alfabeto, na mesma ordem.");
    }
}

} // namespace

AFN simbolo(const Simbolo& evento, const Alfabeto& alfabeto) {
    if (evento == EPSILON) {
        throw std::invalid_argument("Use thompson::epsilon para a cadeia vazia.");
    }
    AFN resultado(alfabeto);
    const Estado entrada = resultado.adicionarEstado();
    const Estado saida = resultado.adicionarEstado(true);
    resultado.definirInicial(entrada);
    resultado.adicionarTransicao(entrada, evento, saida);
    return resultado;
}

AFN epsilon(const Alfabeto& alfabeto) {
    AFN resultado(alfabeto);
    const Estado entrada = resultado.adicionarEstado();
    const Estado saida = resultado.adicionarEstado(true);
    resultado.definirInicial(entrada);
    resultado.adicionarTransicao(entrada, EPSILON, saida);
    return resultado;
}

AFN concatenar(const AFN& primeiro, const AFN& segundo) {
    validarOperandos(primeiro, segundo);
    AFN resultado(primeiro.alfabeto());
    const auto a = copiar(primeiro, resultado);
    const auto b = copiar(segundo, resultado);
    resultado.definirInicial(a.entrada);
    for (const Estado final : a.finais) resultado.adicionarTransicao(final, EPSILON, b.entrada);
    for (const Estado final : b.finais) resultado.definirAceitacao(final);
    return resultado;
}

AFN alternar(const AFN& primeiro, const AFN& segundo) {
    validarOperandos(primeiro, segundo);
    AFN resultado(primeiro.alfabeto());
    const auto a = copiar(primeiro, resultado);
    const auto b = copiar(segundo, resultado);
    const Estado entrada = resultado.adicionarEstado();
    const Estado saida = resultado.adicionarEstado(true);
    resultado.definirInicial(entrada);
    resultado.adicionarTransicao(entrada, EPSILON, a.entrada);
    resultado.adicionarTransicao(entrada, EPSILON, b.entrada);
    for (const Estado final : a.finais) resultado.adicionarTransicao(final, EPSILON, saida);
    for (const Estado final : b.finais) resultado.adicionarTransicao(final, EPSILON, saida);
    return resultado;
}

AFN fechoKleene(const AFN& interior) {
    AFN resultado(interior.alfabeto());
    const auto bloco = copiar(interior, resultado);
    const Estado entrada = resultado.adicionarEstado();
    const Estado saida = resultado.adicionarEstado(true);
    resultado.definirInicial(entrada);
    resultado.adicionarTransicao(entrada, EPSILON, bloco.entrada);
    resultado.adicionarTransicao(entrada, EPSILON, saida);
    for (const Estado final : bloco.finais) {
        resultado.adicionarTransicao(final, EPSILON, bloco.entrada);
        resultado.adicionarTransicao(final, EPSILON, saida);
    }
    return resultado;
}

AFN construir(const Expressao& expressao, const Alfabeto& alfabeto) {
    switch (expressao.tipo()) {
        case TipoExpressao::Simbolo:
            return simbolo(expressao.valor(), alfabeto);
        case TipoExpressao::Epsilon:
            return epsilon(alfabeto);
        case TipoExpressao::Concatenacao:
            return concatenar(construir(expressao.esquerda(), alfabeto),
                              construir(expressao.direita(), alfabeto));
        case TipoExpressao::Alternancia:
            return alternar(construir(expressao.esquerda(), alfabeto),
                            construir(expressao.direita(), alfabeto));
        case TipoExpressao::Estrela:
            return fechoKleene(construir(expressao.esquerda(), alfabeto));
    }
    throw std::logic_error("Tipo de no da expressao sem regra de Thompson.");
}

} // namespace ogl::thompson
