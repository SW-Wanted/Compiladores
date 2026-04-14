# Análise Sintática

# Enunciado
O objectivo do trabalho é construir um mini analisador sintático, ou seja, a segunda fase de um compilador para validar a sintaxe de um subjconjunto de código - fonte da sua linguagem de estudo.

Um analisador sintático (análise sintática ou Parser) faz a verificação da sintaxe do código-fonte (programa-fonte) com auxílio do analisador léxico (primeira fase de um compilador), toda vez que o analisador sintático quisser efectuar a verificação do código-fonte solicita os tokens e lexemas no analisador léxico armazenados na tabela de símbolo, assim que o token é analisado pelo Parser é gravado outras informais deste token na tabela de símbolo suponhamos que o token é um identificador então o analisador sintático deve gravar as seguintes informações (**endereço de memória, tipo de dados, tipo de variável, valor atribuído, escopo da variável, tamanho em byte**, etc). As informações gravadas na fase de análise sintática (analisador sintático ou parser) são usados na terceira fase (análise semântica) para se efectuar a verificação dos possíveis erros semânticos no seu código-fonte como por exemplo **incompatibilidade de tipos, declaração de uma variável mais de uma vezes no mesmo escopo, uso de uma variável não declarada**, etc.

**Primeiro cada estudante deve pesquisar a gramática correspondente a sua linguagem de estudo**

Como o método para a construção do analisador sintático é top-down (recursivo descendente sem retrocesso (backtracking) a gramática precisa sofrer algumas transformações como:

- a) Eliminar a ambiguidade nas regras de produções:
- b) Eliminar recursividade à esquerda;
- c) Eliminar recursividade imediata (indirecta ou segunda ordem);
- d) Efectuar a análise do primeiro símbolo para cada regra de produção que tiver dois ou mais não-terminais para definir qual caminho seguir quando tivemos dois ou mais não-terminais;
- e) Todos não-terminais podem ser implementados como função (método) do tipo void no caso para aqueles que vão construir o analisador sintático em C, método ou classe para quem estiver a programar em uma linguagem orientada ao objecto;

## O que deverá ser entregue:
- Um **manual do usuário** (4 a 8 páginas) em um arquivo chamado `um.doc`, e impresso, contendo uma explicação de como se utiliza o seu analisador sintático (explicar o formato de entrada e saída do programa).
- Um **manual do programador** (8 a 16 páginas) em um arquivo chamado `mp.doc`, e impresso, contendo a **factorização das regras gramatical, eliminação de recursividade à esquerda**.

## Observações:
- Trabalho individual
- Cópias serão anuladas
- As linguagens aceitas para a implementação são: C, C++, JAVA, C# entre outras linguagens
- A entrega consiste de defesa (apresentação) na data previamente marcada em horário de aula e entregue os manuais (usuário e programador)

OBS: O analisador sintático para funcionar depende do analisador léxico, então **bumba.**

Enviar o trabalho no dia **20-05-2026** até as **23:55** (segunda-feira) **Defesa no dia 21-05-2026**

**Email: compilador2023@gmail.com**

## O que o Analisador Sintático (Parser) Linguagem de estudo C
- Função principal (main), declaração (variável, função), tipos de dados, estrutura de controlo (selecção, repetição), operadores (aritméticos, relacionais, lógicos, unário), comandos (entrada e saída), bloco, array, (uma dimensão e mais de uma dimensão), struct, union, expressões (aritméticas, relacionais, lógicas, unário), etc.

## O que o Analisador Sintático (Parser) Linguagem de estudo JAVA (linguagem orientado a objecto)
- package, import, declaração de class, declaração de variável, declaração de método, estrutura de controlo (selecção, repetição), array (uma dimensão e mais de uma dimensão), Operadores (aritméticos, relacionais, lógicos, unário), bloco, union, expressão (aritméticas, relacionais, lógicas, unária), modificadores, etc.

# Parser
A **Análise Sintática** é a fase do compilador que agrupa os tokens gerados pelo Lexer em estruturas hierárquicas, verificando se a sequência obedece às regras gramaticais da linguagem. Se o Lexer verifica a "ortografia", o Parser verifica a "gramática".

Para que isso funcione, a linguagem precisa ser definida formalmente, geralmente através de uma **Gramática Livre de Contexto (CFG)**.

---

### 1. Representação da Gramática: BNF e EBNF
A forma mais comum de representar as regras de uma linguagem é através da **BNF (Backus-Naur Form)**. Ela define como símbolos não-terminais (regras) podem ser expandidos em símbolos terminais (tokens).

Uma gramática formal é geralmente definida como uma 4-tupla:
$$G = (V, \Sigma, R, S)$$
Onde:
* $V$: Conjunto de símbolos não-terminais.
* $\Sigma$: Conjunto de símbolos terminais (tokens).
* $R$: Conjunto de regras de produção.
* $S$: O símbolo inicial.

**Exemplo de regra para uma atribuição simples:**
```ebnf
<atribuicao> ::= <identificador> "=" <expressao> ";"
<expressao>  ::= <termo> (("+" | "-") <termo>)*
```

---

### 2. Possibilidades de Representação (Saída do Parser)
O Parser não apenas valida o código, ele o transforma em uma estrutura de dados que o restante do compilador consiga processar.

#### **CST (Concrete Syntax Tree)**
Também chamada de Árvore de Derivação. Ela contém **todos** os detalhes da gramática, incluindo símbolos auxiliares, parênteses e pontos-e-vírgulas. É útil para ferramentas de formatação de código (linters), mas é muito pesada para a geração de código.

#### **AST (Abstract Syntax Tree)**
Como vimos anteriormente, é uma versão simplificada da CST. Ela foca apenas na lógica (operadores e operandos), removendo ruídos sintáticos. É a representação padrão para a maioria dos compiladores modernos.



#### **DAG (Directed Acyclic Graph)**
Em fases de otimização agressiva, o compilador pode usar um Grafo Acíclico Dirigido. Diferente da árvore, o DAG permite que nós compartilhem filhos. Se você tem `x = (a+b) * (a+b)`, a árvore repetiria a subárvore `(a+b)`, enquanto o DAG apontaria ambos os lados da multiplicação para o mesmo nó `+`.

---

### 3. Estratégias de Implementação
Existem duas abordagens principais para construir essas árvores:

#### **Top-Down (Descendente)**
O parser começa no símbolo inicial da gramática e tenta "prever" qual regra aplicar com base no próximo token.
* **Recursive Descent Parser:** Onde cada regra da gramática vira uma função no código (comum em parsers escritos à mão).
* **LL(k):** Lê da esquerda para a direita (L), gera uma derivação à esquerda (L) e olha $k$ tokens à frente.

#### **Bottom-Up (Ascendente)**
O parser começa pelos tokens (folhas) e vai "reduzindo" sequências até chegar ao símbolo inicial (raiz). É mais potente que o Top-Down, mas quase impossível de escrever à mão (usa-se geradores de parser).
* **LR (Shift-Reduce):** Usa uma pilha para guardar tokens (Shift) e aplicá-los a regras (Reduce).
* **LALR:** Uma variante eficiente usada por ferramentas clássicas como o **Yacc** ou **Bison**.



---

### 4. Recuperação de Erros
Um bom analisador sintático não "morre" no primeiro erro. Ele deve ser capaz de:
1.  **Reportar o erro:** Indicar linha, coluna e o que era esperado (ex: "Esperado ';' após expressão").
2.  **Sincronizar:** Pular tokens até encontrar um "ponto de segurança" (como um `;` ou uma chave `}`) para continuar analisando o restante do arquivo e encontrar mais erros de uma vez.

### Ferramentas Modernas
Atualmente, muitos desenvolvedores utilizam geradores de parser para evitar o trabalho manual:
* **ANTLR:** Muito popular, gera código em Java, C#, Python, etc.
* **Lex/Yacc (ou Flex/Bison):** O padrão clássico para quem trabalha com C/C++.
* **Roslyn:** O compilador moderno do C#, que expõe toda a sua árvore sintática via API, permitindo criar ferramentas de análise estática.
