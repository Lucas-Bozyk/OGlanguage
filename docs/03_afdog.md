# Módulo 3 — AFD da OGLanguage

Este primeiro AFD é construído manualmente em [03_afdog.cpp](03_afdog.cpp), com base nos conceitos do [material do módulo 3](https://bcc-clfa-s.material.moacyr.pro.br/modulos/03/03_material.html) e nas duas tarefas do [projeto de referência](https://bcc-clfa-s.material.moacyr.pro.br/modulos/03/03_projeto_prof.html): escolher a representação das transições e executar uma máquina sobre uma entrada. O exemplo local `Data/afd.cpp` serviu como referência de leitura e de aceitação ao final.

## Recorte mínimo

Foi escolhida apenas a regra já presente no exemplo agrícola:

```ogl
equipamento estufa {
    quando solo_seco, temperatura_alta, temperatura_alta -> irrigar(norte);
}
```

O programa recebe uma sequência de nomes de eventos e responde se ela corresponde **exatamente** ao padrão dessa regra. A ação associada seria `estufa.irrigar(norte)`, mas este módulo apenas reconhece a sequência. Não lê a declaração `.ogl`, não constrói a máquina a partir da árvore e não executa ações.

## Definição formal

```text
M = (Q, Σ, δ, q0, F)
Q = {q0, q1, q2, q3, erro}
Σ = {solo_seco, temperatura_alta, chuva,
     reservatorio_baixo, obstaculo_detectado, cultura_madura}
Estado inicial = q0
F = {q3}
L(M) = { [solo_seco, temperatura_alta, temperatura_alta] }
```

Cada nome inteiro representa um símbolo de `Σ`. A vírgula acima separa símbolos na explicação; os arquivos de entrada usam espaços ou quebras de linha. O alfabeto é o mesmo definido em [alfabeto.md](alfabeto.md).

| Estado | Memória do que foi lido |
| --- | --- |
| `q0` | nenhum evento |
| `q1` | `solo_seco` |
| `q2` | `solo_seco, temperatura_alta` |
| `q3` | sequência completa; estado de aceitação |
| `erro` | sequência inválida; não há recuperação nesta entrada |

```text
q0 --solo_seco--> q1 --temperatura_alta--> q2 --temperatura_alta--> q3
```

Todas as demais transições levam a `erro`, inclusive qualquer evento após `q3`. De `erro`, todos os eventos levam ao próprio `erro`. A tabela completa é:

| Estado | solo_seco | temperatura_alta | chuva | reservatorio_baixo | obstaculo_detectado | cultura_madura |
| --- | --- | --- | --- | --- | --- | --- |
| q0 | q1 | erro | erro | erro | erro | erro |
| q1 | erro | q2 | erro | erro | erro | erro |
| q2 | erro | q3 | erro | erro | erro | erro |
| q3 | erro | erro | erro | erro | erro | erro |
| erro | erro | erro | erro | erro | erro | erro |

Há exatamente um destino por par de estado e evento: a função é determinística e total. Um nome fora de `Σ`, como `geada`, é recusado explicitamente pelo executor, sem acessar uma coluna inválida.

Os cinco estados são necessários para esta linguagem exata: de `q0`, `q1`, `q2` e `q3` faltam, respectivamente, três, dois, um e zero eventos específicos para aceitar. De `erro`, nenhuma continuação aceita. Portanto, nenhum par representa a mesma situação. Isso não é uma implementação do algoritmo de minimização.

## Representação e custo

A transição usa uma matriz fixa `transicoes[5][6]`. Estados são valores de um `enum`; a posição do evento no vetor `alfabeto` determina a coluna. A correspondência com a quíntupla fica explícita sem uma classe genérica ou métodos de configuração.

O custo da matriz é `|Q| × |Σ| × sizeof(Estado)`: são 30 entradas, das quais 27 levam a `erro`. Se `sizeof(Estado)` for 4, serão 120 bytes. O programa imprime `sizeof(transicoes)` para informar o valor real da plataforma. Essa conta inclui apenas a matriz, não os nomes dos eventos, os buffers de leitura ou a saída.

O mapa de mapas de `Data/afd.cpp` foi descartado neste recorte porque cinco estados e seis eventos cabem numa tabela pequena, visível por inteiro. A matriz reserva também as transições para erro, mas evita mapas, conjuntos e configuração dinâmica. A busca do evento faz no máximo seis comparações; a consulta da transição é direta. Para eventos do alfabeto fixo, o reconhecimento é linear na quantidade de eventos. Incluindo nomes arbitrários, a leitura também depende da quantidade de caracteres da entrada.

## Compilar e executar

Com um compilador GCC instalado, a partir da raiz do projeto:

```powershell
g++ -std=c++11 -Wall -Wextra -Werror -pedantic docs/03_afdog.cpp -o 03_afdog.exe
.\03_afdog.exe
```

Sem argumentos, lê [03_eventos.txt](../examples/03_eventos.txt), que contém um exemplo de recusa: `solo_seco temperatura_alta temperatura_alta chuva`. O evento extra `chuva` leva de `q3` ao estado `erro`. Também aceita um caminho ou `-` para a entrada padrão:

```powershell
.\03_afdog.exe examples/03_eventos.txt
"solo_seco temperatura_alta temperatura_alta" | .\03_afdog.exe -
```

A execução com `03_eventos.txt` deverá mostrar, depois do tamanho da tabela:

```text
Inicio: q0
q0 -- solo_seco --> q1
q1 -- temperatura_alta --> q2
q2 -- temperatura_alta --> q3
q3 -- chuva --> erro (evento fora da sequencia esperada)
REJEITADA
```

O comando com entrada padrão acima fornece uma cadeia aceita e deverá mostrar, depois do tamanho da tabela:

```text
Inicio: q0
q0 -- solo_seco --> q1
q1 -- temperatura_alta --> q2
q2 -- temperatura_alta --> q3
ACEITA
```

Aceitação e recusa normais retornam código de saída 0; problemas de abertura, leitura ou uso retornam 1. Eventos são sensíveis a maiúsculas; espaços, tabulações e quebras de linha são separadores. Comentários e vírgulas do fonte `.ogl` não são aceitos neste arquivo de eventos.

## Casos de conferência

| Entrada | Resultado e motivo |
| --- | --- |
| `solo_seco temperatura_alta temperatura_alta` | aceita, termina em q3 |
| vazia ou apenas espaços | recusa, permanece em q0 |
| `solo_seco temperatura_alta` | recusa, termina em q2 |
| `temperatura_alta solo_seco temperatura_alta temperatura_alta` | recusa, não sai do erro após o primeiro evento |
| `solo_seco chuva temperatura_alta temperatura_alta` | recusa, evento conhecido fora da sequência |
| `solo_seco temperatura_alta temperatura_alta chuva` | recusa, passar por q3 antes do fim não basta |
| `geada solo_seco temperatura_alta temperatura_alta` | recusa, evento fora do alfabeto |

O traço continua depois da queda em erro, mostrando que ele é absorvente. Se o último estado for q0, q1 ou q2, a sequência está incompleta; o próprio traço identifica essa situação.

## Relação com o executor futuro

O fluxo completo de [01_eventos.txt](../examples/01_eventos.txt) será recusado por este AFD isolado: ele contém eventos de outras regras e não é a sequência exata acima. Isso não altera a linha do tempo esperada do módulo 1.

O executor futuro deverá procurar padrões em trechos do fluxo, emitir ações durante a leitura e reiniciar as regras após os disparos, conforme [01_recorte.md](01_recorte.md). Nesta etapa, o AFD testa uma sequência candidata completa, com aceitação verificada somente ao final. A geração automática a partir da AST e o gerenciamento de várias regras ficam para etapas posteriores.
