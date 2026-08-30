
## Que classe de ações o sistema aceita

**Decisão:** a OGLanguage pertence ao domínio de jogos digitais e descreve ações de personagens controláveis do tipo `player`. O primeiro recorte aceita as ações `andar(direcao)`, `correr(direcao)`, `pular()`, `atacar()`, `defender()` e `parar()`. As direções válidas são `direita`, `esquerda`, `frente` e `tras`.

Além de ações simples, a linguagem aceita repetição finita por meio de `repetir(NUMERO)` e execução condicional por meio de `se(condicao)`. As condições iniciais são `inimigo_proximo`, `obstaculo_proximo`, `vida_baixa` e `no_chao`.

**Núcleo mínimo:** declaração de um `player` e execução sequencial de ações. Repetições e condições são estruturas de controle construídas sobre esse núcleo: `repetir` executa um bloco uma quantidade inteira e positiva de vezes, enquanto `se` executa um bloco somente quando a condição informada for verdadeira.

**Descartado:** comandos para criação de mapas, gráficos, animações, sons, física, armas, inventário, pontuação, inteligência artificial e comunicação em rede. Esses recursos pertencem à implementação do jogo ou à engine utilizada. Incluí-los transformaria a OGLanguage em uma linguagem de desenvolvimento de jogos completa, afastando-a de sua finalidade: descrever, de maneira formal, as ações de um `player`.

## Que forma tem a descrição escrita pelo usuário

**Decisão:** um programa é uma sequência de declarações `player`. Cada declaração associa um identificador a um bloco de comandos. Dentro desse bloco, os comandos são executados de cima para baixo e podem ser ações simples, repetições ou condições.


A gramática completa está em `gramatica.md`. Palavras reservadas são escritas em letras minúsculas; ações simples terminam com ponto e vírgula; parênteses delimitam argumentos; chaves delimitam blocos; identificadores não podem conter espaços; e comentários de uma linha começam com `//`.

**Descartado:** uma sintaxe formada apenas por ações soltas, sem declaração de `player`. Exigir um nome para o personagem permite que o analisador construa uma tabela de símbolos, detecte declarações duplicadas e deixe claro qual entidade executará cada sequência de comandos. Essa decisão também prepara a linguagem para programas com mais de um personagem.

## O que o sistema produz

**Decisão:** o compilador da OGLanguage analisa o programa, valida os símbolos e comandos e produz uma representação intermediária composta por instruções de ação e de controle. Cada instrução registra o `player`, a ação, seus argumentos e a ordem de execução. Uma máquina virtual própria percorre essas instruções e envia os comandos correspondentes à engine do jogo.

Exemplo conceitual de saída para `andar(direita);`:

```text
PLAYER heroi
ACTION ANDAR direita
```

Repetições são traduzidas para instruções de início, retorno e fim de laço. Condições são traduzidas para testes e desvios. Assim, o resultado não cria sozinho a animação ou a física do personagem; ele descreve de forma executável qual ação a engine deverá realizar.

**Descartado:** interpretar diretamente o texto-fonte sem produzir representação intermediária. Essa alternativa seria mais curta, mas esconderia as etapas de análise léxica, análise sintática, análise semântica e geração de código que o projeto deve demonstrar. A emissão de instruções torna explícita a passagem entre a descrição formal do comportamento e sua execução no jogo.

