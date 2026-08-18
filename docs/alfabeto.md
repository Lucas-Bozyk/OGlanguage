# Alfabeto da OGLanguage

O alfabeto da OGLanguage é o conjunto finito `Σ` de símbolos permitidos na escrita dos programas.

## Definição formal

```text
Σ = L ∪ D ∪ S ∪ E
```

Onde:

- `L = { A, B, ..., Z, a, b, ..., z, _, á, à, â, ã, é, ê, í, ó, ô, õ, ú, ç, Á, À, Â, Ã, É, Ê, Í, Ó, Ô, Õ, Ú, Ç }`
- `D = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }`
- `S = { +, -, *, /, %, =, !, <, >, &, |, \\, (, ), {, }, [, ], ,, ;, :, ., \" , ' }`
- `E = { espaço, tabulação, quebra de linha, retorno de carro }`

## Observações

- Letras sem acento e o caractere `_` podem formar identificadores.
- Letras acentuadas são permitidas em textos, comentários e palavras reservadas em português.
- Espaços, tabulações e quebras de linha separam elementos léxicos, salvo dentro de textos.
- Símbolos compostos, como `==`, `!=`, `<=`, `>=`, `&&` e `||`, são sequências formadas por elementos de `S`.
- A codificação de referência dos arquivos-fonte é UTF-8.
