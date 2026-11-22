#include "thompson.h"
#include "test_utils.h"

using namespace ogl;
using namespace ogl::thompson;

namespace {

void conferirPontas(const AFN& afn) {
    std::size_t finais = 0;
    Estado final = SEM_ESTADO;
    for (Estado q = 0; q < afn.quantidadeEstados(); ++q) {
        if (afn.ehFinal(q)) { ++finais; final = q; }
        for (const auto& t : afn.saidas(q)) EXIGIR(t.destino != afn.inicial());
    }
    EXIGIR(finais == 1);
    EXIGIR(afn.saidas(final).empty());
}

} // namespace

int main() {
    return teste::executar([] {
        const Alfabeto canonico{"a", "b", "c"};
        const auto a = Expressao::simbolo("a");
        const auto b = Expressao::simbolo("b");
        const auto c = Expressao::simbolo("c");
        const auto canonica = Expressao::concatenar(
            Expressao::estrela(Expressao::alternar(a, b)), c);
        const auto automatoCanonico = construir(canonica, canonico);
        std::size_t transicoesVazias = 0;
        for (Estado q = 0; q < automatoCanonico.quantidadeEstados(); ++q) {
            for (const auto& transicao : automatoCanonico.saidas(q)) {
                if (transicao.simbolo == EPSILON) ++transicoesVazias;
            }
        }
        EXIGIR(automatoCanonico.quantidadeEstados() == 10);
        EXIGIR(automatoCanonico.quantidadeTransicoes() == 12);
        EXIGIR(transicoesVazias == 9);
        EXIGIR(automatoCanonico.aceita({"a", "b", "a", "b", "a", "c"}));
        EXIGIR(!automatoCanonico.aceita({"a", "b", "a", "b"}));
        EXIGIR(automatoCanonico.fechoEpsilon({automatoCanonico.inicial()}) ==
               ConjuntoEstados({0, 2, 4, 6, 7, 8}));

        const auto ab = construir(Expressao::concatenar(a, b), canonico);
        const auto alt = construir(Expressao::alternar(a, b), canonico);
        const auto estrela = construir(Expressao::estrela(a), canonico);
        EXIGIR(ab.quantidadeEstados() == 4 && ab.quantidadeTransicoes() == 3);
        EXIGIR(alt.quantidadeEstados() == 6 && alt.quantidadeTransicoes() == 6);
        EXIGIR(estrela.quantidadeEstados() == 4 && estrela.quantidadeTransicoes() == 5);

        const auto maisA = construir(Expressao::mais(a), canonico);
        const auto reduzMaisA = construir(
            Expressao::concatenar(a, Expressao::estrela(a)), canonico);
        const auto opcionalB = construir(Expressao::opcional(b), canonico);
        EXIGIR(maisA.quantidadeEstados() == reduzMaisA.quantidadeEstados());
        EXIGIR(maisA.quantidadeTransicoes() == reduzMaisA.quantidadeTransicoes());
        EXIGIR(maisA.aceita({"a"}) && maisA.aceita({"a", "a"}));
        EXIGIR(!maisA.aceita({}));
        EXIGIR(opcionalB.aceita({}) && opcionalB.aceita({"b"}));
        EXIGIR(!opcionalB.aceita({"b", "b"}));
        const auto epsilonAST = construir(Expressao::epsilon(), canonico);
        EXIGIR(epsilonAST.aceita({}) && !epsilonAST.aceita({"a"}));

        const auto chuva = simbolo("chuva");
        const auto solo = simbolo("solo_seco");
        const auto temperatura = simbolo("temperatura_alta");
        const auto vazio = epsilon();
        const auto sequencia = concatenar(solo, chuva);
        const auto alternativa = alternar(solo, chuva);
        const auto repeticao = fechoKleene(chuva);
        EXIGIR(chuva.quantidadeEstados() == 2);
        EXIGIR(chuva.quantidadeTransicoes() == 1);
        EXIGIR(vazio.quantidadeEstados() == 2);
        EXIGIR(vazio.quantidadeTransicoes() == 1);
        EXIGIR(sequencia.quantidadeEstados() == 4);
        EXIGIR(sequencia.quantidadeTransicoes() == 3);
        EXIGIR(alternativa.quantidadeEstados() == 6);
        EXIGIR(alternativa.quantidadeTransicoes() == 6);
        EXIGIR(repeticao.quantidadeEstados() == 4);
        EXIGIR(repeticao.quantidadeTransicoes() == 5);
        for (const auto* afn : {&chuva, &vazio, &sequencia, &alternativa, &repeticao}) {
            conferirPontas(*afn);
        }

        for (const auto& evento : ALFABETO_EVENTOS) {
            const auto folha = simbolo(evento);
            EXIGIR(folha.aceita({evento}));
            EXIGIR(!folha.aceita({}));
            EXIGIR(!folha.aceita({evento, evento}));
        }
        EXIGIR(vazio.aceita({}));
        EXIGIR(!vazio.aceita({EPSILON}));
        EXIGIR(!vazio.aceita({"chuva"}));
        EXIGIR(sequencia.aceita({"solo_seco", "chuva"}));
        EXIGIR(!sequencia.aceita({"chuva", "solo_seco"}));
        EXIGIR(!sequencia.aceita({"solo_seco"}));
        EXIGIR(!sequencia.aceita({"solo_seco", "chuva", "chuva"}));
        EXIGIR(alternativa.aceita({"solo_seco"}));
        EXIGIR(alternativa.aceita({"chuva"}));
        EXIGIR(!alternativa.aceita({}));
        EXIGIR(!alternativa.aceita({"solo_seco", "chuva"}));
        EXIGIR(alternativa.fechoEpsilon({alternativa.inicial()}) == ConjuntoEstados({0, 2, 4}));
        EXIGIR(repeticao.aceita({}));
        EXIGIR(repeticao.aceita({"chuva"}));
        EXIGIR(repeticao.aceita({"chuva", "chuva", "chuva"}));
        EXIGIR(!repeticao.aceita({"chuva", "solo_seco"}));
        EXIGIR(!repeticao.aceita({"geada", "chuva"}));
        EXIGIR(!repeticao.aceita({EPSILON}));

        // Todos os vereditos abaixo seguem as linguagens, antes da execucao.
        const auto prefixoComum = alternar(sequencia, concatenar(solo, temperatura));
        EXIGIR(prefixoComum.aceita({"solo_seco", "chuva"}));
        EXIGIR(prefixoComum.aceita({"solo_seco", "temperatura_alta"}));
        EXIGIR(!prefixoComum.aceita({"solo_seco"}));
        const auto pares = fechoKleene(sequencia);
        EXIGIR(pares.aceita({}));
        EXIGIR(pares.aceita({"solo_seco", "chuva", "solo_seco", "chuva"}));
        EXIGIR(!pares.aceita({"chuva", "solo_seco"}));
        EXIGIR(!pares.aceita({"solo_seco", "chuva", "solo_seco"}));
        const auto estrelaAninhada = fechoKleene(repeticao);
        EXIGIR(estrelaAninhada.aceita({}));
        EXIGIR(estrelaAninhada.aceita({"chuva", "chuva"}));
        EXIGIR(estrelaAninhada.quantidadeEstados() == 6);
        EXIGIR(estrelaAninhada.quantidadeTransicoes() == 9);
        EXIGIR(fechoKleene(vazio).aceita({}));
        EXIGIR(!fechoKleene(vazio).aceita({"chuva"}));
        EXIGIR(concatenar(vazio, chuva).aceita({"chuva"}));
        EXIGIR(concatenar(chuva, vazio).aceita({"chuva"}));
        EXIGIR(alternar(vazio, chuva).aceita({}));

        const auto regra = concatenar(concatenar(solo, fechoKleene(alternar(temperatura, chuva))),
                                     simbolo("reservatorio_baixo"));
        EXIGIR(regra.quantidadeEstados() == 12);
        EXIGIR(regra.quantidadeTransicoes() == 14);
        conferirPontas(regra);
        EXIGIR(regra.aceita({"solo_seco", "reservatorio_baixo"}));
        EXIGIR(regra.aceita({"solo_seco", "temperatura_alta", "chuva", "reservatorio_baixo"}));
        EXIGIR(!regra.aceita({}));
        EXIGIR(!regra.aceita({"solo_seco", "chuva"}));
        EXIGIR(!regra.aceita({"solo_seco", "reservatorio_baixo", "cultura_madura"}));
        EXIGIR(!regra.aceita({"chuva", "solo_seco", "reservatorio_baixo"}));
        EXIGIR(regra.aceita(Cadeia{"solo_seco", "chuva", "chuva", "reservatorio_baixo"}));
        const auto antiga = concatenar(concatenar(solo, temperatura), temperatura);
        EXIGIR(antiga.aceita({"solo_seco", "temperatura_alta", "temperatura_alta"}));
        EXIGIR(!antiga.aceita({"solo_seco", "temperatura_alta", "temperatura_alta", "chuva"}));
        EXIGIR(!antiga.aceita({"chuva", "solo_seco", "temperatura_alta", "temperatura_alta"}));

        // A composicao aceita AFN com varios finais e saidas a partir deles.
        AFN varios;
        const auto q0 = varios.adicionarEstado(true);
        const auto q1 = varios.adicionarEstado(true);
        varios.definirInicial(q0);
        varios.adicionarTransicao(q0, "chuva", q1);
        const auto variosDepoisSolo = concatenar(varios, solo);
        EXIGIR(variosDepoisSolo.aceita({"solo_seco"}));
        EXIGIR(variosDepoisSolo.aceita({"chuva", "solo_seco"}));
        EXIGIR(!variosDepoisSolo.aceita({"chuva"}));
        EXIGIR(fechoKleene(varios).aceita({"chuva", "chuva"}));
        EXIGIR(alternar(varios, solo).aceita({}));

        // Operandos permanecem independentes depois de compor ou alterar copias.
        auto copia = chuva;
        copia.definirAceitacao(1, false);
        EXIGIR(chuva.aceita({"chuva"}));
        EXIGIR(!copia.aceita({"chuva"}));
        EXIGIR(chuva.quantidadeEstados() == 2);
        EXIGIR(chuva.quantidadeTransicoes() == 1);
        const auto binario = simbolo("a", {"a", "b"});
        EXIGIR(binario.aceita({"a"}));
        EXIGIR(!binario.aceita({"chuva"}));
        teste::exigirExcecao<std::invalid_argument>([] { simbolo("geada"); });
        teste::exigirExcecao<std::invalid_argument>([] { simbolo(EPSILON); });
        teste::exigirExcecao<std::invalid_argument>([&] { concatenar(chuva, binario); });
        teste::exigirExcecao<std::invalid_argument>([&] { alternar(chuva, binario); });
        teste::exigirExcecao<std::logic_error>([] { fechoKleene(AFN{}); });
    });
}
