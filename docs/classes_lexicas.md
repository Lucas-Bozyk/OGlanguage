# Classes léxicas da OGLanguage

Cada linha abaixo apresenta o nome da classe léxica e uma definição resumida.

```text
PALAVRA_RESERVADA — termo com significado fixo na linguagem: programa, var, inteiro, real, texto, logico, se, senao, enquanto, para, funcao, retorne, leia, escreva, verdadeiro e falso.
IDENTIFICADOR — nome de variável, função ou programa; começa com letra ou `_` e continua com letras, dígitos ou `_`.
NUMERO_INTEIRO — sequência de um ou mais dígitos decimais, sem parte fracionária.
NUMERO_REAL — número decimal com parte inteira e fracionária separadas por ponto.
CADEIA_TEXTO — sequência de caracteres delimitada por aspas duplas.
CARACTERE — um caractere delimitado por aspas simples.
OPERADOR_ARITMETICO — símbolo de operação matemática: `+`, `-`, `*`, `/` ou `%`.
OPERADOR_RELACIONAL — símbolo de comparação: `==`, `!=`, `<`, `>`, `<=` ou `>=`.
OPERADOR_LOGICO — símbolo de operação booleana: `&&`, `||` ou `!`.
OPERADOR_ATRIBUICAO — símbolo `=` usado para atribuir um valor.
DELIMITADOR — símbolo que organiza blocos ou expressões: `(`, `)`, `{`, `}`, `[`, `]`.
SEPARADOR — símbolo que separa ou encerra elementos: `,`, `;`, `:` ou `.`.
COMENTARIO_LINHA — texto iniciado por `//` e encerrado na quebra de linha.
COMENTARIO_BLOCO — texto iniciado por `/*` e encerrado por `*/`.
ESPACO_EM_BRANCO — espaço, tabulação ou quebra de linha; ignorado fora de textos, exceto por separar tokens.
FIM_DE_ARQUIVO — marca lógica que indica o término do código-fonte.
ERRO_LEXICO — caractere ou sequência que não pertence a nenhuma classe léxica válida.
```

## Padrões iniciais

```text
IDENTIFICADOR  = [A-Za-z_][A-Za-z0-9_]*
NUMERO_INTEIRO = [0-9]+
NUMERO_REAL    = [0-9]+\.[0-9]+
CADEIA_TEXTO   = \"([^\"\\]|\\.)*\"
CARACTERE      = '([^'\\]|\\.)'
```


