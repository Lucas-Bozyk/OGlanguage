#include "afn.h"

#include <algorithm>
#include <utility>

namespace ogl {

AFN::AFN(Alfabeto alfabeto) : alfabeto_(std::move(alfabeto)) {
    validarAlfabeto(alfabeto_);
}

Estado AFN::adicionarEstado(bool aceitacao) {
    const Estado estado = transicoes_.size();
    transicoes_.emplace_back();
    finais_.push_back(aceitacao);
    return estado;
}

void AFN::validarEstado(Estado estado) const {
    if (estado >= quantidadeEstados()) throw std::out_of_range("Estado inexistente no AFN.");
}

void AFN::definirInicial(Estado estado) {
    validarEstado(estado);
    inicial_ = estado;
}

void AFN::definirAceitacao(Estado estado, bool aceitacao) {
    validarEstado(estado);
    finais_[estado] = aceitacao;
}

void AFN::adicionarTransicao(Estado origem, const Simbolo& simbolo, Estado destino) {
    validarEstado(origem);
    validarEstado(destino);
    if (simbolo != EPSILON &&
        std::find(alfabeto_.begin(), alfabeto_.end(), simbolo) == alfabeto_.end()) {
        throw std::invalid_argument("Evento fora do alfabeto: " + simbolo);
    }
    auto& lista = transicoes_[origem];
    const auto repetida = std::find_if(lista.begin(), lista.end(), [&](const auto& t) {
        return t.simbolo == simbolo && t.destino == destino;
    });
    if (repetida == lista.end()) lista.push_back({simbolo, destino});
}

const Alfabeto& AFN::alfabeto() const noexcept { return alfabeto_; }
std::size_t AFN::quantidadeEstados() const noexcept { return transicoes_.size(); }

std::size_t AFN::quantidadeTransicoes() const noexcept {
    std::size_t total = 0;
    for (const auto& lista : transicoes_) total += lista.size();
    return total;
}

Estado AFN::inicial() const {
    if (inicial_ == SEM_ESTADO) throw std::logic_error("AFN sem estado inicial definido.");
    return inicial_;
}

bool AFN::ehFinal(Estado estado) const {
    validarEstado(estado);
    return finais_[estado];
}

const std::vector<Transicao>& AFN::saidas(Estado estado) const {
    validarEstado(estado);
    return transicoes_[estado];
}

ConjuntoEstados AFN::fechoEpsilon(const ConjuntoEstados& estados) const {
    std::vector<bool> visitado(quantidadeEstados(), false);
    ConjuntoEstados pendentes;
    for (const Estado estado : estados) {
        validarEstado(estado);
        if (!visitado[estado]) {
            visitado[estado] = true;
            pendentes.push_back(estado);
        }
    }
    while (!pendentes.empty()) {
        const Estado atual = pendentes.back();
        pendentes.pop_back();
        for (const auto& t : transicoes_[atual]) {
            if (t.simbolo == EPSILON && !visitado[t.destino]) {
                visitado[t.destino] = true; // Antes de empilhar, inclusive em ciclos.
                pendentes.push_back(t.destino);
            }
        }
    }
    ConjuntoEstados resultado;
    for (Estado estado = 0; estado < visitado.size(); ++estado) {
        if (visitado[estado]) resultado.push_back(estado);
    }
    return resultado; // Canonico: ordenado e sem repeticoes.
}

ConjuntoEstados AFN::mover(const ConjuntoEstados& estados, const Simbolo& evento) const {
    for (const Estado estado : estados) validarEstado(estado);
    // Entrada desconhecida, incluindo EPSILON, nao segue transicoes vazias.
    if (std::find(alfabeto_.begin(), alfabeto_.end(), evento) == alfabeto_.end()) return {};
    std::vector<bool> destinos(quantidadeEstados(), false);
    for (const Estado estado : estados) {
        for (const auto& t : transicoes_[estado]) {
            if (t.simbolo == evento) destinos[t.destino] = true;
        }
    }
    ConjuntoEstados resultado;
    for (Estado estado = 0; estado < destinos.size(); ++estado) {
        if (destinos[estado]) resultado.push_back(estado);
    }
    return resultado;
}

ConjuntoEstados AFN::avancar(const ConjuntoEstados& estados, const Simbolo& evento) const {
    return fechoEpsilon(mover(estados, evento));
}

bool AFN::contemFinal(const ConjuntoEstados& estados) const {
    return std::any_of(estados.begin(), estados.end(), [&](Estado estado) {
        return finais_[estado];
    });
}

bool AFN::aceita(const Cadeia& cadeia) const {
    auto ativos = fechoEpsilon({inicial()});
    for (const auto& evento : cadeia) {
        ativos = avancar(ativos, evento);
        if (ativos.empty()) return false;
    }
    return contemFinal(ativos);
}

SimulacaoAFN AFN::simular(const Cadeia& cadeia) const {
    auto ativos = fechoEpsilon({inicial()});
    SimulacaoAFN resultado{false, {{EPSILON, ativos}}};
    for (const auto& evento : cadeia) {
        ativos = avancar(ativos, evento);
        resultado.passos.push_back({evento, ativos});
    }
    resultado.aceitou = contemFinal(ativos);
    return resultado;
}

} // namespace ogl
