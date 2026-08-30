# Especificação do sistema em sete seções

## 1. Domínio e cena

Vários personagens habitam o mesmo mundo. Cada regra de cada personagem é um reconhecedor que recebe o fluxo global de eventos. Ao aceitar, emite uma ação. Aceitações simultâneas são ordenadas deterministicamente por nome do personagem e índice da regra.

## 2. O que se escreve

```ogl
player guarda { quando vi_inimigo -> atacar(); }
player heroi { quando vi_inimigo, dano -> recuar(); }
player mago { quando dano | vida_baixa -> defender(); }
player sentinela { quando vi_inimigo, (dano | bloqueio)*, vida_baixa -> recuar(); }
```

## 3. O que aceita e recusa

Aceita `vi_inimigo`, `vi_inimigo,dano`, `dano|bloqueio`, `(dano|bloqueio)*` e `vida_baixa?`. Recusa `*dano`, `dano|`, `(dano`, `dano,,vida_baixa`, grupo vazio e caractere desconhecido.

## 4. Aninhamento

`vi_inimigo,((dano|bloqueio)+,vida_baixa)?` possui três níveis. A expressão denota linguagem regular, mas sua notação parentizada é lida por parser com pilha implícita.

## 5. Verificação antes de executar

O sistema verifica `player` duplicado, palavra esperada, bloco, regra vazia, padrão malformado, parêntese, operador sem operando, ação/direção desconhecida e evento fora do alfabeto. A primeira versão devolve o primeiro erro.

## 6. Produto e executor

```text
fonte .ogl → AST do programa → AST normalizada dos padrões → reconhecedores
→ objeto compilado → máquina virtual → linha do tempo de ações
```

O parser não executa ações. A máquina virtual recebe o objeto validado.

## 7. Pergunta experimental

**Pergunta:** o tempo por evento cresce linearmente com o número de reconhecedores ativos?

**Métrica:** tempo médio por evento. **Casos:** 1, 10, 100 e 1.000 regras. **Hipótese:** dobrar reconhecedores aproximadamente dobra o custo. **Refutação:** crescimento quadrático ou variação não determinística relevante entre execuções equivalentes.
