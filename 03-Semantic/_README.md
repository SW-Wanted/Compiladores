# Implementação do Analisador Semântico

O Analisador Semântico é a **terceira e última fase do front-end** do mini-compilador.
Recebe a **árvore sintática (AST)** produzida pelo [Parser](../02-Parser) (que por sua vez
usa o [Lexer](../01-Lexer)), percorre-a com o auxílio de uma **tabela de símbolos com
escopos** e valida as regras semânticas da linguagem, produzindo uma tabela de
**diagnósticos** (erros e avisos) com a respectiva linha e coluna.

```
código-fonte ──▶ Lexer ──▶ Parser ──▶ AST ──▶ Analisador Semântico ──▶ diagnósticos
```

## Compilação

O `Makefile` reutiliza automaticamente as bibliotecas estáticas das fases anteriores
(`liblexer.a` e `libparser.a`), compilando-as se necessário:

```bash
make          # compila o executável ./semantic
make test     # executa o analisador sobre todos os testes em tests/
make re       # recompila tudo do zero
make clean    # remove os objectos
```

## Utilização

```bash
./semantic [opções] <arquivo.c>

Opções:
  -a, --ast          mostrar a árvore sintática (AST)
  -s, --symbols      mostrar a tabela de símbolos semântica
  -d, --diagnostics  mostrar os diagnósticos semânticos (predefinido)
  -A, --all          mostrar tudo
```

O programa termina com código de saída `0` quando não há erros e `1` caso contrário
(ou quando o Parser detecta erros de sintaxe, situação em que a análise semântica é
abortada, como num compilador real).

Exemplos:

```bash
./semantic tests/valido.c        # 0 erros
./semantic tests/erros.c         # lista todos os erros semânticos
./semantic -A tests/valido.c     # AST + símbolos + diagnósticos
```

## Verificações semânticas efectuadas

| # | Verificação | Exemplo detectado |
|---|-------------|-------------------|
| 1 | **Variável não declarada** | `x = naoexiste;` |
| 2 | **Declaração dupla no mesmo escopo** | `int x; int x;` |
| 3 | **Incompatibilidade de tipos** (atribuição) | `int x; x = "texto";` |
| 4 | **Tipos dos argumentos de funções** | `soma(1, umaStruct)` |
| 5 | **Número de argumentos de funções** | `soma(1)` quando espera 2 |
| 6 | **Condições das estruturas de controlo** (`if`, `while`, `for`, `do-while`, `switch`) devem ser escalares/inteiras | `if (umaStruct) { ... }` |
| 7 | **`return`** compatível com o tipo de retorno; `void` não retorna valor | `void f(){ return 7; }` |
| 8 | **`break`/`continue`** apenas dentro de ciclo/switch | `break;` fora de ciclo |
| 9 | **Operadores** com operandos válidos (ex.: `%`, `&`, `<<` exigem inteiros; `*`/`->` exigem ponteiro/struct) | `3.5 % 2` |
| 10 | **Acesso a membros** de struct/union (`.` e `->`) | `p.campoInexistente` |
| 11 | **`lvalue`** no lado esquerdo de atribuições e em `&`, `++`, `--` | `2 = x;` |
| 12 | **Avisos** de conversão com perda (ex.: `double → int`) e ponteiros incompatíveis | `int x = 3.5;` |

As chamadas a funções da biblioteca padrão (ex.: `printf`, `malloc`, `strlen`) são
reconhecidas através de uma tabela de *builtins*; funções desconhecidas provenientes de
cabeçalhos externos (`#include`) geram apenas um **aviso** e são assumidas como externas,
evitando falsos positivos.

## Arquitectura (módulos)

Seguindo a mesma organização modular do Parser:

| Ficheiro | Responsabilidade |
|----------|------------------|
| `main.c` | Ponto de entrada: lê o ficheiro, invoca Parser e Analisador, imprime resultados |
| `semantic.c` | Inicialização, registo de *builtins* e impressão de símbolos/diagnósticos |
| `sem_decl.c` | Duas passagens: recolha de declarações globais (funções, typedefs, structs, macros) e verificação dos corpos |
| `sem_stmt.c` | Verificação de instruções, blocos, declarações de variáveis e estruturas de controlo |
| `sem_expr.c` | Inferência de tipos das expressões e verificação de compatibilidade |
| `sem_type_build.c` | Construção de tipos a partir da AST e registo de etiquetas struct/union |
| `sem_types.c` | Sistema de tipos (`SemType`) e regras de classificação/compatibilidade |
| `sem_symtab.c` | Tabela de símbolos com pilha de escopos |
| `sem_error.c` | Coleccão de diagnósticos (erros e avisos) |

### Metodologia

1. **Duas passagens** sobre as declarações globais: a primeira *regista* as assinaturas
   das funções, os `typedef`, as etiquetas `struct`/`union` e as macros `#define`
   (permitindo referências antecipadas e recursão mútua); a segunda *verifica* os corpos
   das funções e as variáveis globais pela ordem do programa (garantindo a regra
   "declarar antes de usar").
2. **Tabela de símbolos com escopos**: `sem_scope_enter`/`sem_scope_exit` mantêm uma pilha
   de escopos. As redeclarações são detectadas no escopo corrente e a resolução de nomes
   percorre do escopo mais interno para o mais externo. Os parâmetros partilham o escopo do
   corpo da função.
3. **Sistema de tipos** (`SemType`): tipo base + nível de ponteiro + nível de array +
   nome de etiqueta. As regras distinguem tipos **aritméticos**, **ponteiros**,
   **agregados** (struct/union) e **void**, aplicando as conversões usuais e assinalando
   incompatibilidades como erro e conversões com perda como aviso.
