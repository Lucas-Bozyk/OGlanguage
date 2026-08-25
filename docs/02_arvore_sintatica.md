# Tarefa 2 — Leitura e conversão em árvore

O analisador da OGLanguage é recursivo-descendente e possui uma função para cada produção estrutural da gramática: programa, declaração de `player`, bloco, comando, ação, repetição e condição.

## Correspondência com a gramática

```text
programa          -> programa()
declaracaoPlayer  -> player()
bloco             -> bloco()
comando           -> comando()
acaoSimples       -> acao()
repeticao         -> repeticao()
condicional       -> condicional()
```

O analisador léxico transforma caracteres em tokens e registra, para cada token, deslocamento, linha e coluna. O analisador sintático consome os tokens e constrói a árvore reduzida ao núcleo definido na Tarefa 1.

## Exemplo

Entrada:

```oglanguage
player heroi {
    andar(direita);
    repetir(2) {
        atacar();
    }
}
```

Forma prefixa:

```text
(programa
  (player heroi
    (bloco
      (acao andar direita)
      (repetir 2
        (bloco
          (acao atacar))))))
```

## Representação

Os nós vivem em um vetor e são referenciados por índice. Cada nó contém:

- Tipo estrutural;
- Valor principal, como nome do `player`, ação, condição ou número;
- Argumento opcional, usado por movimentos;
- Lista ordenada de filhos;
- Posição inicial no código-fonte.

Essa representação evita gerenciamento manual de ponteiros, permite copiar a árvore e preserva a ordem dos comandos.

## Verificações da tarefa

O programa de demonstração confirma que:

1. Um programa válido produz uma árvore;
2. Todas as ações simples usam o mesmo tipo de nó;
3. Blocos aninhados aparecem como filhos de `Repetição` e `Condição`;
4. A forma prefixa é determinística e pode ser comparada em testes.

## Onde é fácil errar

É incorreto executar comandos durante a análise ou representar chaves e parênteses como nós. Esses símbolos pertencem à sintaxe concreta. Na árvore abstrata, a hierarquia dos nós já representa agrupamento e ordem.

