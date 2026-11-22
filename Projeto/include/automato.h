#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace ogl {

using Estado = std::size_t;
using Simbolo = std::string;
using Alfabeto = std::vector<Simbolo>;
using Cadeia = std::vector<Simbolo>;
using ConjuntoEstados = std::vector<Estado>;

// Um nome inteiro e um simbolo. A cadeia vazia e Cadeia{}, nao {EPSILON}.
inline const Simbolo EPSILON = "";
inline constexpr Estado SEM_ESTADO = std::numeric_limits<Estado>::max();
inline const Alfabeto ALFABETO_EVENTOS = {
    "solo_seco", "temperatura_alta", "chuva", "reservatorio_baixo",
    "obstaculo_detectado", "cultura_madura"
};

struct Transicao {
    Simbolo simbolo;
    Estado destino;
};

// Centralizado aqui: automato.h nao possui arquivo de implementacao proprio.
inline void validarAlfabeto(const Alfabeto& alfabeto) {
    std::unordered_set<Simbolo> vistos;
    for (const auto& simbolo : alfabeto) {
        if (simbolo == EPSILON) {
            throw std::invalid_argument("Epsilon nao pertence ao alfabeto.");
        }
        if (!vistos.insert(simbolo).second) {
            throw std::invalid_argument("Simbolo repetido no alfabeto: " + simbolo);
        }
    }
}

} // namespace ogl
