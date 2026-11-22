#include "afd.h"
#include "test_utils.h"

using namespace ogl;

int main() {
    return teste::executar([] {
        // Mesma linguagem exata do exemplo do modulo 3; tabela total com erro.
        std::vector<ConjuntoEstados> tabela(5, ConjuntoEstados(6, 4));
        tabela[0][0] = 1;
        tabela[1][1] = 2;
        tabela[2][1] = 3;
        const AFD afd(ALFABETO_EVENTOS, tabela, 0, {false, false, false, true, false});
        EXIGIR(afd.alfabeto() == ALFABETO_EVENTOS);
        EXIGIR(afd.quantidadeEstados() == 5);
        EXIGIR(afd.inicial() == 0);
        EXIGIR(afd.estadoDeErro() == 4);
        EXIGIR(afd.ehFinal(3));
        EXIGIR(!afd.ehFinal(0));
        EXIGIR(afd.transicao(0, "solo_seco") == 1);
        EXIGIR(afd.transicao(0, "chuva") == 4);
        for (const auto& evento : ALFABETO_EVENTOS) EXIGIR(afd.transicao(4, evento) == 4);
        EXIGIR(afd.transicao(0, "geada") == SEM_ESTADO);
        EXIGIR(afd.transicao(0, EPSILON) == SEM_ESTADO);
        EXIGIR(afd.aceita({"solo_seco", "temperatura_alta", "temperatura_alta"}));
        EXIGIR(!afd.aceita({}));
        EXIGIR(!afd.aceita({"solo_seco", "temperatura_alta"}));
        EXIGIR(!afd.aceita({"solo_seco", "temperatura_alta", "temperatura_alta", "chuva"}));
        EXIGIR(!afd.aceita({"chuva", "solo_seco", "temperatura_alta", "temperatura_alta"}));
        EXIGIR(!afd.aceita({"geada", "solo_seco", "temperatura_alta", "temperatura_alta"}));
        const auto traco = afd.simular({"solo_seco", "temperatura_alta", "temperatura_alta"});
        EXIGIR(traco.aceitou);
        EXIGIR(traco.passos == ConjuntoEstados({0, 1, 2, 3}));
        EXIGIR(afd.simular({"geada", "solo_seco"}).passos ==
               ConjuntoEstados({0, SEM_ESTADO, SEM_ESTADO}));
        EXIGIR(!afd.simular({"geada"}).aceitou);

        const AFD universal({"chuva"}, {{0}}, 0, {true});
        EXIGIR(universal.aceita({}));
        EXIGIR(universal.aceita({"chuva", "chuva"}));
        EXIGIR(universal.estadoDeErro() == SEM_ESTADO);
        const AFD apenasVazia({}, {{}}, 0, {true});
        EXIGIR(apenasVazia.aceita({}));
        EXIGIR(!apenasVazia.aceita({EPSILON}));
        teste::exigirExcecao<std::out_of_range>([&] { afd.ehFinal(5); });
        teste::exigirExcecao<std::out_of_range>([&] { afd.transicao(5, "chuva"); });
        teste::exigirExcecao<std::invalid_argument>([] { AFD a({"chuva"}, {}, 0, {}); });
        teste::exigirExcecao<std::invalid_argument>([] { AFD a({"chuva"}, {{0}}, 0, {}); });
        teste::exigirExcecao<std::invalid_argument>([] { AFD a({"chuva"}, {{}}, 0, {false}); });
        teste::exigirExcecao<std::invalid_argument>([] { AFD a({EPSILON}, {{0}}, 0, {false}); });
        teste::exigirExcecao<std::invalid_argument>([] { AFD a({"chuva", "chuva"}, {{0, 0}}, 0, {false}); });
        teste::exigirExcecao<std::out_of_range>([] { AFD a({"chuva"}, {{1}}, 0, {false}); });
        teste::exigirExcecao<std::out_of_range>([] { AFD a({"chuva"}, {{0}}, 1, {false}); });
    });
}
