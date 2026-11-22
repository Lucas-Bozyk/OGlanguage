# OGLanguage

Linguagem formal de estudo para equipamentos agrícolas que reagem a eventos de sensores. A especificação da linguagem está em `docs/`; a etapa atual implementa os autômatos usados para reconhecer padrões.

## Módulos de autômatos

A implementação C++20/CMake em [Projeto/](Projeto/README.md) cobre Thompson a partir de uma AST, simulação AFN, determinização e minimização por partições. O mapa [progress.md](progress.md) descreve a arquitetura, as decisões e o escopo concluído.

Para executar a compilação e toda a bateria de verificações:

```powershell
powershell -File Projeto/verificar.ps1
```

O relatório do caso canônico, da comparação entre as três máquinas e do experimento de crescimento é:

```powershell
.\Projeto\build\ogl.exe estudo
```


## Linguagem do grupo

Cada equipamento tem regras `quando padrão -> ação`. Por exemplo:

```ogl
equipamento estufa { quando solo_seco, temperatura_alta, temperatura_alta -> irrigar(norte); }
equipamento colheitadeira { quando cultura_madura -> colher(); }
```

Esta etapa não implementa ainda o parser de `.ogl`, a compilação das declarações nem a execução de ações por máquina virtual. Os exemplos e decisões do domínio estão em `docs/`.
