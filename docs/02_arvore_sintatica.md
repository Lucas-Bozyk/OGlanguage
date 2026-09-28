# Leitura e conversão de expressão em árvore

O parser de padrões previsto é recursivo-descendente e deverá implementar `alternacao`, `concatenacao`, `posfixa` e `primaria`. O lexer deverá registrar deslocamento, linha e coluna. Depois do parse, `+` e `?` serão reduzidos ao núcleo.

Entrada:

```text
solo_seco,(temperatura_alta|chuva)*
```

Forma prefixa:

```text
(concat (simbolo solo_seco)
  (estrela (alt (simbolo temperatura_alta) (simbolo chuva))))
```

O parser da OGLanguage será separado: lerá `equipamento`, `quando`, `->` e ações e entregará a fatia do padrão ao parser regular, preservando suas posições no fonte completo. Pontuação não vira nó. Os testes previstos deverão verificar precedência, associatividade, agrupamento, normalização e forma prefixa estável.

Na AST hospedeira, `equipamento estufa { quando solo_seco -> irrigar(norte); }` deverá produzir:

```text
(programa
  (equipamento estufa
    (regra (simbolo solo_seco) (acao irrigar norte))))
```
 