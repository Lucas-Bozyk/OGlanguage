# Gramática da OGLanguage

```text
programa          := declaracaoEquipamento+ ;
declaracaoEquipamento := "equipamento" ID "{" regra+ "}" ;
regra             := "quando" padrao "->" acao ";" ;
acao              := aplicacao | "ventilar" "(" ")" | "fechar" "(" ")"
                   | "colher" "(" ")" | "recolher" "(" ")" | "parar" "(" ")" ;
aplicacao         := ("irrigar" | "pulverizar") "(" setor ")" ;
setor             := "norte" | "sul" | "leste" | "oeste" ;
padrao            := alternacao ;
alternacao        := concatenacao ( "|" concatenacao )* ;
concatenacao      := posfixa ( "," posfixa )* ;
posfixa           := primaria ( "*" | "+" | "?" )? ;
primaria          := EVENTO | "(" padrao ")" ;
ID                := LETRA (LETRA | DIGITO | "_")* ;
EVENTO            := ID ;
LETRA             := "a" ... "z" | "A" ... "Z" ;
DIGITO            := "0" ... "9" ;
```

Precedência: pós-fixos, concatenação e alternância. Binários associam à esquerda. Espaços e comentários `//` são descartados. Literais entre aspas são palavras ou símbolos da linguagem; `+`, `*` e `?` fora de aspas são metanotação da gramática.

`EVENTO` é um `ID` usado no padrão e validado semanticamente contra o alfabeto de eventos definido em [alfabeto.md](alfabeto.md). Palavras reservadas não podem ser identificadores. Não há literais numéricos; dígitos aparecem apenas depois da primeira letra de um identificador.

Os padrões denotam linguagens regulares. Sua notação permite parênteses com aninhamento arbitrário, lidos por gramática livre de contexto; os blocos de equipamentos não se aninham. Duplicidade de nomes e vocabulário de eventos são restrições semânticas. Ações e setores válidos estão enumerados na gramática.
