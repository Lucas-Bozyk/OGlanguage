#pragma once

#include "afd.h"
#include "afn.h"

namespace ogl {

struct ResultadoDeterminacao {
    AFD automato;
    // Cada entrada associa o estado do AFD ao conjunto canonico de estados do AFN.
    std::vector<ConjuntoEstados> subconjuntos;
};

ResultadoDeterminacao determinizar(const AFN& afn);

} // namespace ogl
