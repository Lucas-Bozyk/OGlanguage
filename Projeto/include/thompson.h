#pragma once

#include "afn.h"
#include "expressao.h"

namespace ogl::thompson {

// Operacoes de baixo nivel sobre automatos e conversao recursiva da AST.
AFN simbolo(const Simbolo& evento, const Alfabeto& alfabeto = ALFABETO_EVENTOS);
AFN epsilon(const Alfabeto& alfabeto = ALFABETO_EVENTOS);
AFN concatenar(const AFN& primeiro, const AFN& segundo);
AFN alternar(const AFN& primeiro, const AFN& segundo);
AFN fechoKleene(const AFN& interior);
AFN construir(const Expressao& expressao, const Alfabeto& alfabeto = ALFABETO_EVENTOS);

} // namespace ogl::thompson
