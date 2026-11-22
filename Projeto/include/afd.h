#pragma once

#include "automato.h"

namespace ogl {

struct SimulacaoAFD {
    bool aceitou;
    // Inicial e um estado por evento. SEM_ESTADO representa evento desconhecido.
    ConjuntoEstados passos;
};

// A tabela pode conter SEM_ESTADO para representar uma transicao parcial.
class AFD {
public:
    AFD(Alfabeto alfabeto, std::vector<ConjuntoEstados> tabela,
        Estado inicial, std::vector<bool> finais);

    const Alfabeto& alfabeto() const noexcept;
    std::size_t quantidadeEstados() const noexcept;
    bool ehCompleto() const noexcept;
    Estado inicial() const noexcept;
    bool ehFinal(Estado estado) const;
    Estado transicao(Estado origem, const Simbolo& evento) const;
    Estado estadoDeErro() const noexcept;
    bool aceita(const Cadeia& cadeia) const;
    SimulacaoAFD simular(const Cadeia& cadeia) const;

private:
    void validarEstado(Estado estado) const;

    Alfabeto alfabeto_;
    std::vector<ConjuntoEstados> tabela_;
    Estado inicial_;
    std::vector<bool> finais_;
};

} // namespace ogl
