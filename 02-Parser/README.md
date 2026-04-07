# Análise Sintática

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
