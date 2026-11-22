# Mapa de progresso — OGLanguage

Atualizado em 22/11/2026. Antes da próxima etapa, releia `Data/Sources.md`: ele define o escopo acadêmico vigente. A pasta `Data/` é ignorada pelo Git; preserve a fonte usada ao compartilhar o trabalho.

## Estado atual

O recorte pedido para o módulo 4 e o início do módulo 5 está implementado em `Projeto/`:

- AST regular imutável e construção recursiva de Thompson.
- Reduções dos atalhos `+` e `?` para `x,x*` e `x|ε`.
- AFN com fecho ε, movimento, avanço, aceitação e simulação.
- Determinização por subconjuntos, guardando o subconjunto AFN associado a cada estado AFD e incluindo o conjunto vazio.
- Minimização: completar transições, remover estados inalcançáveis, refinar partições até o ponto fixo e construir o AFD mínimo.
- Relatório das contagens, comparação das três máquinas em 121 cadeias e experimento de crescimento.
- Passo a passo Markdown para as contas que o grupo deve fazer à mão, além de ficha preenchível.

Verificação executada em 22/11/2026 com `powershell -File Projeto/verificar.ps1`: compilação C++20 aprovada e CTest **6/6 suítes**. O comando `ogl estudo` reportou Thompson 10/12/9; AFD 5 estados; minimização 5 antes e 3 depois; 121 cadeias, 15 aceitas e zero divergências.

## Requisitos e fontes

`Data/Sources.md` solicita concluir Thompson pela árvore sintática, simular AFN, conferir casos à mão, implementar determinização/minimização, comparar as três máquinas e registrar um exemplo de crescimento. `Data/Skills.md` descreve uma organização C++/CMake, embora o título mencione C#.

O guia oficial do módulo 4 confirma estes custos (estados, transições totais, transições ε): `ab` = 4/3/1; `a|b` = 6/6/4; `a*` = 4/5/4; `(a|b)*c` = 10/12/9. Também confirma que a simulação começa pelo fecho ε do estado inicial e repete o fecho depois de cada movimento. Fontes consultadas: [guia do módulo 4](https://bcc-clfa-s.material.moacyr.pro.br/modulos/04/04_guia) e [resumo do módulo 4](https://bcc-clfa-s.material.moacyr.pro.br/modulos/04/04_material_resumo). Os requisitos detalhados do início do módulo 5 vieram de `Sources.md`.

## Arquitetura e fluxo

```text
Expressao (AST)
      │ thompson::construir
      ▼
     AFN ─────────────── simulação de uma cadeia
      │ determinizar
      ▼
     AFD ─────────────── completar + remover inacessíveis
      │ minimizar
      ▼
  AFD mínimo
```

As responsabilidades são separadas: a AST descreve o padrão; Thompson cria o AFN; a simulação segue todos os caminhos; a determinização guarda conjuntos como estados; a minimização agrupa estados indistinguíveis. Os casos do curso usam `{a,b,c}`. As demonstrações agrícolas mantêm cada nome de evento, como `chuva`, como um símbolo indivisível.

## Mapa de arquivos

### Interfaces — `Projeto/include/`

- `automato.h`: tipos compartilhados, alfabeto agrícola, `EPSILON`, `SEM_ESTADO` e validação do alfabeto.
- `expressao.h`: AST imutável `Expressao`: símbolo, ε, concatenação, alternância e estrela. As fábricas `mais` e `opcional` aplicam as reduções do núcleo.
- `afn.h`: representação, traço e operações do AFN.
- `afd.h`: tabela AFD total ou parcial; `SEM_ESTADO` indica uma transição ausente e `ehCompleto()` detecta lacunas.
- `thompson.h`: operações de baixo nível e `construir(const Expressao&, const Alfabeto&)`.
- `determinizacao.h`: resultado com AFD e correspondência de subconjuntos.
- `minimizacao.h`: resultado com AFD mínimo, contagens e histórico das partições.

### Implementações — `Projeto/src/`

- `afn.cpp`: fecho ε com visitados, movimento, avanço, aceitação e simulação.
- `afd.cpp`: validação, transições, reconhecimento e detecção de erro absorvente.
- `thompson.cpp`: símbolos, ε, concatenação, alternância, estrela e descida recursiva pela AST.
- `determinizacao.cpp`: catálogo de subconjuntos ordenados; explora os alcançáveis e inclui `∅`.
- `minimizacao.cpp`: completa lacunas com erro, percorre estados alcançáveis, cria partição final/não final, refina assinaturas e monta a tabela mínima.

### Aplicação e suporte

- `Projeto/main.cpp`: demonstração agrícola, AFD manual anterior e comando `ogl estudo`.
- `Projeto/tests/test_afn.cpp`, `test_afd.cpp`, `test_thompson.cpp`, `test_determinizacao.cpp` e `test_minimizacao.cpp`: testes dos módulos e casos de referência.
- `Projeto/tests/test_interface.cmake`: checa a interface e o relatório de estudo.
- `Projeto/CMakeLists.txt`: biblioteca, aplicação e seis suítes CTest.
- `Projeto/verificar.ps1`: um comando para configurar, compilar e executar todos os testes.
- `Projeto/PASSO_A_PASSO_MANUAL.md`: sequência para as atividades manuais.
- `Projeto/REGISTROS_MANUAIS.md`: ficha de desenhos, contagens, vereditos e conferência.
- `Projeto/README.md`: arquitetura, API, comandos, resultados medidos e limites.

## Decisões que devem ser preservadas

- A fonte diz que já existe uma árvore, mas não havia parser/AST dentro de `Projeto/`. Foi criado o menor tipo de AST compatível com a gramática do repositório. O exemplo e a demonstração agrícola agora também partem dessa AST. Parser textual não foi acrescentado.
- A AST é imutável e pode compartilhar subárvores. A construção recursiva cria estados novos para cada ocorrência.
- O núcleo é símbolo, ε, concatenação, alternância e estrela. `+` reduz para concatenação com estrela; `?` reduz para alternância com ε.
- `EPSILON` é rótulo interno de transição do AFN, não pertence ao alfabeto nem à cadeia de entrada.
- Fecho ε inclui os estados iniciais e marca estados visitados para terminar em ciclos.
- Conjuntos de estados são ordenados e sem duplicatas. A construção de subconjuntos usa `std::map` para catálogo estável.
- O conjunto vazio é estado absorvente do AFD completo.
- O AFD pode representar transições parciais com `SEM_ESTADO`; a minimização cria o estado de erro antes de descartar inacessíveis.
- A assinatura usada no refinamento inclui o bloco atual e o bloco destino de cada símbolo, na ordem do alfabeto. A parada ocorre quando nenhuma classe se divide.
- A bateria compara cada máquina com o veredito esperado, incluindo a máquina intermediária AFD.
- Resultados manuais e resultados do programa ficam em colunas diferentes. O grupo ainda deve calcular e preencher as contas à mão.

## Resultados observados

### `(a|b)*c` sobre `{a,b,c}`

- Thompson: 10 estados, 12 transições, 9 transições ε.
- `ababac` é aceita e `abab` é recusada.
- AFD por subconjuntos completo: 5 estados, contando o conjunto vazio/erro.
- Minimização: 5 estados antes e 3 depois.
- Até comprimento 4: 121 cadeias, 15 aceitas, zero divergências entre AFN, AFD e mínimo.

### Crescimento: o n-ésimo símbolo a partir do fim é `a`

Para cada `n`, o padrão é `(a|b)*a` seguido de `n−1` ocorrências de `(a|b)`; o alfabeto é `{a,b}`. As contagens abaixo foram impressas por `ogl estudo`:

| n | Estados AFN | AFD completo | AFD mínimo |
| ---: | ---: | ---: | ---: |
| 1 | 10 | 3 | 2 |
| 2 | 16 | 5 | 4 |
| 3 | 22 | 9 | 8 |
| 4 | 28 | 17 | 16 |
| 5 | 34 | 33 | 32 |
| 6 | 40 | 65 | 64 |

## Etapa manual a cargo do grupo

A implementação foi conferida, mas os cálculos pedidos como atividade manual devem ser feitos por vocês. Use [`Projeto/PASSO_A_PASSO_MANUAL.md`](Projeto/PASSO_A_PASSO_MANUAL.md) e registre em [`Projeto/REGISTROS_MANUAIS.md`](Projeto/REGISTROS_MANUAIS.md): árvore e Thompson, fechos/movimentos, catálogo de subconjuntos, 121 vereditos, refinamento e interpretação do crescimento. Execute o programa somente depois de registrar os vereditos previstos.

## Limites e continuidade

O escopo atual não inclui parser textual `.ogl`, compilação de `equipamento/quando`, seleção de regras no fluxo, execução de ações ou máquina virtual. Uma etapa futura de parser deve seguir `docs/01_gramatica.md` e `docs/02_arvore_sintatica.md`, sem misturar parsing à construção dos autômatos.

## Comandos

Na raiz do repositório:

```powershell
powershell -File Projeto/verificar.ps1
.\Projeto\build\ogl.exe estudo
.\Projeto\build\ogl.exe thompson Projeto/examples/eventos_aceitos.txt
.\Projeto\build\ogl.exe afd
```

`Projeto/build/` é gerada e ignorada pelo Git. Confira `git status` antes de encerrar novas etapas. A pasta `Data/` também é ignorada e precisa ser distribuída separadamente se a equipe depender de `Sources.md` e `Skills.md`.
