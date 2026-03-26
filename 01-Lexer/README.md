*Trabalho sobre Análise Léxica (Analisador Léxico): (primeira fase de um compilador)*

# Enunciado

O objectivo do trabalho é construir um analisador léxico para uma linguagem de programação de alto nível. (Subconjuunto da sua linguagem de estudo).

O programa deve ler um programa-fonte de um subconjunto da sua linguagem de estudo e gravar todos os toekn e lexema encontrados em uma **tabela de símbolos**.

O programa (**analisador Léxico**) deve conter uma **função** (**classe** ou **método**) **analex()** que fará análise de todos os símbos do programa-fonte lido pela função (**classe** ou **método**) **ler caractere()** depois de validar o lexema e token e gravar na **tabela de símbolo**.

O analisa léxico deve ler um arquivo de entrada com os seguintes símbolos válidos da linguagem escolhida pelo grupo:

- Letra: a, b...z, A, B..Z
- Digito: 0, 1..9
- Simbolos Especiais: (|)|{|}|[|]|etc
- Operador Relacionais: <|>|<=|>=|etc
- Palavras Reservadas: while | if | etc.
- Pontuações: , | : | ; | . | etc
- etc

## Algumas funções (classe ou método) que os senhores deverão criar:

- **Analex()**: função (**classe** ou **método**) que implementa os autómatos finitos determinísticos e faz a validação do lexema e tken.
- **Ler caractere()**: esta função (**classe** ou **método**) deve fazer a leitura caractere a caractere e entrega o caractere (símbolo) para a função (**classe** ou **método**) Analex() aonde o símbolo é analiado e validado.
- **Volta caractere()** esta função (**classe** ou **método**) será sempre usado quando for lido "outro" no diagrama de transição.
- **Gravar Token Lexema()**: esta função (**classe** ou **método**) terá o papel de gravar ou guardar o token e lexema na tabela de símbolos.

## Dica:
- Criar os tokens como constantes (`#define`) para os estudantes que forem implementar em C (no java podemos utilizar `final`)

## O que será entregue:
- Um **manual do usuário** (**duas** a **cinco páginas**) em um arquivo chamado `mu.doc`, contendo uma **explicação** de como se utiliza o seu **Analisador Léxico** (explicar o formato de **entrada** e **saída** do programa)
- Um **manual do programador** (**cinco** a **dez páginas**) em um arquivo chamado `mp.doc`, contendo a **lista completa dos tokens utilizados**, o **autómato finito determinístico**, **expressões regulares** e o **pseudo-código de cada função (classe ou método)**.
- É obrigatório o uso de conceito de máquina de estado na implementação do Léxico.

# Obsevações:
- As linguagens aceitas para a implementação são: C, C++, JAVA, C#, entre outras linguagens.
- É proibido o uso da linguagem Python tanto para linguagem de estudo como linguagem de implementação:
- Só é permitido trabalhar com linguagens compiladas;
- Entrega atrasada implica em 2% de desconto do valor total da nota por dia útil de atraso;
- A entrega consiste de defesa (apresentação) na data previamente marcada em horário de aula e entregue os manuais (usuário e programador)

## Datas
* Envio do autómato Finito Determinístico até dia 14/04/2026 testado na ferramenta JFLAP até as 23:50 (enviar também todas as expressões regulares):
* Enviar o Analisador Léxico até 14/04/2026 até as 23:50
* Defesa no dia 16/04/2023;

# Análise Léxica
## 1º Abrir o ficheiro (com o código fonte)
- C: Fopen
- Java: File reader
## 2º Ler caractere a caractere (no código-fonte)
C: Caractere a Caractere
Java: Ler toda linha

## 3º Validar o símbolo (caractere) com o auxílio do Autômato finito
- [i|n|t]

- 4º Dar o padrão (token) ou lexema montado
### int - Palavra reservada 
- int
- token_int
- Keywords_int


## 5º Guardar o token, lexema  e linha na tabela de símbolo

| Lexema | Token | Linha |
| :--- | :--- | :--- |
| int|  | Linha |
| x | Token | Linha |
| = | Token | Linha |
| 10 | Token | Linha |
| ; | Token | Linha |
