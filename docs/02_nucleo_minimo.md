# Tarefa 1 — O núcleo mínimo da OGLanguage

Esta decisão parte do recorte fixado em `recorte.md` e da gramática registrada em `gramatica.md`. O critério é manter na árvore apenas construções que possuem significado próprio durante a execução. Diferenças que existem somente na escrita são normalizadas durante a análise.

## O núcleo

| Construção | Papel na árvore |
|---|---|
| `Programa` | Raiz que reúne uma ou mais declarações |
| `Player` | Associa um identificador a um bloco de comandos |
| `Bloco` | Preserva uma sequência ordenada de comandos |
| `Ação` | Representa qualquer ação simples e seus argumentos |
| `Repetição` | Executa um bloco uma quantidade positiva de vezes |
| `Condição` | Executa um bloco quando uma condição é verdadeira |

O analisador não cria um tipo de nó diferente para cada ação. `andar`, `correr`, `pular`, `atacar`, `defender` e `parar` convergem para `Ação`. O nome e o argumento ficam armazenados no próprio nó.

## Normalizações

| O usuário escreve | A árvore recebe |
|---|---|
| `andar(direita);` | `Acao(nome=andar, argumento=direita)` |
| `correr(frente);` | `Acao(nome=correr, argumento=frente)` |
| `pular();` | `Acao(nome=pular)` |
| `atacar();` | `Acao(nome=atacar)` |
| `{ atacar(); defender(); }` | `Bloco[Acao(atacar), Acao(defender)]` |
| `repetir(3) { atacar(); }` | `Repeticao(3, Bloco[Acao(atacar)])` |
| `se(no_chao) { pular(); }` | `Condicao(no_chao, Bloco[Acao(pular)])` |

Parênteses, chaves, vírgulas inexistentes, ponto e vírgula e palavras de ligação não sobrevivem na árvore. Eles orientam a leitura, mas sua função já está representada pela estrutura e pelos campos dos nós.

## O que não é redução

`repetir` e `se` permanecem no núcleo. Embora ambos sejam posteriormente traduzidos para saltos e testes no bytecode, não podem ser removidos durante a leitura sem antecipar a geração de código e perder a correspondência entre a árvore e o programa-fonte.

Também não reduzimos `correr` a várias ocorrências de `andar`. Velocidade e deslocamento são conceitos da engine, e a gramática não afirma que correr seja equivalente a andar duas ou mais vezes.

## Folhas léxicas

Os valores `ID`, `NUMERO`, direções e condições aparecem como atributos de nós, não como nós independentes. Comentários e espaços são descartados pelo analisador léxico.

## Decisões descartadas

**Um nó por ação.** Criar `Andar`, `Correr`, `Pular`, `Atacar`, `Defender` e `Parar` aumentaria os casos tratados pelo formatador, pelo verificador semântico e pelo gerador de bytecode sem acrescentar uma nova forma estrutural.

**Executar durante a leitura.** O analisador apenas reconhece e constrói a árvore. Executar ações nessa etapa impediria validação completa, serialização e geração de código intermediário.

**Aceitar comandos genéricos.** A forma `acao("voar")` facilitaria extensões, mas contrariaria o recorte, que fixa um conjunto fechado de ações. Uma ação desconhecida deve ser recusada.

## Como verificar

A saída prefixa canônica permite comparar árvores por texto. As ações abaixo têm detalhes diferentes, mas a mesma categoria estrutural:

```text
andar(direita);  -> (acao andar direita)
pular();         -> (acao pular)
```

O núcleo contém seis tipos de nó. Novas ações simples podem ser acrescentadas futuramente sem criar novos tipos estruturais.

