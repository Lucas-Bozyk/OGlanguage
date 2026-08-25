# Tarefa 3 — Recusa de código malformado com posição

O analisador não encerra o processo nem lança uma exceção para erros esperados do usuário. Ele devolve um resultado com uma árvore válida ou com o primeiro erro encontrado.

## Estrutura do erro

Cada erro contém:

- Mensagem objetiva;
- Deslocamento contado desde o início do arquivo;
- Linha, iniciando em 1;
- Coluna, iniciando em 1.

A mensagem formatada reproduz a linha original e posiciona um cursor sob o local do problema.

## Exemplos recusados

### Ponto e vírgula ausente

```oglanguage
player heroi {
    pular()
}
```

O cursor aparece no `}` porque é nesse ponto que o analisador constata que o `;` esperado não foi encontrado.

### Direção inválida

```oglanguage
player heroi {
    andar(cima);
}
```

O cursor aponta para `cima`, e a mensagem informa as quatro direções válidas.

### Bloco não fechado

```oglanguage
player heroi {
    atacar();
```

O cursor aparece no fim do arquivo, local em que a chave ausente deve ser inserida.

### Comando desconhecido

```oglanguage
player heroi {
    voar();
}
```

O cursor aponta para `voar`, e a mensagem informa que o comando não pertence ao recorte.

## Primeiro erro

Somente o primeiro erro é devolvido. Continuar sem uma estratégia explícita de recuperação produziria mensagens em cascata derivadas do mesmo defeito. A análise semântica é posterior e tratará separadamente repetição igual a zero e declarações duplicadas de `player`.

## Como verificar

O programa de demonstração analisa várias entradas inválidas em chamadas independentes. Assim, um erro não impede que os demais casos sejam testados, e cada diagnóstico pode ser lido como orientação de correção.

