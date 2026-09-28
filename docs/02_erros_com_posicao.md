# Recusa de expressão malformada com posição

Cada token e diagnóstico deverá registrar deslocamento iniciado em 0 e linha/coluna iniciadas em 1. O resultado deverá conter árvore ou primeiro erro, sem encerrar o processo.

Formato:

```text
erro: esperado padrão após '|'
linha 1, coluna 7

chuva|
      ^
```

| Entrada | Diagnóstico |
| --- | --- |
| `(chuva` | esperado `)` antes do fim |
| `*chuva` | operador `*` sem operando |
| `chuva|` | esperado padrão após `|` |
| `chuva,,reservatorio_baixo` | esperado símbolo após `,` |
| `()` | grupo vazio não permitido |
| `chuva**` | operador pós-fixo repetido |

O cursor deverá apontar onde o erro é detectado. Isso pode diferir do local conceitual da omissão. No exemplo, aponta para o fim da entrada, com deslocamento 6. Cada caso deverá ser executado de modo independente nos testes da implementação.

Na linguagem hospedeira, `irrigar(centro)` deverá ser recusado por setor inválido e `colher(norte)` por argumento indevido. Na análise semântica, nomes de equipamento duplicados e eventos como `geada`, ainda fora do alfabeto, deverão ser recusados com posição no fonte.

## Exemplo de código incorreto e saída esperada

O programa abaixo contém duas vírgulas consecutivas no padrão da regra, sem um símbolo entre elas:

```ogl
equipamento estufa {
    quando chuva,,reservatorio_baixo -> irrigar(norte);
}
```

Saída esperada do diagnóstico:

```text
erro: esperado símbolo após ','
linha 2, coluna 18

    quando chuva,,reservatorio_baixo -> irrigar(norte);
                 ^
```

O cursor aponta para a segunda vírgula, onde o erro é detectado, preservando a linha e a coluna do fonte completo. Para corrigir o código, basta remover uma das vírgulas: `quando chuva,reservatorio_baixo -> irrigar(norte);`.
