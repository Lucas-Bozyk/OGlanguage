# OGLanguage — autômatos dos módulos 4 e 5

Projeto C++20/CMake para estudar AFN, construção de Thompson, determinização e minimização por refinamento de partições. A construção recebe uma AST de expressão regular; não há parser textual nesta etapa. A gramática do grupo está em [`docs/01_gramatica.md`](../docs/01_gramatica.md) e a fonte de implementação está em `Data/Sources.md`.

O guia da disciplina confirma para `(a|b)*c`: AFN com 10 estados, 12 transições e 9 ε; AFD por subconjuntos com 5 estados completos; AFD mínimo com 3 estados; e 15 cadeias aceitas entre as 121 de comprimento até 4.

## Compilar e executar

Na raiz do repositório, com PowerShell e `g++` disponíveis:

```powershell
powershell -File Projeto/verificar.ps1
.\Projeto\build\ogl.exe estudo
```

O primeiro comando configura e compila o projeto e executa todas as seis suítes de CTest. O segundo imprime as contagens do caso canônico, compara AFN/AFD/mínimo nas 121 cadeias e mede a família de padrões de crescimento.

Para executar as demonstrações anteriores:

```powershell
.\Projeto\build\ogl.exe
.\Projeto\build\ogl.exe thompson Projeto/examples/eventos_aceitos.txt
.\Projeto\build\ogl.exe thompson Projeto/examples/eventos_recusados.txt
.\Projeto\build\ogl.exe afd
.\Projeto\build\ogl.exe --help
```

O modo padrão e `thompson` reconhecem sequências de eventos agrícolas; `afd` executa o AFD manual da etapa anterior. `estudo` usa o alfabeto `{a,b,c}`. Aceitação e recusa são resultados normais e retornam código 0; erro de uso, arquivo ou compilação retorna código diferente de zero.

## Organização e responsabilidades

| Arquivo | Responsabilidade |
| --- | --- |
| `include/automato.h` | Tipos compartilhados, alfabeto e marcas `EPSILON`/`SEM_ESTADO`. |
| `include/expressao.h` | AST imutável: símbolo, ε, concatenação, alternância e estrela; `+` e `?` são reduzidos às formas do núcleo. |
| `include/afn.h`, `src/afn.cpp` | Representação, fecho ε, movimento, avanço e simulação do AFN. |
| `include/afd.h`, `src/afd.cpp` | Tabela de transições do AFD; aceita lacunas marcadas por `SEM_ESTADO` para permitir completar durante a minimização. |
| `include/thompson.h`, `src/thompson.cpp` | Construtores de baixo nível e conversão recursiva da AST em AFN. |
| `include/determinizacao.h`, `src/determinizacao.cpp` | Construção de subconjuntos e mapa dos estados do AFD aos conjuntos do AFN. |
| `include/minimizacao.h`, `src/minimizacao.cpp` | Completar, remover inacessíveis, refinar partições e montar o AFD mínimo. |
| `main.cpp` | Demonstrações agrícolas e relatório integrado `ogl estudo`. |
| `tests/test_*.cpp`, `tests/test_interface.cmake` | Casos de AFN, Thompson, determinização, minimização e interface. |
| `PASSO_A_PASSO_MANUAL.md` | Procedimento sequencial para as contas que o grupo deve fazer à mão. |
| `REGISTROS_MANUAIS.md` | Ficha para desenhos, contagens, vereditos e conferência pós-execução. |

`EPSILON` rotula transições que não consomem entrada e não pertence ao alfabeto. A cadeia vazia é `Cadeia{}`. Os conjuntos de estados do AFN são ordenados e sem duplicatas. Na determinização, o conjunto vazio é guardado como estado de erro para tornar o AFD completo.

## Interface da AST

```cpp
using namespace ogl;
const auto padrao = Expressao::concatenar(
    Expressao::estrela(Expressao::alternar(Expressao::simbolo("a"),
                                           Expressao::simbolo("b"))),
    Expressao::simbolo("c"));
const auto afn = thompson::construir(padrao, {"a", "b", "c"});
```

Os atalhos documentados são fábricas da AST: `Expressao::mais(x)` gera `x,x*`; `Expressao::opcional(x)` gera `x|ε`. Toda folha símbolo é validada pelo alfabeto passado a `construir`. A árvore é imutável e pode compartilhar filhos; a construção gera estados próprios para cada ocorrência.

## Resultados medidos

O comando `ogl estudo` mede o padrão construído em `main.cpp`: `(a|b)*a` seguido de `n−1` ocorrências de `(a|b)`. A linguagem exige que o n-ésimo símbolo a partir do fim seja `a`:

| n | Estados AFN | Estados AFD completo | Estados mínimos |
| ---: | ---: | ---: | ---: |
| 1 | 10 | 3 | 2 |
| 2 | 16 | 5 | 4 |
| 3 | 22 | 9 | 8 |
| 4 | 28 | 17 | 16 |
| 5 | 34 | 33 | 32 |
| 6 | 40 | 65 | 64 |

Para `(a|b)*c`, a comparação percorreu todas as 121 cadeias de comprimento até 4: cada máquina aceitou as mesmas 15 e houve zero divergências. A tabela de crescimento registra exatamente as contagens impressas pelo programa nesta execução; pode ser reproduzida com `ogl estudo`.


## Limites desta etapa

O projeto ainda não lê a sintaxe textual de `.ogl`, não constrói a árvore por parser, não compila declarações `equipamento/quando`, não executa ações e não inclui máquina virtual. O trabalho atual cobre os autômatos pedidos no módulo 4 e a determinização/minimização especificadas para o início do módulo 5. O mapa do projeto e próximos passos estão em [`progress.md`](../progress.md).
