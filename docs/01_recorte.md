# Recorte da OGLanguage

## Domínio

A linguagem descreve como equipamentos agrícolas reagem a acontecimentos de uma unidade de cultivo. O usuário declara `equipamento`s e associa padrões regulares de eventos a ações. Estufas, sistemas de irrigação e colheitadeiras são representados por equipamentos nomeados, sem criar categorias sintáticas diferentes.

Eventos iniciais: `solo_seco`, `temperatura_alta`, `chuva`, `reservatorio_baixo`, `obstaculo_detectado`, `cultura_madura`. São sinais simbólicos; limiares e medidas são convertidos em eventos fora da linguagem.

| Ação | Comando emitido para o equipamento |
| --- | --- |
| `irrigar(setor)` | iniciar irrigação no setor indicado |
| `pulverizar(setor)` | iniciar pulverização no setor indicado |
| `ventilar()` | acionar ventilação |
| `fechar()` | fechar aberturas da estufa |
| `colher()` | iniciar colheita |
| `recolher()` | recolher o implemento agrícola |
| `parar()` | interromper a operação do equipamento |

Setores válidos: `norte`, `sul`, `leste`, `oeste`. Cada equipamento é um destino lógico de comandos. A associação a dispositivos e a compatibilidade física das ações cabem à integração externa; não há tipos de equipamento nesta versão.

Leitura de sensores, protocolos de comunicação, controle de motores, navegação, dosagem, duração das operações e intertravamentos pertencem ao controlador externo. O recorte inicial simula a emissão dos comandos.

## Forma

```ogl
equipamento estufa {
    quando solo_seco, temperatura_alta, temperatura_alta -> irrigar(norte);
    quando reservatorio_baixo -> parar();
}
```

Padrões aceitam concatenação `,`, alternância `|`, `*`, `+`, `?` e agrupamento.

## Fluxo e determinismo

Cada regra gera um reconhecedor; todos recebem cada evento do mesmo fluxo da unidade de cultivo. Os eventos não possuem destino individual nesta versão. Após disparar, a regra reinicia. Disparos simultâneos são ordenados por `(indice_do_evento, nome_do_equipamento, indice_da_regra)`, com nomes em ordem lexicográfica ASCII e índices iniciados em 1. A ordem de declaração dos equipamentos não participa do desempate.

Os padrões reconhecem trechos contíguos do fluxo, podendo iniciar em qualquer evento. A regra dispara na primeira aceitação não vazia, no máximo uma vez por evento, e descarta as correspondências em andamento ao reiniciar para o próximo evento. Aceitar a cadeia vazia por `*` ou `?` não emite comandos. A ordenação registra todos os disparos; não resolve conflitos físicos entre comandos.

## Produto

Após análises léxica, sintática e semântica, o compilador deverá produzir equipamento, reconhecedor, ação com eventual setor e índice da regra. Uma máquina virtual separada deverá emitir a linha do tempo.

## Alternativas descartadas

- controle imperativo: não exercita padrões;
- ações sem equipamento: não identificam o destino dos comandos;
- interpretar o fonte: esconderia as fases;
- ordem de declaração ou threads como desempate: tornam o resultado dependente da organização do fonte ou da execução;
- fluxos separados: ampliam o modelo de eventos compartilhado;
- controle físico e cálculos agronômicos: ampliam o domínio além dos padrões regulares.
