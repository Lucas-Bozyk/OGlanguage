#include "determinizacao.h"
#include "expressao.h"
#include "test_utils.h"
#include "thompson.h"

#include <algorithm>

using namespace ogl;

namespace {

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

Expressao expressaoCanonica() {
    return Expressao::concatenar(
        Expressao::estrela(Expressao::alternar(Expressao::simbolo("a"),
                                              Expressao::simbolo("b"))),
        Expressao::simbolo("c"));
}

} // namespace

int main() {
    return teste::executar([] {
        const Alfabeto alfabeto{"a", "b", "c"};
        const auto afn = thompson::construir(expressaoCanonica(), alfabeto);
        const auto resultado = determinizar(afn);
        EXIGIR(resultado.automato.quantidadeEstados() == 5);
        EXIGIR(resultado.automato.ehCompleto());
        EXIGIR(resultado.subconjuntos.size() == 5);
        EXIGIR(resultado.subconjuntos[0] == ConjuntoEstados({0, 2, 4, 6, 7, 8}));
        EXIGIR(std::find(resultado.subconjuntos.begin(), resultado.subconjuntos.end(),
                         ConjuntoEstados{}) != resultado.subconjuntos.end());
        EXIGIR(resultado.automato.estadoDeErro() != SEM_ESTADO);

        std::vector<Cadeia> cadeias;
        Cadeia prefixo;
        enumerar(alfabeto, prefixo, 4, cadeias);
        EXIGIR(cadeias.size() == 121);
        std::size_t aceitas = 0;
        for (const auto& cadeia : cadeias) {
            const bool esperado = !cadeia.empty() && cadeia.back() == "c" &&
                std::all_of(cadeia.begin(), cadeia.end() - 1, [](const Simbolo& s) {
                    return s == "a" || s == "b";
                });
            const bool porAfn = afn.aceita(cadeia);
            const bool porAfd = resultado.automato.aceita(cadeia);
            EXIGIR(porAfn == esperado);
            EXIGIR(porAfd == esperado);
            if (esperado) ++aceitas;
        }
        EXIGIR(aceitas == 15);

        AFN epsilon({});
        const auto inicial = epsilon.adicionarEstado();
        const auto final = epsilon.adicionarEstado(true);
        epsilon.definirInicial(inicial);
        epsilon.adicionarTransicao(inicial, EPSILON, final);
        const auto soVazia = determinizar(epsilon);
        EXIGIR(soVazia.automato.aceita({}));
        EXIGIR(!soVazia.automato.aceita({"x"}));
        EXIGIR(soVazia.subconjuntos.size() == 1);
    });
}
