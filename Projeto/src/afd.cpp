#include "afd.h"

#include <algorithm>
#include <utility>

namespace ogl {

AFD::AFD(Alfabeto alfabeto, std::vector<ConjuntoEstados> tabela,
         Estado inicial, std::vector<bool> finais)
    : alfabeto_(std::move(alfabeto)), tabela_(std::move(tabela)),
      inicial_(inicial), finais_(std::move(finais)) {
    validarAlfabeto(alfabeto_);
    if (tabela_.empty() || finais_.size() != tabela_.size()) {
        throw std::invalid_argument("Tabela e finais devem descrever ao menos um estado.");
    }
    validarEstado(inicial_);
    for (const auto& linha : tabela_) {
        if (linha.size() != alfabeto_.size()) {
            throw std::invalid_argument("AFD deve ter uma coluna por simbolo do alfabeto.");
        }
        for (const Estado destino : linha) {
            if (destino != SEM_ESTADO) validarEstado(destino);
        }
    }
}

void AFD::validarEstado(Estado estado) const {
    if (estado >= quantidadeEstados()) throw std::out_of_range("Estado inexistente no AFD.");
}

const Alfabeto& AFD::alfabeto() const noexcept { return alfabeto_; }
std::size_t AFD::quantidadeEstados() const noexcept { return tabela_.size(); }
bool AFD::ehCompleto() const noexcept {
    for (const auto& linha : tabela_) {
        if (std::find(linha.begin(), linha.end(), SEM_ESTADO) != linha.end()) return false;
    }
    return true;
}
Estado AFD::inicial() const noexcept { return inicial_; }

bool AFD::ehFinal(Estado estado) const {
    validarEstado(estado);
    return finais_[estado];
}

Estado AFD::transicao(Estado origem, const Simbolo& evento) const {
    validarEstado(origem);
    const auto coluna = std::find(alfabeto_.begin(), alfabeto_.end(), evento);
    if (coluna == alfabeto_.end()) return SEM_ESTADO;
    return tabela_[origem][static_cast<std::size_t>(coluna - alfabeto_.begin())];
}

Estado AFD::estadoDeErro() const noexcept {
    for (Estado estado = 0; estado < quantidadeEstados(); ++estado) {
        if (!finais_[estado] && std::all_of(tabela_[estado].begin(), tabela_[estado].end(),
            [estado](Estado destino) { return destino == estado; })) return estado;
    }
    return SEM_ESTADO; // Linguagens como Sigma* podem dispensar um erro alcancavel.
}

bool AFD::aceita(const Cadeia& cadeia) const {
    Estado atual = inicial_;
    for (const auto& evento : cadeia) {
        atual = transicao(atual, evento);
        if (atual == SEM_ESTADO) return false;
    }
    return finais_[atual];
}

SimulacaoAFD AFD::simular(const Cadeia& cadeia) const {
    Estado atual = inicial_;
    SimulacaoAFD resultado{false, {atual}};
    for (const auto& evento : cadeia) {
        if (atual != SEM_ESTADO) atual = transicao(atual, evento);
        resultado.passos.push_back(atual);
    }
    resultado.aceitou = atual != SEM_ESTADO && finais_[atual];
    return resultado;
}

} // namespace ogl
