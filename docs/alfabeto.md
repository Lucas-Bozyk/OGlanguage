# Alfabetos da OGLanguage

O alfabeto de caracteres do fonte e o alfabeto de eventos dos reconhecedores são distintos.

## Caracteres do fonte

Fontes usam UTF-8. A sintaxe usa letras ASCII, dígitos, `_`, espaços, tabulações, quebras de linha e `{ } ( ) ; , - > | * + ? /`. Identificadores começam com letra. Acentos são permitidos apenas no conteúdo dos comentários `//`, que se estendem até a quebra de linha ou o fim do arquivo.

`->` é um token composto; `-` e `>` isolados são inválidos. `/` só inicia comentário quando seguido de outro `/`. Textos, literais numéricos, atribuição e aritmética não pertencem ao recorte. Os dígitos podem compor identificadores, como `estufa2`.

## Eventos agrícolas

```text
Σ_eventos = { solo_seco, temperatura_alta, chuva,
              reservatorio_baixo, obstaculo_detectado, cultura_madura }
```

Cada nome representa um único símbolo do fluxo, independentemente de seu número de caracteres. Os padrões combinam esses símbolos para reconhecer sequências de acontecimentos. Leituras numéricas, limiares e origem física dos sensores são tratados fora da linguagem.
