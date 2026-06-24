# FASE 02 - ANÁLISE SINTÁTICA

# Enunciado
O objectivo do trabalho é construir um mini analisador sintático, ou seja, a segunda fase de um compilador para validar a sintaxe de um subjconjunto de código - fonte da sua linguagem de estudo.

Um analisador sintático (análise sintática ou Parser) faz a verificação da sintaxe do código-fonte (programa-fonte) com auxílio do analisador léxico (primeira fase de um compilador), toda vez que o analisador sintático quisser efectuar a verificação do código-fonte solicita os tokens e lexemas no analisador léxico armazenados na tabela de símbolo, assim que o token é analisado pelo Parser é gravado outras informais deste token na tabela de símbolo suponhamos que o token é um identificador então o analisador sintático deve gravar as seguintes informações (**endereço de memória, tipo de dados, tipo de variável, valor atribuído, escopo da variável, tamanho em byte**, etc). As informações gravadas na fase de análise sintática (analisador sintático ou parser) são usados na terceira fase (análise semântica) para se efectuar a verificação dos possíveis erros semânticos no seu código-fonte como por exemplo **incompatibilidade de tipos, declaração de uma variável mais de uma vezes no mesmo escopo, uso de uma variável não declarada**, etc.

**Primeiro cada estudante deve pesquisar a gramática correspondente a sua linguagem de estudo**

Como o método para a construção do analisador sintático é top-down (recursivo descendente sem retrocesso (backtracking)) a gramática precisa sofrer algumas transformações como:

- a) Eliminar a ambiguidade nas regras de produções:
- b) Eliminar recursividade à esquerda;
- c) Eliminar recursividade imediata (indirecta ou segunda ordem);
- d) Efectuar a análise do primeiro símbolo para cada regra de produção que tiver dois ou mais não-terminais para definir qual caminho seguir quando tivemos dois ou mais não-terminais;
- e) Todos não-terminais podem ser implementados como função (método) do tipo void no caso para aqueles que vão construir o analisador sintático em C, método ou classe para quem estiver a programar em uma linguagem orientada ao objecto;

## O que deverá ser entregue:
- Um **manual do usuário** (4 a 8 páginas) em um arquivo chamado `mu.doc`, e impresso, contendo uma explicação de como se utiliza o seu analisador sintático (explicar o formato de entrada e saída do programa).
- Um **manual do programador** (8 a 16 páginas) em um arquivo chamado `mp.doc`, e impresso, contendo a **factorização das regras gramatical, eliminação de recursividade à esquerda**.

## Observações:
- Trabalho individual
- Cópias serão anuladas
- As linguagens aceitas para a implementação são: `C`, `C++`, `Java`, `C#` entre outras linguagens
- A entrega consiste de defesa (apresentação) na data previamente marcada em horário de aula e entregue os manuais (usuário e programador)

OBS: O analisador sintático para funcionar depende do analisador léxico, então **bumba.**

Enviar o trabalho no dia **20-05-2026** até as **23:55** (segunda-feira) **Defesa no dia 21-05-2026**

**Email: compiladores2023@gmail.com**

## O que o Analisador Sintático (Parser) Linguagem de estudo C
- Função principal (main), declaração (variável, função), tipos de dados, estrutura de controlo (selecção, repetição), operadores (aritméticos, relacionais, lógicos, unário), comandos (entrada e saída), bloco, array, (uma dimensão e mais de uma dimensão), struct, union, expressões (aritméticas, relacionais, lógicas, unário), etc.

## O que o Analisador Sintático (Parser) Linguagem de estudo Java (linguagem orientado a objecto)
- package, import, declaração de class, declaração de variável, declaração de método, estrutura de controlo (selecção, repetição), array (uma dimensão e mais de uma dimensão), Operadores (aritméticos, relacionais, lógicos, unário), bloco, union, expressão (aritméticas, relacionais, lógicas, unária), modificadores, etc.