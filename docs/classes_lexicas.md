# Classes léxicas da OGLanguage

Cada linha abaixo apresenta o nome da classe léxica e uma definição resumida.

```text
PALAVRA_RESERVADA — equipamento, quando, irrigar, pulverizar, ventilar, fechar, colher, recolher, parar, norte, sul, leste e oeste.
IDENTIFICADOR — nome de equipamento ou evento; começa com letra ASCII e continua com letras ASCII, dígitos ou `_`.
SETA — `->`, que associa um padrão a uma ação.
CONCATENACAO — `,`, que exige eventos ou subpadrões em sequência.
ALTERNANCIA — `|`, que oferece alternativas de padrões.
ESTRELA — `*`, zero ou mais repetições do subpadrão anterior.
MAIS — `+`, uma ou mais repetições do subpadrão anterior.
OPCIONAL — `?`, zero ou uma ocorrência do subpadrão anterior.
DELIMITADOR — `(`, `)`, `{` ou `}`; cada símbolo possui identidade própria para o parser.
TERMINADOR — `;`, que encerra uma regra.
COMENTARIO_LINHA — texto iniciado por `//` e encerrado na quebra de linha.
ESPACO_EM_BRANCO — espaço, tabulação, retorno de carro ou quebra de linha; separa tokens e é descartado.
FIM_DE_ARQUIVO — marca lógica que indica o término do código-fonte.
ERRO_LEXICO — caractere ou sequência que não pertence a nenhuma classe léxica válida.
```

## Padrões iniciais

```text
IDENTIFICADOR    = [A-Za-z][A-Za-z0-9_]*
COMENTARIO_LINHA = //[^\r\n]*
ESPACO_EM_BRANCO = [ \t\r\n]+
```

O lexer deverá consumir o identificador completo e consultar a lista de palavras reservadas. Assim, `equipamento2` é um identificador, mas `equipamento` é palavra reservada. Maiúsculas e minúsculas são distintas.

Os eventos, como `solo_seco`, são tokens `IDENTIFICADOR`; `EVENTO` na gramática indica seu uso dentro de um padrão. A análise semântica deverá verificar se pertencem ao alfabeto agrícola. Nomes de equipamento não podem ser palavras reservadas.

Comentários também podem terminar no fim do arquivo. Comentários e espaços são descartados, preservando as posições dos demais tokens. Não há comentários de bloco, literais de texto ou número, nem operadores aritméticos. `_estufa`, `/` isolado e `=` são inválidos.
