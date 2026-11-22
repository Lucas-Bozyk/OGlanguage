#include "determinizacao.h"

#include <algorithm>
#include <map>
#include <utility>

namespace ogl {
namespace {

bool contemFinal(const AFN& afn, const ConjuntoEstados& estados) {
    return std::any_of(estados.begin(), estados.end(), [&](Estado estado) {
        return afn.ehFinal(estado);
    });
}

} // namespace

ResultadoDeterminacao determinizar(const AFN& afn) {
    using Catalogo = std::map<ConjuntoEstados, Estado>;
    Catalogo catalogo;
    std::vector<ConjuntoEstados> subconjuntos;
    std::vector<bool> finais;
    std::vector<ConjuntoEstados> tabela;

    const auto inicial = afn.fechoEpsilon({afn.inicial()});
    catalogo.emplace(inicial, 0);
    subconjuntos.push_back(inicial);
    finais.push_back(contemFinal(afn, inicial));
    tabela.emplace_back();

    for (Estado atual = 0; atual < subconjuntos.size(); ++atual) {
        for (const auto& simbolo : afn.alfabeto()) {
            auto destino = afn.avancar(subconjuntos[atual], simbolo);
            const auto encontrado = catalogo.find(destino);
            Estado indice = SEM_ESTADO;
            if (encontrado != catalogo.end()) {
                indice = encontrado->second;
            } else {
                indice = subconjuntos.size();
                catalogo.emplace(destino, indice);
                subconjuntos.push_back(std::move(destino));
                finais.push_back(contemFinal(afn, subconjuntos.back()));
                tabela.emplace_back();
            }
            tabela[atual].push_back(indice);
        }
    }

    return {AFD(afn.alfabeto(), std::move(tabela), 0, std::move(finais)),
            std::move(subconjuntos)};
}

} // namespace ogl
