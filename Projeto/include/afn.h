#pragma once

#include "automato.h"

namespace ogl {

struct PassoAFN {
    Simbolo evento; // EPSILON apenas no passo inicial do traco.
    ConjuntoEstados ativos;
};

struct SimulacaoAFN {
    bool aceitou;
    std::vector<PassoAFN> passos;
};

class AFN {
public:
    explicit AFN(Alfabeto alfabeto = ALFABETO_EVENTOS);

    Estado adicionarEstado(bool aceitacao = false);
    void definirInicial(Estado estado);
    void definirAceitacao(Estado estado, bool aceitacao = true);
    void adicionarTransicao(Estado origem, const Simbolo& simbolo, Estado destino);

    const Alfabeto& alfabeto() const noexcept;
    std::size_t quantidadeEstados() const noexcept;
    std::size_t quantidadeTransicoes() const noexcept;
    Estado inicial() const;
    bool ehFinal(Estado estado) const;
    const std::vector<Transicao>& saidas(Estado estado) const;

    ConjuntoEstados fechoEpsilon(const ConjuntoEstados& estados) const;
    ConjuntoEstados mover(const ConjuntoEstados& estados, const Simbolo& evento) const;
    ConjuntoEstados avancar(const ConjuntoEstados& estados, const Simbolo& evento) const;
    bool aceita(const Cadeia& cadeia) const;
    SimulacaoAFN simular(const Cadeia& cadeia) const;

private:
    void validarEstado(Estado estado) const;
    bool contemFinal(const ConjuntoEstados& estados) const;

    Alfabeto alfabeto_;
    std::vector<std::vector<Transicao>> transicoes_;
    std::vector<bool> finais_;
    Estado inicial_ = SEM_ESTADO;
};

} // namespace ogl
