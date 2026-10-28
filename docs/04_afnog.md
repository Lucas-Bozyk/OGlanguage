# Módulo 4 — Autômatos não determinísticos da OGLanguage

Aplicação do Modulo 04

## Recorte e ligação com os módulos anteriores

O exemplo padrão é uma regra já prevista em [02_especificacao.md](02_especificacao.md):

```ogl
equipamento irrigador {
    quando solo_seco, (temperatura_alta | chuva)*, reservatorio_baixo -> parar();
}
```

Reconhece `solo_seco`, seguido de zero ou mais eventos `temperatura_alta` ou `chuva`, e termina com `reservatorio_baixo`. Os dois eventos das extremidades são obrigatórios. O AFN recebe **uma sequência candidata completa** e aceita apenas ao fim. O programa demonstra:

```text
padrão isolado → árvore normalizada → AFN de Thompson
                                      ↑ eventos → ACEITA / REJEITADA
```

O módulo 2 tinha a árvore e a normalização especificadas em documentos, mas ainda não tinha implementação. Por isso este exemplo inclui `LeitorPadrao`, o leitor necessário para entregar uma árvore a Thompson. Ele aceita os padrões da gramática vigente: eventos, agrupamento, concatenação explícita `,`, alternância `|` e pós-fixos `*`, `+`, `?`. Respeita precedência, associação à esquerda, espaços e comentários `//`. Cada primária admite no máximo um pós-fixo: `chuva**` continua inválido; `(chuva*)*` é válido.

O leitor valida o alfabeto e informa o primeiro erro com deslocamento a partir de 0 e linha/coluna a partir de 1. As posições se referem ao **padrão isolado**. Não lê declarações de equipamentos, ações ou arquivos `.ogl` completos. A procura de trechos no fluxo, o reinício após disparos e a emissão de comandos continuam previstos para o executor futuro. Aceitar a cadeia vazia aqui é um resultado de reconhecimento e não emite ações.

## Tarefa 1: construção de Thompson

A árvore contém os cinco tipos de [02_nucleo_minimo.md](02_nucleo_minimo.md): `Simbolo`, `Epsilon`, `Concatenacao`, `Alternancia` e `Estrela`. O leitor reduz `a+` a `a,a*` e `a?` a `a|ε`. `ε` é interno e não é um evento digitável. As subárvores são imutáveis; a redução de `+` compartilha a referência ao operando, mas a construção gera estados distintos para cada ocorrência.

`AFN::construir` percorre a árvore recursivamente. Cada chamada devolve um `Bloco` com uma entrada e uma saída:

| Nó | Construção |
| --- | --- |
| Símbolo | dois estados ligados pelo evento |
| Épsilon | dois estados ligados por ε |
| Concatenação | saída do primeiro ligada por ε à entrada do segundo |
| Alternância | nova entrada com ε para os dois ramos; saídas dos ramos ligadas por ε à nova saída |
| Estrela | nova entrada/saída com caminhos ε para entrar, pular, repetir e sair do bloco |

Cada estado guarda uma lista de transições `(evento, destino)`. `EPSILON = -1` identifica as transições que não consomem entrada; os seis eventos usam índices de 0 a 5. Não existe um estado de erro explícito: uma transição ausente devolve nenhum destino.

Para o exemplo padrão:

```text
N = (Q, Σ, Δ, q0, F)
Q = {q0, q1, ..., q11}
Σ = {solo_seco, temperatura_alta, chuva,
     reservatorio_baixo, obstaculo_detectado, cultura_madura}
F = {q11}
Δ: Q × (Σ ∪ {ε}) → P(Q)
```

As transições não vazias da função Δ são:

| Origem | Rótulo | Destinos |
| --- | --- | --- |
| q0 | solo_seco | {q1} |
| q1 | ε | {q8} |
| q2 | temperatura_alta | {q3} |
| q3 | ε | {q7} |
| q4 | chuva | {q5} |
| q5 | ε | {q7} |
| q6 | ε | {q2, q4} |
| q7 | ε | {q6, q9} |
| q8 | ε | {q6, q9} |
| q9 | ε | {q10} |
| q10 | reservatorio_baixo | {q11} |

Todos os pares omitidos devolvem `∅`, inclusive as saídas de q11. São 12 estados e 14 transições individuais, das quais 10 são ε.

## Tarefa 2: simulação por conjuntos

O AFD do módulo 3 guardava um único estado. O AFN guarda um conjunto `ativos`, representado por `vector<unsigned char>` com uma marca por estado. Um estado só aparece uma vez, mesmo se vários caminhos chegam nele, e a impressão segue a ordem numérica dos estados.

`fechoVazio` acrescenta todos os estados alcançáveis por zero ou mais transições ε, sem consumir eventos. Usa uma pilha de estados pendentes e a própria marcação do conjunto para não revisitar estados. Assim, até ciclos formados só por ε, como os de `(chuva?)*`, terminam.

```text
S0 = fechoVazio({inicial})
Si = fechoVazio(união de Δ(q, evento_i) para q em S(i-1))
aceita ⇔ Sn contém o estado final
```

O fecho é calculado antes do primeiro evento e depois de cada movimento. Percorrer um caminho de aceitação antes do fim não basta. Quando todos os caminhos morrem, o conjunto fica vazio e os eventos seguintes não o reativam. Eventos fora do alfabeto também esvaziam o conjunto; o índice `-1` de evento desconhecido é tratado antes da consulta, para nunca ser confundido com ε.

Este é o traço de [04_eventos.txt](../examples/04_eventos.txt), após a descrição do padrão e do AFN:

```text
Inicio: {q0}
Apos solo_seco: {q1, q2, q4, q6, q8, q9, q10}
Apos temperatura_alta: {q2, q3, q4, q6, q7, q9, q10}
Apos chuva: {q2, q4, q5, q6, q7, q9, q10}
Apos reservatorio_baixo: {q11}
Maior conjunto ativo: 7
ACEITA
```

Depois de `solo_seco`, q10 já está ativo porque a estrela pode fazer zero repetições. q2 e q4 também estão ativos porque ela pode consumir uma temperatura ou uma chuva. A simulação mantém essas possibilidades juntas. Em [04_eventos_recusa.txt](../examples/04_eventos_recusa.txt), o evento extra `cultura_madura` esvazia `{q11}` e o resultado é `REJEITADA`.

## Compilar, executar e verificar

A partir da raiz do projeto, com GCC no PATH:

```powershell
g++ -std=c++11 -Wall -Wextra -Werror -pedantic docs/04_afnog.cpp -o 04_afnog.exe
.\04_afnog.exe
.\04_afnog.exe examples/04_eventos_recusa.txt
"solo_seco reservatorio_baixo" | .\04_afnog.exe -
"chuva chuva" | .\04_afnog.exe - "chuva+"
.\04_afnog.exe --testes
```

Sem argumentos, usa o padrão demonstrado e `examples/04_eventos.txt`. O primeiro argumento é o arquivo de eventos ou `-` para entrada padrão; o segundo, opcional, substitui o padrão. Mantenha o padrão entre aspas para proteger seus operadores do shell. Eventos são separados por espaços, tabulações ou quebras de linha, sem vírgulas e sem comentários. A comparação diferencia maiúsculas de minúsculas. A saída completa padrão está em [04_saida_esperada.txt](../examples/04_saida_esperada.txt).

Um único comando recompila e executa a bateria, inclusive se chamado de outra pasta:

```powershell
powershell -File examples/04_verificar.ps1
```

O script [04_verificar.ps1](../examples/04_verificar.ps1) interrompe com erro se a compilação ou os testes falharem. O executável gerado na raiz é ignorado pelo Git. Foi mantido C++11 para acompanhar o módulo 3 e a configuração atual do projeto; não há dependências externas.

## Tarefa 3: casos com veredito independente

[04_casos.txt](../examples/04_casos.txt) contém **71 casos**, com grupos comentados que explicam os resultados determinados à mão. Cada linha usa três campos separados por TAB: padrão, cadeia e veredito (`ACEITA`, `REJEITADA` ou `ERRO_PADRAO`). O campo de cadeia vazio representa zero eventos; não escreva `epsilon` nele. É possível usar outra bateria com `--testes caminho`.

Os casos cobrem os seis eventos, concatenação, alternância inicial e com prefixo comum, zero/uma/várias repetições, `+`, `?`, precedência, agrupamento, aninhamento, ciclos ε, aceitação seguida de evento extra, conjunto vazio absorvente, evento desconhecido e erros de padrão. Exemplos de justificativas:

| Padrão | Entrada | Resultado e razão |
| --- | --- | --- |
| `chuva*` | vazia | aceita zero repetições |
| `chuva+` | vazia | recusa; exige pelo menos uma chuva |
| `chuva?,solo_seco` | `solo_seco` | aceita pulando o opcional |
| `chuva*,chuva` | `chuva` | aceita com zero chuvas na estrela e uma no final |
| `(chuva?)*` | vazia | aceita e termina apesar do ciclo ε |
| `solo_seco,chuva\|solo_seco,temperatura_alta` | `solo_seco temperatura_alta` | aceita pelo segundo ramo, embora o primeiro morra |
| `chuva` | `solo_seco chuva` | recusa; não reinicia após perder todos os caminhos |

A bateria também verifica separadamente a igualdade estrutural entre as árvores normalizadas de `chuva+` e `chuva,chuva*`. Resultado verificado:

```text
Bateria: 71/71 casos passaram.
Normalizacao de chuva+: OK
```

As falhas indicam arquivo, linha, veredito esperado e obtido, sem parar no primeiro caso. Arquivo ausente, vazio, linha malformada e veredito desconhecido fazem a verificação falhar. Em execução comum, aceitar ou recusar uma cadeia retorna 0, como no módulo 3; erro de padrão, uso ou leitura retorna 1. Na bateria, qualquer divergência retorna 1.

## Decisões e custo

- **Listas de transições:** guardam só as arestas existentes. A matriz total do AFD anterior reservaria também muitas entradas vazias, e cada par agora pode ter vários destinos.
- **Conjunto por marcas:** dispensa nós de `std::set`, buscas e duplicatas. Em troca, percorre marcas de estados inativos. Neste exemplo pequeno, a representação deixa a união e o fecho explícitos.
- **Construção por blocos:** cada caso usa apenas entrada e saída dos filhos. A concatenação mantém os estados e acrescenta ε, em vez de fundi-los e depender dos detalhes internos.
- **Árvore antes da máquina:** os cinco casos de Thompson seguem diretamente os cinco construtores do núcleo. A implementação ficou sistemática porque cada composição reaproveita a mesma interface de bloco. Isso não elimina a necessidade dos testes de transcrição.
- **Simulação direta:** conjuntos são calculados durante a leitura; não são armazenados como estados de um AFD. A determinização pertence à etapa posterior.

Para uma árvore normalizada com `m` ocorrências de nós, a construção ocupa tempo e espaço `O(m)`. Essa contagem considera as ocorrências expandidas pelas reduções de `+`, mesmo quando compartilham uma referência. Se o AFN tem `Q` estados, `T` transições e a entrada tem `n` eventos, a simulação custa `O((n + 1)(Q + T))` e usa `O(Q)` de memória auxiliar, além do token de entrada e da máquina. O traço é escrito diretamente, sem guardar o histórico. A leitura também depende do número de caracteres; a saída do traço depende do tamanho dos conjuntos impressos.
