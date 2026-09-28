# Núcleo mínimo das expressões regulares

## Nós do núcleo

| Nó | Significado |
| --- | --- |
| `Simbolo` | um evento agrícola do alfabeto, como `solo_seco` |
| `Epsilon` | cadeia vazia |
| `Concatenacao` | sequência de duas linguagens |
| `Alternancia` | união de duas linguagens |
| `Estrela` | zero ou mais repetições |

`Programa`, `Equipamento`, `Regra` e `Ação` pertencem à AST hospedeira e não a este núcleo. O nó `Equipamento` guarda o nome e suas regras. Todas as ações compartilham o nó `Ação`, com nome e setor opcional: `irrigar(norte)` gera `Acao(nome=irrigar, setor=norte)` e `colher()` gera `Acao(nome=colher)`.

## Reduções

```text
a+ → a,a*
a? → a|ε
```

A normalização ocorre depois da leitura. A AST final não contém nós `Mais` ou `Opcional`.

Nas fórmulas, `a`, `b` e `c` representam padrões, não novos eventos do domínio. `ε` é uma representação interna; não pode ser escrito no fonte e não dispara comandos por si só.

## Precedência e associatividade

Da maior para a menor: pós-fixos; concatenação; alternância. Concatenação e alternância associam à esquerda.

```text
a,b|c → (alt (concat (simbolo a) (simbolo b)) (simbolo c))
```

## Convergência

`chuva+` e `chuva,chuva*` convergem para `(concat (simbolo chuva) (estrela (simbolo chuva)))`. A comparação estrutural deverá ser um teste obrigatório da implementação.

## Decisões descartadas

- executar durante a leitura;
- manter pontuação na AST abstrata;
- manter `+` e `?` no núcleo;
- aceitar retroreferências, pois não descrevem linguagens regulares em geral.
