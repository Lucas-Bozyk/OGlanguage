# Testes da interface real: arquivos, stdin, recusas e erros de uso.
execute_process(COMMAND "${OGL_EXECUTAVEL}"
    RESULT_VARIABLE codigo OUTPUT_VARIABLE saida ERROR_VARIABLE erro)
file(READ "${OGL_EXEMPLOS}/saida_esperada.txt" esperado)
string(REPLACE "\r\n" "\n" saida "${saida}")
string(REPLACE "\r\n" "\n" esperado "${esperado}")
if(NOT codigo EQUAL 0 OR NOT saida STREQUAL esperado)
    message(FATAL_ERROR "Demonstracao difere da saida esperada: ${erro}\n${saida}")
endif()

foreach(modo IN ITEMS arquivo stdin)
    if(modo STREQUAL "arquivo")
        execute_process(COMMAND "${OGL_EXECUTAVEL}" thompson "${OGL_EXEMPLOS}/eventos_aceitos.txt"
            RESULT_VARIABLE codigo OUTPUT_VARIABLE saida)
    else()
        execute_process(COMMAND "${OGL_EXECUTAVEL}" thompson -
            INPUT_FILE "${OGL_EXEMPLOS}/eventos_aceitos.txt"
            RESULT_VARIABLE codigo OUTPUT_VARIABLE saida)
    endif()
    string(REPLACE "\r\n" "\n" saida "${saida}")
    if(NOT codigo EQUAL 0 OR NOT saida STREQUAL esperado)
        message(FATAL_ERROR "Leitura por ${modo} falhou: ${saida}")
    endif()
endforeach()

execute_process(COMMAND "${OGL_EXECUTAVEL}" thompson "${OGL_EXEMPLOS}/eventos_recusados.txt"
    RESULT_VARIABLE codigo OUTPUT_VARIABLE saida)
if(NOT codigo EQUAL 0 OR NOT saida MATCHES "REJEITADA")
    message(FATAL_ERROR "Cadeia com evento extra deveria ser recusada com codigo 0.")
endif()

execute_process(COMMAND "${OGL_EXECUTAVEL}" afd RESULT_VARIABLE codigo OUTPUT_VARIABLE saida)
if(NOT codigo EQUAL 0 OR NOT saida MATCHES "ACEITA")
    message(FATAL_ERROR "Exemplo do AFD manual falhou.")
endif()

file(WRITE "${OGL_TEMPORARIOS}/teste_vazio.txt" "")
file(WRITE "${OGL_TEMPORARIOS}/teste_desconhecido.txt" "geada solo_seco reservatorio_baixo\n")
foreach(nome IN ITEMS teste_vazio.txt teste_desconhecido.txt)
    execute_process(COMMAND "${OGL_EXECUTAVEL}" thompson "${OGL_TEMPORARIOS}/${nome}"
        RESULT_VARIABLE codigo OUTPUT_VARIABLE saida)
    if(NOT codigo EQUAL 0 OR NOT saida MATCHES "REJEITADA")
        message(FATAL_ERROR "Entrada ${nome} deveria ser recusada.")
    endif()
endforeach()

execute_process(COMMAND "${OGL_EXECUTAVEL}" thompson "${OGL_TEMPORARIOS}/arquivo_inexistente.txt"
    RESULT_VARIABLE codigo OUTPUT_VARIABLE saida ERROR_VARIABLE erro)
if(NOT codigo EQUAL 1 OR NOT erro MATCHES "Nao foi possivel abrir")
    message(FATAL_ERROR "Arquivo ausente deveria causar erro de leitura.")
endif()
execute_process(COMMAND "${OGL_EXECUTAVEL}" modo_inexistente
    RESULT_VARIABLE codigo OUTPUT_VARIABLE saida ERROR_VARIABLE erro)
if(NOT codigo EQUAL 1 OR NOT erro MATCHES "Uso:")
    message(FATAL_ERROR "Modo desconhecido deveria mostrar uso com codigo 1.")
endif()
execute_process(COMMAND "${OGL_EXECUTAVEL}" thompson - extra
    RESULT_VARIABLE codigo OUTPUT_VARIABLE saida ERROR_VARIABLE erro)
if(NOT codigo EQUAL 1 OR NOT erro MATCHES "Uso:")
    message(FATAL_ERROR "Argumentos em excesso deveriam causar erro de uso.")
endif()
execute_process(COMMAND "${OGL_EXECUTAVEL}" --help
    RESULT_VARIABLE codigo OUTPUT_VARIABLE saida)
if(NOT codigo EQUAL 0 OR NOT saida MATCHES "Uso:")
    message(FATAL_ERROR "Ajuda deveria mostrar uso com codigo 0.")
endif()
execute_process(COMMAND "${OGL_EXECUTAVEL}" estudo RESULT_VARIABLE codigo OUTPUT_VARIABLE saida)
if(NOT codigo EQUAL 0 OR NOT saida MATCHES "Comparacao: 121 cadeias, 15 aceitas, 0 divergencias" OR
   NOT saida MATCHES "Minimizacao: 5 estados antes, 3 depois" OR
   NOT saida MATCHES "6 \| 40 \| 65 \| 64")
    message(FATAL_ERROR "Relatorio dos modulos 4/5 ou crescimento de estados incorreto: ${saida}")
endif()
message(STATUS "Interface: 11 cenarios passaram.")
