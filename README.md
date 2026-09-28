# OGLanguage

Linguagem formal para equipamentos agrícolas reagindo ao mesmo fluxo de eventos de sensores. Fontes usam `.ogl`.

Cada `equipamento` possui regras `quando padrão -> ação`. O compilador previsto deverá ler padrões regulares e produzir reconhecedores executados por máquina virtual, emitindo comandos de irrigação, ventilação e colheita.

## Determinismo

Todos consomem o mesmo fluxo. Disparos simultâneos são ordenados pelo nome do equipamento e pela posição local da regra. Reordenar declarações não altera a linha do tempo. As regras completas estão em [01_recorte.md](../docs/01_recorte.md).

## Exemplo

```ogl
equipamento estufa { quando solo_seco, temperatura_alta, temperatura_alta -> irrigar(norte); }
equipamento colheitadeira { quando cultura_madura -> colher(); }
```

## Estado atual e material de referência

Os arquivos atuais em `docs/` e `examples/` definem a linguagem agrícola. O primeiro AFD manual está em [03_afdog.cpp](../docs/03_afdog.cpp), com explicação e comandos de compilação em [03_afdog.md](../docs/03_afdog.md). Ainda não há compilador da linguagem ou máquina virtual integrados.

`Sources.md` indica o site da disciplina e `afd.cpp` contém o exemplo de referência. Esta pasta é ignorada pelo Git; a especificação vigente e a adaptação agrícola estão em `docs/`.
