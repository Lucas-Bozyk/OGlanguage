# Especificação do sistema em sete seções

## 1. Domínio e cena

Vários equipamentos operam na mesma unidade de cultivo: estufas, irrigadores e colheitadeiras recebem um fluxo global de eventos de sensores. Cada regra de cada equipamento é um reconhecedor. Ao aceitar um trecho não vazio, emite uma ação. Aceitações simultâneas são ordenadas deterministicamente por nome do equipamento e índice da regra, conforme [01_recorte.md](01_recorte.md).

## 2. O que se escreve

```ogl
equipamento colheitadeira { quando cultura_madura -> colher(); }
equipamento estufa { quando solo_seco, temperatura_alta -> irrigar(norte); }
equipamento pulverizador { quando chuva | reservatorio_baixo -> parar(); }
equipamento irrigador { quando solo_seco, (temperatura_alta | chuva)*, reservatorio_baixo -> parar(); }
```

## 3. O que aceita e recusa

Aceita `solo_seco`, `solo_seco,temperatura_alta`, `temperatura_alta|chuva`, `(temperatura_alta|chuva)*` e `reservatorio_baixo?`. Recusa `*chuva`, `chuva|`, `(chuva`, `chuva,,reservatorio_baixo`, grupo vazio e caractere desconhecido. Padrões que aceitam a cadeia vazia são válidos, mas essa aceitação não dispara ações.

## 4. Aninhamento

`solo_seco,((temperatura_alta|chuva)+,reservatorio_baixo)?` combina concatenação, alternância e pós-fixos com dois níveis de parênteses. A expressão denota linguagem regular, mas sua notação parentizada é lida por parser com pilha implícita.

## 5. Verificação antes de executar

O sistema deverá verificar equipamento duplicado, palavra esperada, bloco, regra vazia, padrão malformado, parêntese, operador sem operando, ação/setor desconhecido e evento fora do alfabeto. A primeira versão deverá devolver o primeiro erro. Identificadores diferenciam maiúsculas de minúsculas; palavras reservadas e eventos são escritos em minúsculas.

## 6. Produto e executor

```text
fonte .ogl → AST do programa → AST normalizada dos padrões → reconhecedores
→ objeto compilado → máquina virtual → linha do tempo de ações
```

O parser não deverá executar ações. A máquina virtual receberá o objeto validado e o fluxo de eventos simbólicos, emitindo comandos para os equipamentos. Nesta etapa, a saída esperada é textual; sensores e atuadores reais exigirão integração externa.

O exemplo completo está em [01_exemplo.ogl](../examples/01_exemplo.ogl), com um evento por linha em [01_eventos.txt](../examples/01_eventos.txt). Os índices de eventos começam em 1. A saída usa `evento N: equipamento.acao` para ações sem argumentos e `evento N: equipamento.acao(setor)` para ações com setor, conforme [01_saida_esperada.txt](../examples/01_saida_esperada.txt).

## 7. Pergunta experimental

**Pergunta:** o tempo por evento cresce linearmente com o número de reconhecedores ativos?

**Métrica:** tempo médio por evento. **Casos:** 1, 10, 100 e 1.000 regras, mantendo o mesmo tamanho dos padrões e o mesmo fluxo agrícola. **Hipótese:** dobrar reconhecedores aproximadamente dobra o custo. **Refutação:** crescimento quadrático persistente. A igualdade da linha do tempo entre execuções equivalentes será verificada separadamente. O experimento depende da implementação dos reconhecedores e da máquina virtual.
