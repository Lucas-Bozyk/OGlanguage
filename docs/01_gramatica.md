# Gramática da OGLanguage

A gramática OGLanguage iniciada no primeiro módulo.

Esta é a forma em estado bruto, pode ter expressões que serão alteradas conforme estudo de fatoração e formas normais.

## Gramática hospedeira — a linguagem escrita pelo usuário

```text
programa       := declaracaoPlayer+ ;

declaracaoPlayer := "player" ID bloco ;

bloco          := "{" comando* "}" ;

comando        := acaoSimples
                | repeticao
                | condicional ;

acaoSimples    := movimento ";"
                | "pular" "(" ")" ";"
                | "atacar" "(" ")" ";"
                | "defender" "(" ")" ";"
                | "parar" "(" ")" ";" ;

movimento      := verboMovimento "(" direcao ")" ;

verboMovimento := "andar" | "correr" ;

direcao        := "direita"
                | "esquerda"
                | "frente"
                | "tras" ;

repeticao      := "repetir" "(" NUMERO ")" bloco ;

condicional    := "se" "(" condicao ")" bloco ;

condicao       := "inimigo_proximo"
                | "obstaculo_proximo"
                | "vida_baixa"
                | "no_chao" ;
```

## Regras léxicas

```text
ID             := LETRA ( LETRA | DIGITO | "_" )* ;
NUMERO         := DIGITO+ ;
LETRA          := "a" ... "z" | "A" ... "Z" ;
DIGITO         := "0" ... "9" ;
COMENTARIO     := "//" qualquer_caractere_exceto_quebra_de_linha* ;
ESPACO         := " " | "\t" | "\r" | "\n" ;
```

Os tokens `COMENTARIO` e `ESPACO` são reconhecidos pelo analisador léxico e descartados antes da análise sintática. O token `NUMERO`, quando usado em `repetir`, deve representar um inteiro maior que zero; essa restrição é semântica, pois a gramática reconhece também a forma textual de `0`.

## Exemplo válido

```oglanguage
player guerreiro {
    correr(frente);

    se(inimigo_proximo) {
        repetir(3) {
            atacar();
        }
    }

    defender();
    parar();
}
```

## Exemplos inválidos

```oglanguage
player jogador {
    voar();
}
```

O comando `voar` não pertence ao conjunto de ações aceitas.

```oglanguage
player jogador {
    andar(cima);
}
```

A direção `cima` não pertence ao conjunto inicial de direções.

```oglanguage
player jogador {
    repetir(0) {
        atacar();
    }
}
```

A estrutura é sintaticamente reconhecível, mas falha na análise semântica porque a quantidade de repetições deve ser positiva.

## Onde cada nível da hierarquia de Chomsky aparece

A gramática da OGLanguage é livre de contexto (tipo 2). Os blocos aninhados de `player`, `repetir` e `se` exigem que o reconhecedor controle a correspondência entre chaves de abertura e fechamento. Como o aninhamento pode crescer, um autômato finito não possui memória suficiente para reconhecer todos os programas válidos; é necessário um autômato com pilha ou um analisador sintático equivalente.

As regras léxicas de `ID`, `NUMERO`, comentários, espaços, palavras reservadas e símbolos são regulares (tipo 3) e podem ser reconhecidas por autômatos finitos. Por isso, o compilador separa o trabalho em duas etapas: o analisador léxico reconhece tokens regulares, e o analisador sintático verifica a estrutura livre de contexto formada por esses tokens.

As restrições como “o número de repetições deve ser maior que zero”, “o nome do `player` não pode ser declarado duas vezes” e “a ação deve existir” não são decididas apenas pela forma da gramática. Elas pertencem à análise semântica e são verificadas depois que a árvore sintática é construída.
