#pragma once

#include "automato.h"

#include <memory>
#include <stdexcept>
#include <utility>

namespace ogl {

enum class TipoExpressao { Simbolo, Epsilon, Concatenacao, Alternancia, Estrela };

// AST imutavel do nucleo regular. Os atalhos + e ? sao reduzidos nas fabricas.
class Expressao {
public:
    static Expressao simbolo(Simbolo valor) {
        if (valor.empty()) throw std::invalid_argument("Simbolo da expressao nao pode ser vazio.");
        return Expressao(TipoExpressao::Simbolo, std::move(valor), {}, {});
    }

    static Expressao epsilon() { return Expressao(TipoExpressao::Epsilon, {}, {}, {}); }

    static Expressao concatenar(Expressao esquerda, Expressao direita) {
        return Expressao(TipoExpressao::Concatenacao, {}, guardar(std::move(esquerda)),
                         guardar(std::move(direita)));
    }

    static Expressao alternar(Expressao esquerda, Expressao direita) {
        return Expressao(TipoExpressao::Alternancia, {}, guardar(std::move(esquerda)),
                         guardar(std::move(direita)));
    }

    static Expressao estrela(Expressao interior) {
        return Expressao(TipoExpressao::Estrela, {}, guardar(std::move(interior)), {});
    }

    static Expressao mais(Expressao interior) {
        // x+ -> x,x*. A copia compartilha uma AST imutavel.
        auto repeticao = estrela(interior);
        return concatenar(std::move(interior), std::move(repeticao));
    }

    static Expressao opcional(Expressao interior) {
        // x? -> x|epsilon.
        return alternar(std::move(interior), epsilon());
    }

    TipoExpressao tipo() const noexcept { return tipo_; }
    const Simbolo& valor() const noexcept { return valor_; }
    const Expressao& esquerda() const { return exigir(esquerda_); }
    const Expressao& direita() const { return exigir(direita_); }

private:
    using No = std::shared_ptr<const Expressao>;

    Expressao(TipoExpressao tipo, Simbolo valor, No esquerda, No direita)
        : tipo_(tipo), valor_(std::move(valor)), esquerda_(std::move(esquerda)),
          direita_(std::move(direita)) {}

    static No guardar(Expressao expressao) { return No(new Expressao(std::move(expressao))); }

    static const Expressao& exigir(const No& no) {
        if (!no) throw std::logic_error("Este no da expressao nao possui esse filho.");
        return *no;
    }

    TipoExpressao tipo_;
    Simbolo valor_;
    No esquerda_;
    No direita_;
};

} // namespace ogl
