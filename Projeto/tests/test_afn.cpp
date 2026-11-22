#include "afn.h"
#include "test_utils.h"

using namespace ogl;

int main() {
    return teste::executar([] {
        AFN afn;
        teste::exigirExcecao<std::logic_error>([&] { afn.aceita({}); });
        const auto q0 = afn.adicionarEstado();
        const auto q1 = afn.adicionarEstado(true);
        const auto q2 = afn.adicionarEstado();
        const auto q3 = afn.adicionarEstado(true);
        const auto q4 = afn.adicionarEstado(); // Inacessivel.
        afn.definirInicial(q0);
        afn.adicionarTransicao(q0, EPSILON, q2);
        afn.adicionarTransicao(q2, EPSILON, q0); // Ciclo epsilon.
        afn.adicionarTransicao(q0, "chuva", q1);
        afn.adicionarTransicao(q0, "chuva", q3); // Dois destinos para o mesmo simbolo.
        afn.adicionarTransicao(q1, EPSILON, q3);
        afn.adicionarTransicao(q1, EPSILON, q3); // Aresta duplicada nao aumenta a relacao.

        EXIGIR(afn.alfabeto() == ALFABETO_EVENTOS);
        EXIGIR(afn.quantidadeEstados() == 5);
        EXIGIR(afn.quantidadeTransicoes() == 5);
        EXIGIR(afn.inicial() == q0);
        EXIGIR(afn.saidas(q0).size() == 3);
        EXIGIR(afn.fechoEpsilon({q2, q0, q2}) == ConjuntoEstados({q0, q2}));
        EXIGIR(afn.fechoEpsilon({q1}) == ConjuntoEstados({q1, q3}));
        EXIGIR(afn.fechoEpsilon({q4}) == ConjuntoEstados({q4}));
        EXIGIR(afn.fechoEpsilon({}).empty());
        EXIGIR(afn.mover({q0, q2, q0}, "chuva") == ConjuntoEstados({q1, q3}));
        EXIGIR(afn.mover({q0}, EPSILON).empty());
        EXIGIR(afn.mover({q0}, "geada").empty());
        EXIGIR(afn.avancar({q0, q2}, "chuva") == ConjuntoEstados({q1, q3}));
        EXIGIR(!afn.aceita({}));
        EXIGIR(afn.aceita({"chuva"}));
        EXIGIR(!afn.aceita({"chuva", "chuva"}));
        EXIGIR(!afn.aceita({"solo_seco", "chuva"}));
        EXIGIR(!afn.aceita({"geada", "chuva"}));
        EXIGIR(!afn.aceita({EPSILON}));

        const auto traco = afn.simular({"geada", "chuva"});
        EXIGIR(!traco.aceitou);
        EXIGIR(traco.passos.size() == 3);
        EXIGIR(traco.passos[0].evento == EPSILON);
        EXIGIR(traco.passos[0].ativos == ConjuntoEstados({q0, q2}));
        EXIGIR(traco.passos[1].ativos.empty());
        EXIGIR(traco.passos[2].ativos.empty());
        EXIGIR(afn.simular({"chuva"}).aceitou);
        afn.definirAceitacao(q2);
        EXIGIR(afn.aceita({})); // Inicial nao final, mas fecho inicial contem um final.
        afn.definirAceitacao(q2, false);
        EXIGIR(!afn.aceita({}));
        teste::exigirExcecao<std::out_of_range>([&] { afn.definirInicial(99); });
        teste::exigirExcecao<std::out_of_range>([&] { afn.definirAceitacao(99); });
        teste::exigirExcecao<std::out_of_range>([&] { afn.saidas(99); });
        teste::exigirExcecao<std::out_of_range>([&] { afn.ehFinal(99); });
        teste::exigirExcecao<std::out_of_range>([&] { afn.adicionarTransicao(q0, EPSILON, 99); });
        teste::exigirExcecao<std::out_of_range>([&] { afn.adicionarTransicao(99, EPSILON, q0); });
        teste::exigirExcecao<std::out_of_range>([&] { afn.fechoEpsilon({99}); });
        teste::exigirExcecao<std::out_of_range>([&] { afn.mover({99}, "chuva"); });
        teste::exigirExcecao<std::invalid_argument>([&] { afn.adicionarTransicao(q0, "geada", q1); });
        teste::exigirExcecao<std::invalid_argument>([] { AFN invalido({"chuva", "chuva"}); });
        teste::exigirExcecao<std::invalid_argument>([] { AFN invalido({EPSILON}); });

        AFN soEpsilon(Alfabeto{});
        soEpsilon.definirInicial(soEpsilon.adicionarEstado(true));
        EXIGIR(soEpsilon.aceita({}));
        EXIGIR(!soEpsilon.aceita({"chuva"}));
    });
}
