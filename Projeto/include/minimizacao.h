#pragma once

#include "afd.h"

namespace ogl {

struct ResultadoMinimizacao {
    AFD automato;
    std::size_t estadosAntes;
    std::size_t estadosCompletados;
    std::size_t estadosInacessiveis;
    std::size_t rodadasRefinamento;
    // Particoes por rodada, sobre os indices depois de remover inacessiveis.
    std::vector<std::vector<ConjuntoEstados>> historicoParticoes;
};

ResultadoMinimizacao minimizar(const AFD& afd);

} // namespace ogl
