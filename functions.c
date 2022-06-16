/*
***REMOVED*** * Funciones de interpretación y construcción del arbol sintáctico
*/
#include "functions.h"

// Declaración de apuntadores que servirán para la tabla de símbolos principal
Node *treeRoot = NULL;
Node *symbolRoot = NULL;

// Función que termina la ejecución del programa. Imprime un código, linea y el número del token que falló
void die(const char *s) 
{
  fprintf(stderr, "Error: %s\n", s);
  exit(1);
}

void die_line(const char *s) 
{
  fprintf(stderr, "Error: %s en linea %d con 'token' %s\n", s, yylineno, yytext);
  exit(1);
}

void msg(const char *s)
{
  fprintf(stderr, "Advertencia: %s en linea %d\n", s, yylineno);
}

/*
* Imprime un árbol sintáctico con identación. La identación debe comenzar en un valor mayor o igual a uno y simboliza la profundidad en la que se encuentra el elemento del árbol que se imprime. El step es solo el paso que le sumarémos a la identación para indicar los niveles. 
* Se revisa el tipo y subtipo para imprimir correctamente cada nodo. Al final llamamos a los campos op* del nodo y los imprimimos
*/
void printTree(Node *node, int indent, int step)
{
  if (indent < 1) die("Indent can not be negative in printTree");

  if (node != NULL) {
    //printf("call %d\naddress: %p\ntype: %d\nsubtype: %d\nnext: %p\nop1: %p\nop2: %p\nop3: %p\nop4: %p\n", indent, (void *)node, node -> type, node -> subtype, (void *)node -> next, (void *)node -> op1, (void *)node -> op2, (void *)node -> op3, (void *)node -> op4);
    switch (node -> type) {
      case T_SENTENCE:
        switch (node -> subtype) {
          case ASSIGN:
            printf("%*cRead: T_SENTENCE\tsubtype: ASSIGN\n", indent, ' ');
            break;
          case IF:
            printf("%*cRead: T_SENTENCE\tsubtype: IF\n", indent, ' ');
            break;
          case IF_ELSE:
            printf("%*cRead: T_SENTENCE\tsubtype: IF_ELSE\n", indent, ' ');
            break;
          case WHILE:
            printf("%*cRead: T_SENTENCE\tsubtype: WHILE\n", indent, ' ');
            break;
          case REPEAT:
            printf("%*cRead: T_SENTENCE\tsubtype: REPEAT\n", indent, ' ');
            break;
          case FOR:
            printf("%*cRead: T_SENTENCE\tsubtype: FOR\n", indent, ' ');
            break;
          case READ:
            printf("%*cRead: T_SENTENCE\tsubtype: READ\n", indent, ' ');
            break;
          case PRINT:
            printf("%*cRead: T_SENTENCE\tsubtype: PRINT\n", indent, ' ');
            break;
          case BEGN_END:
            printf("%*cRead: T_SENTENCE\tsubtype: BEGN_END\n", indent, ' ');
            break;
        }
        break;
      case T_VARIABLE:
        switch (node -> subtype) {
          case INT: 
            printf("%*cRead: T_VARIABLE\tsubtype: INT\tname: %s value: %d\n", indent, ' ', node -> name, *node -> value -> val_int);
            break;
          case FLOAT:
            printf("%*cRead: T_VARIABLE\tsubtype: FLOAT\tname: %s value: %f\n", indent, ' ', node -> name, *node -> value -> val_float);
            break;
        }
        break;
      case T_CONSTANT:
        switch (node -> subtype) {
          case INT:
            printf("%*cRead: T_CONSTANT\tsubtype: INT\n", indent, ' ');
            break;
          case FLOAT:
            printf("%*cRead: T_CONSTANT\tsubtype: FLOAT\n", indent, ' ');
            break;
        }
        break;
      case T_ARITHMETIC_OP:
        switch (node -> subtype) {
          case SUM:
            printf("%*cRead: T_ARITH_OP\tsubtype: SUM\n", indent, ' ');
            break;
          case RES:
            printf("%*cRead: T_ARITH_OP\tsubtype: RES\n", indent, ' ');
            break;
          case MUL:
            printf("%*cRead: T_ARITH_OP\tsubtype: MUL\n", indent, ' ');
            break;
          case DIV:
            printf("%*cRead: T_ARITH_OP\tsubtype: DIV\n", indent, ' ');
            break;
        }
        break;
      case T_LOGICAL_OP:
        switch (node -> subtype) {
          case LESTN:
            printf("%*cRead: T_LOGICAL_OP\tsubtype: LESTN\n", indent, ' ');
            break;
          case GRETN:
            printf("%*cRead: T_LOGICAL_OP\tsubtype: GRETN\n", indent, ' ');
            break;
          case EQUALS:
            printf("%*cRead: T_LOGICAL_OP\tsubtype: EQUALS\n", indent, ' ');
            break;
          case LESSOREQ:
            printf("%*cRead: T_LOGICAL_OP\tsubtype: LESSOREQ\n", indent, ' ');
            break;
          case GRETOREQ:
            printf("%*cRead: T_LOGICAL_OP\tsubtype: GRETOREQ\n", indent, ' ');
            break;
        }
        break;
    }
    if (node -> op1 != NULL) printTree(node -> op1, indent+step, step);
    if (node -> op2 != NULL) printTree(node -> op2, indent+step, step);
    if (node -> op3 != NULL) printTree(node -> op3, indent+step, step);
    if (node -> op4 != NULL) printTree(node -> op4, indent+step, step);
    // Si el nodo es variable no imprimimos su next porque las variables en next contienen apuntadores a la lista de símbolos
    if (node -> type != T_VARIABLE && node -> next != NULL) printTree(node -> next, indent, step);
  }
}

//Imprime la table de símbolos que es una simple lista ligada
void printSymbolTable(Node *st)
{
  if (st == NULL) return;
  Node *tmp = malloc(sizeof(Node));
  tmp = st;

  while (tmp != NULL) {
    if (tmp -> value -> type == INT) {
      printf("nombre: %s, tipo: %d, valor: %d\n", tmp -> name, tmp -> type, *tmp -> value -> val_int);
    }
    else {
      printf("nombre: %s, tipo: %d, valor: %.2f\n", tmp -> name, tmp -> type, *tmp -> value -> val_float);
    }
    tmp = tmp -> next;
  }

  tmp = NULL;
  free(tmp);
}

// Busca en la table de símbolos la variable con el nombre especificado
Node* getSymbolTableNode(Node **st, const char *name)
{
  Node *tmp = *st; 
  while (tmp != NULL) {
    if (strcmp(tmp -> name, name) == 0)
      return tmp;
    tmp = tmp -> next;
  }

  free(tmp);
  die("Variable no encontrada en la tabla de símbolos");
  return NULL;
}

// Crea un nuevo nodo y regresa un apuntador a este
Node* newNode(int type, int subtype, char *name, Value *value, Node *next, Node *op1, Node *op2, Node *op3, Node *op4)
{
  Node *tmp = malloc(sizeof(Node));
  tmp -> type = type;
  tmp -> subtype = subtype;
  tmp -> value = value;
  tmp -> next = next;
  tmp -> op1 = op1;
  tmp -> op2 = op2;
  tmp -> op3 = op3;
  tmp -> op4 = op4;
  if (name != NULL) {
    strcpy(tmp -> name, name);
  }
  return tmp;
}

// Añade el nodo que le mandamos a la table de símbolos, comprueba que sea de tipo variable para que no se agregue otro tipo de elemento a la tabla
void addNodeToSymbolTable(Node **st, Node *n)
{
  if (n == NULL) die("Node nulo como argumento a addNodeToSymbolTable");
  if (n -> type != T_VARIABLE) die("Nodo incorrento como argumento a addNodeToSymbolTable");
  Node *tmp;

  if (*st == NULL) {
    *st = n;
  } else {
    tmp = *st;
    while (tmp -> next != NULL) {
      tmp = tmp -> next; 
    }
    tmp -> next = n;
  }
}

// Crea un nuevo Value y comprueba que su subtipo sea valido
Value* newValue(int type, int val_int, double val_float)
{
  if (type != INT && type != FLOAT) die("El tipo pasado a newValue no es INT ni FLOAT");

  Value *tmp = malloc(sizeof(Value));
  tmp -> type = type;
  if (type == INT) {
    tmp -> val_int = malloc(sizeof(int));
    *tmp -> val_int = val_int;
  } else {
    tmp -> val_float = malloc(sizeof(double));
    *tmp -> val_float = val_float;
  }

  return tmp;
}

// Regresa el tipo común entre los dos tipos, si no tienen regresa -1
int getCommonType(int t1, int t2)
{
  if (t1 == t2)
    return t1;
  return -1;
}

// Regresa el subtipo común entre los dos tipos, si no tienen regresa -1
int getCommonSubtype(int s1, int s2)
{
  if (s1 == s2)
    return s1;
  return -1;
}

/*
* Función que regresa el tipo de un árbol (se usa con árboles que contienen operaciones aritméticas o lógicas). Comprueba que cada nodo sea del mismo tipo y regresa éste, si no retorna un -1.
* Es una función recursiva que se llama a sí misma desde las hojas hasta la raíz.
*/
int treeGetType(Node *n)
{
  if (n -> op1 == NULL && n -> op2 == NULL)
    return n -> subtype;

  return getCommonSubtype(treeGetType(n -> op1), treeGetType(n -> op2));
}

void stmt_readInt(Node *symb)
{
  char *str = malloc(sizeof(char) * MAX_NUM_LEN);
  char **tmp = NULL;
  int *val_int = malloc(sizeof(int));

  fgets(str, MAX_NUM_LEN, stdin);
  *val_int = strtoimax(str, tmp, 10);

  symb -> value -> val_int = val_int; 
  str = NULL;
  free(str);
}

void stmt_readFloat(Node *symb)
{
  char *str = malloc(sizeof(char) * MAX_NUM_LEN);
  char **tmp = NULL;
  double *val_float = malloc(sizeof(double));

  fgets(str, MAX_NUM_LEN, stdin);
  *val_float = strtod(str, tmp);

  symb -> value -> val_float = val_float; 
  str = NULL;
  free(str);
}

void valuePrint(Value* val)
{
  if (val -> type == INT) {
    printf("%d\n", *val -> val_int);
  } else {
    printf("%.2f\n", *val -> val_float);
  }
}

Value* sumValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    return newValue(INT, *n1 -> val_int + *n2 -> val_int, 0);
  } else {
    return newValue(FLOAT, 0, *n1 -> val_float + *n2 -> val_float);
  }

  return NULL;
}

Value* resValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    return newValue(INT, *n1 -> val_int - *n2 -> val_int, 0);
  } else {
    return newValue(FLOAT, 0, *n1 -> val_float - *n2 -> val_float);
  }

  return NULL;
}

Value* divValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    return newValue(INT, *n1 -> val_int / *n2 -> val_int, 0);
  } else {
    return newValue(FLOAT, 0, *n1 -> val_float / *n2 -> val_float);
  }

  return NULL;
}

Value* mulValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    return newValue(INT, *n1 -> val_int * *n2 -> val_int, 0);
  } else {
    return newValue(FLOAT, 0, *n1 -> val_float * *n2 -> val_float);
  }

  return NULL;
}

int lestnValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int < *n2 -> val_int) return 1 ; else return 0;
  } else {
    if (*n1 -> val_float < *n2 -> val_float) return 1 ; else return 0;
  }
  
  return 0;
}

int gretnValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int > *n2 -> val_int) return 1 ; else return 0;
  } else {
    if (*n1 -> val_float > *n2 -> val_float) return 1 ; else return 0;
  }
  
  return 0;
}

int equalsValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int == *n2 -> val_int) return 1 ; else return 0;
  } else {
    if (*n1 -> val_float == *n2 -> val_float) return 1 ; else return 0;
  }
  
  return 0;
}

int lessoreqValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int <= *n2 -> val_int) return 1; else return 0;
  } else {
    if (*n1 -> val_float <= *n2 -> val_float) return 1; else return 0;
  }
  
  return 0;
}

int gretoreqValues(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("sumValue llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int >= *n2 -> val_int) return 1; else return 0;
  } else {
    if (*n1 -> val_float >= *n2 -> val_float) return 1; else return 0;
  }
  
  return 0;
}

void stmt_if(Node *node)
{
  if (expression_eval(node -> op1)) {
    stmt_interpret(node -> op2);
  }
}

void stmt_if_else(Node *node)
{
  if (expression_eval(node -> op1)) {
    stmt_interpret(node -> op2);
  } else {
    stmt_interpret(node -> op3);
  }
}

void stmt_while(Node *node)
{
  while (expression_eval(node -> op1)) {
    // Es un nodo BEGN_END y por eso lo llamamos con su op1
    stmt_interpret(node -> op2 -> op1);
  }
}

void stmt_repeat(Node *node)
{
  do {
    stmt_interpret(node -> op1);    
  } while (expression_eval(node -> op2));
}

void stmt_for(Node *node)
{
  valueAssign(&getSymbolTableNode(&symbolRoot, node -> name) -> value, expr_eval(node -> op1)); 

  while (equalsValues(getSymbolTableNode(&symbolRoot, node -> name) -> value, expr_eval(node -> op2)) != 1) {
    stmt_interpret(node -> op4);
    valueAssign(&getSymbolTableNode(&symbolRoot, node->name) -> value, sumValues(getSymbolTableNode(&symbolRoot, node -> name) -> value, expr_eval(node->op3)));
  } 
}

Value* expr_eval(Node *node)
{
  if (node -> type != T_ARITHMETIC_OP && node -> type != T_CONSTANT && node -> type != T_VARIABLE) die("Expresión incorrecta como argumento a expr_eval");

  switch (node -> subtype) {
    case SUM:
      return sumValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break;
    case RES:
      return resValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break; 
    case MUL:
      return mulValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break; 
    case DIV:
      return divValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break;
    case INT:
      return node -> value;
      break; 
    case FLOAT:
      return node -> value;
      break;
    default:
      die("Nodo con 'subtype' incorrecto en expr_eval");
      break;
  }

  return NULL;
}

int expression_eval(Node *node)
{
  if (node -> type != T_LOGICAL_OP) die("Expresión incorrecta para evaluar en expression_eval");

  switch (node -> subtype) {
    case LESTN:
      return lestnValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break;
    case GRETN:
      return gretnValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break;
    case EQUALS:
      return equalsValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break;
    case LESSOREQ:
      return lessoreqValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break;
    case GRETOREQ:
      return gretoreqValues(expr_eval(node -> op1), expr_eval(node -> op2));
      break;
    default:
      die("Expresión pasada a expression_eval con subtipo desconocido");
      break;
  }

  return 0;
}

void valueAssign(Value **var, Value *expr)
{
  if ((*var) -> type != expr -> type) die("Asignación incorrecta en valueAssign");
  Value *tmp = *var;

  *var = expr;

  free(tmp);
}

void stmt_interpret(Node *node)
{
  if (node == NULL) return;

  switch (node -> subtype) {
    case ASSIGN:
      valueAssign(&node -> op1 -> value, expr_eval(node -> op2));
      break;
    case READ:
      switch (node -> op1 -> subtype) {
        case INT:
          stmt_readInt(node -> op1);
        break;
        case FLOAT:
          stmt_readFloat(node -> op1);
        break;
      }
      break;
    case PRINT:
      valuePrint(expr_eval(node -> op1));
      break;
    case IF:
      stmt_if(node); 
      break;
    case IF_ELSE:
      stmt_if_else(node);
      break;
    case WHILE:
      stmt_while(node);
      break;
    case REPEAT:
      stmt_repeat(node);
      break;
    case FOR:
      stmt_for(node);
      break;
    default:
      die("Entrado a stmt_interpret con subtipo no valido");
      break;  
  }

  if (node -> next != NULL) stmt_interpret(node -> next);
}

// Función personalizada de yyerror, para que siempre se llame la función personalizada die
int yyerror(char const *s)
{
  die_line(s);
  return 1;
}

int main(int argc, char *argv[])
{
  // Para no lidiar con el nombre del programa
  argc--; argv++;

  if (argc < 1) {
    die ("Falta el nombre de un archivo");
  }

  yyin = fopen(argv[0], "r");

  if (yyin == NULL) {
    die ("El archivo no existe");
  }

  yyparse();
  printf("El programa es valido\n\n");
  printf("Tabla de símbolos\n");
  printSymbolTable(symbolRoot);
  printf("\n");
  printf("Arbol sintáctico\n");
  printTree(treeRoot, 1, 1);
  printf("\n");
  printf("Program execution\n");
  stmt_interpret(treeRoot);
  return 0;
}
