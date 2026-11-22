#include "minimizacao.h"

#include <algorithm>
#include <map>
#include <queue>
#include <utility>

namespace ogl {
namespace {

using Particao = std::vector<ConjuntoEstados>;

ConjuntoEstados idDosBlocos(std::size_t quantidade, const Particao& grupos) {
    ConjuntoEstados resultado(quantidade, SEM_ESTADO);
    for (Estado bloco = 0; bloco < grupos.size(); ++bloco) {
        for (const Estado estado : grupos[bloco]) resultado[estado] = bloco;
    }
    return resultado;
}

} // namespace

ResultadoMinimizacao minimizar(const AFD& afd) {
    const std::size_t estadosAntes = afd.quantidadeEstados();
    const std::size_t quantidadeSimbolos = afd.alfabeto().size();

    // Conclui transicoes ausentes com um estado de erro absorvente.
    auto tabelaCompleta = std::vector<ConjuntoEstados>(estadosAntes,
                                                       ConjuntoEstados(quantidadeSimbolos));
    bool precisaErro = false;
    for (Estado origem = 0; origem < estadosAntes; ++origem) {
        for (std::size_t coluna = 0; coluna < quantidadeSimbolos; ++coluna) {
            tabelaCompleta[origem][coluna] = afd.transicao(origem, afd.alfabeto()[coluna]);
            if (tabelaCompleta[origem][coluna] == SEM_ESTADO) precisaErro = true;
        }
    }
    if (precisaErro) {
        const Estado erro = tabelaCompleta.size();
        for (auto& linha : tabelaCompleta) {
            for (auto& destino : linha) if (destino == SEM_ESTADO) destino = erro;
        }
        tabelaCompleta.emplace_back(quantidadeSimbolos, erro);
    }
    std::vector<bool> finaisCompletos(estadosAntes + (precisaErro ? 1U : 0U), false);
    for (Estado estado = 0; estado < estadosAntes; ++estado) finaisCompletos[estado] = afd.ehFinal(estado);
    const std::size_t estadosCompletados = tabelaCompleta.size();

    // Descarta estados que nao podem ser visitados a partir do inicial.
    std::vector<bool> acessivel(estadosCompletados, false);
    std::queue<Estado> fila;
    acessivel[afd.inicial()] = true;
    fila.push(afd.inicial());
    while (!fila.empty()) {
        const Estado atual = fila.front();
        fila.pop();
        for (const Estado destino : tabelaCompleta[atual]) {
            if (!acessivel[destino]) {
                acessivel[destino] = true;
                fila.push(destino);
            }
        }
    }

    std::vector<Estado> mapa(estadosCompletados, SEM_ESTADO);
    std::size_t quantidadeAcessiveis = 0;
    for (Estado estado = 0; estado < estadosCompletados; ++estado) {
        if (acessivel[estado]) mapa[estado] = quantidadeAcessiveis++;
    }
    std::vector<ConjuntoEstados> tabelaAcessivel(quantidadeAcessiveis,
                                                 ConjuntoEstados(quantidadeSimbolos));
    std::vector<bool> finaisAcessiveis(quantidadeAcessiveis, false);
    for (Estado estado = 0; estado < estadosCompletados; ++estado) {
        if (!acessivel[estado]) continue;
        const Estado novo = mapa[estado];
        finaisAcessiveis[novo] = finaisCompletos[estado];
        for (std::size_t coluna = 0; coluna < quantidadeSimbolos; ++coluna) {
            tabelaAcessivel[novo][coluna] = mapa[tabelaCompleta[estado][coluna]];
        }
    }

    Particao grupos;
    ConjuntoEstados finais, naoFinais;
    for (Estado estado = 0; estado < quantidadeAcessiveis; ++estado) {
        (finaisAcessiveis[estado] ? finais : naoFinais).push_back(estado);
    }
    if (!naoFinais.empty()) grupos.push_back(std::move(naoFinais));
    if (!finais.empty()) grupos.push_back(std::move(finais));
    std::vector<Particao> historico{grupos};
    std::size_t rodadas = 0;

    while (true) {
        const auto blocoAtual = idDosBlocos(quantidadeAcessiveis, grupos);
        Particao refinada;
        for (const auto& grupo : grupos) {
            std::map<ConjuntoEstados, ConjuntoEstados> porAssinatura;
            for (const Estado estado : grupo) {
                ConjuntoEstados assinatura{blocoAtual[estado]};
                for (const Estado destino : tabelaAcessivel[estado]) {
                    assinatura.push_back(blocoAtual[destino]);
                }
                porAssinatura[assinatura].push_back(estado);
            }
            for (auto& [assinatura, equivalentes] : porAssinatura) {
                (void)assinatura;
                refinada.push_back(std::move(equivalentes));
            }
        }
        ++rodadas;
        if (refinada.size() == grupos.size()) break;
        grupos = std::move(refinada);
        historico.push_back(grupos);
    }

    const auto blocoFinal = idDosBlocos(quantidadeAcessiveis, grupos);
    std::vector<ConjuntoEstados> tabelaMinima(grupos.size(),
                                               ConjuntoEstados(quantidadeSimbolos));
    std::vector<bool> finaisMinimos(grupos.size(), false);
    for (Estado bloco = 0; bloco < grupos.size(); ++bloco) {
        const Estado representante = grupos[bloco].front();
        finaisMinimos[bloco] = finaisAcessiveis[representante];
        for (std::size_t coluna = 0; coluna < quantidadeSimbolos; ++coluna) {
            tabelaMinima[bloco][coluna] = blocoFinal[tabelaAcessivel[representante][coluna]];
        }
    }
    const Estado inicialMinimo = blocoFinal[mapa[afd.inicial()]];
    AFD minimo(afd.alfabeto(), std::move(tabelaMinima), inicialMinimo,
               std::move(finaisMinimos));
    return {std::move(minimo), estadosAntes, estadosCompletados,
            estadosCompletados - quantidadeAcessiveis, rodadas, std::move(historico)};
}

} // namespace ogl
