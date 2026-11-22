#include "determinizacao.h"
#include "minimizacao.h"
#include "test_utils.h"
#include "thompson.h"

#include <algorithm>

using namespace ogl;

namespace {

Expressao expressaoCanonica() {
    return Expressao::concatenar(
        Expressao::estrela(Expressao::alternar(Expressao::simbolo("a"),
                                              Expressao::simbolo("b"))),
        Expressao::simbolo("c"));
}

void enumerar(const Alfabeto& alfabeto, Cadeia& prefixo, std::size_t restante,
              std::vector<Cadeia>& resultado) {
    resultado.push_back(prefixo);
    if (restante == 0) return;
    for (const auto& simbolo : alfabeto) {
        prefixo.push_back(simbolo);
        enumerar(alfabeto, prefixo, restante - 1, resultado);
        prefixo.pop_back();
    }
}

} // namespace

int main() {
    return teste::executar([] {
        const Alfabeto alfabeto{"a", "b", "c"};
        const auto afn = thompson::construir(expressaoCanonica(), alfabeto);
        const auto determinizado = determinizar(afn);
        const auto minimo = minimizar(determinizado.automato);
        EXIGIR(minimo.estadosAntes == 5);
        EXIGIR(minimo.estadosCompletados == 5);
        EXIGIR(minimo.estadosInacessiveis == 0);
        EXIGIR(minimo.automato.quantidadeEstados() == 3);
        EXIGIR(minimo.automato.ehCompleto());
        EXIGIR(minimo.automato.estadoDeErro() != SEM_ESTADO);

        std::vector<Cadeia> cadeias;
        Cadeia prefixo;
        enumerar(alfabeto, prefixo, 4, cadeias);
        std::size_t aceitas = 0;
        for (const auto& cadeia : cadeias) {
            const bool esperado = !cadeia.empty() && cadeia.back() == "c" &&
                std::all_of(cadeia.begin(), cadeia.end() - 1, [](const Simbolo& s) {
                    return s == "a" || s == "b";
                });
            EXIGIR(afn.aceita(cadeia) == esperado);
            EXIGIR(determinizado.automato.aceita(cadeia) == esperado);
            EXIGIR(minimo.automato.aceita(cadeia) == esperado);
            if (esperado) ++aceitas;
        }
        EXIGIR(cadeias.size() == 121 && aceitas == 15);

        // AFD parcial: completar cria o erro, e q2 e inalcançavel.
        const AFD parcial({"a", "b"}, {{1, SEM_ESTADO},
                                         {SEM_ESTADO, SEM_ESTADO},
                                         {2, 2}},
                          0, {false, true, true});
        EXIGIR(!parcial.ehCompleto());
        const auto reduzido = minimizar(parcial);
        EXIGIR(reduzido.estadosAntes == 3);
        EXIGIR(reduzido.estadosCompletados == 4);
        EXIGIR(reduzido.estadosInacessiveis == 1);
        EXIGIR(reduzido.automato.quantidadeEstados() == 3);
        EXIGIR(reduzido.automato.ehCompleto());
        EXIGIR(reduzido.automato.aceita({"a"}));
        EXIGIR(!reduzido.automato.aceita({}));
        EXIGIR(!reduzido.automato.aceita({"b"}));
        EXIGIR(!reduzido.automato.aceita({"a", "a"}));

        // AFD universal nao precisa de estado de erro; todos os estados sao finais.
        const AFD universal({"x"}, {{1}, {0}}, 0, {true, true});
        const auto universalMinimo = minimizar(universal);
        EXIGIR(universalMinimo.automato.quantidadeEstados() == 1);
        EXIGIR(universalMinimo.automato.aceita({"x", "x", "x"}));
    });
}
