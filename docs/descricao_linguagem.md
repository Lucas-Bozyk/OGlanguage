# Descrição da OGLanguage

Linguagem formal de domínio específico para automação agrícola: descreve como equipamentos de estufas, irrigação e colheita reagem a padrões de eventos de sensores. O nome OGLanguage e a extensão `.ogl` são mantidos. Cada declaração `equipamento` associa um nome a regras `quando padrão -> ação`.

O compilador previsto tokeniza o fonte, reconhece declarações, normaliza padrões, valida símbolos e produz reconhecedores e ações para uma máquina virtual. O produto inicial é uma linha do tempo de comandos, como irrigar um setor, ventilar uma estufa ou iniciar a colheita. A leitura física dos sensores e o acionamento das máquinas cabem a uma integração externa.

O projeto possui gramática, núcleo de expressões regulares, diagnósticos e exemplos de entrada e saída especificados. O [módulo 3](03_afdog.md) acrescenta um primeiro AFD manual em C++ para reconhecer uma sequência agrícola. O compilador, a geração automática dos autômatos e a máquina virtual ainda precisam ser implementados. Variáveis gerais, matemática, textos, funções, APIs e bancos de dados ficam fora do recorte.
