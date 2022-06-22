/*
***REMOVED*** * Funciones de interpretación y construcción del arbol sintáctico
*/
#include "functions.h"
#include "parser.tab.h"
#include <stdio.h>

// Declaración de apuntadores que servirán para la tabla de símbolos principal
Node *treeRoot = NULL;
Node *symbolRoot = NULL;
Node *funcRoot = NULL;
Node *currentSymbolTable = NULL;
Node *currentParamTable = NULL;
Node *currentFunc = NULL;
int paramCounter = 0;
int argCounter = 0;

// Función que termina la ejecución del programa e imprime un mensaje personalizado
void die(const char *s) 
{
  fprintf(stderr, "Error: %s\n", s);
  exit(1);
}

// Función que termina la ejecución del programa. Imprime un mensaje personalizado, la linea y el número del token que falló
void die_line(const char *s) 
{
  fprintf(stderr, "Error: %s en linea %d con 'token' %s\n", s, yylineno, yytext);
  exit(1);
}

// Función que imprime un mensaje de advertencia a stdout
void msg_warning(const char *s)
{
  fprintf(stdout, "Advertencia: %s en linea %d\n", s, yylineno);
}

/*
* Imprime un árbol sintáctico con identación. La identación debe comenzar en un valor mayor o igual a uno y simboliza la profundidad en la que se encuentra el elemento del árbol que se imprime. El step es solo el paso que le sumarémos a la identación para indicar los niveles. 
* Se revisa el tipo y subtipo para imprimir correctamente cada nodo. Al final llamamos a los campos op* del nodo y los imprimimos
*/
void treePrint(Node *node, int indent, int step)
{
  if (indent < 1) die("La identación no puede ser negativa en treePrint");
  if (node == NULL) die("Nodo negativo como argumento a treePrint");

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
        case FUN_CALL:
          printf("%*cRead: T_SENTENCE\tsubtype: CALL\tname: %s\n", indent, ' ', node->name);
          break;
        case RETURN:
          printf("%*cRead: T_SENTENCE\tsubtype: RETURN\n", indent, ' ');
          break;
        default:
          die("Recibido subtipo desconocido en nodo de tipo T_SENTENCE dentro de treePrint");
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
    default:
      die("Recibido tipo de nodo desconocido en treePrint\n");
      break;
  }
  if (node -> op1 != NULL) treePrint(node -> op1, indent+step, step);
  if (node -> op2 != NULL) treePrint(node -> op2, indent+step, step);
  if (node -> op3 != NULL) treePrint(node -> op3, indent+step, step);
  if (node -> op4 != NULL) treePrint(node -> op4, indent+step, step);
  // Si el nodo es variable no imprimimos su next porque las variables en next contienen apuntadores a la lista de símbolos
  if (node -> type != T_VARIABLE && node -> next != NULL) treePrint(node -> next, indent, step);
}

//Imprime la table de símbolos que es una lista ligada
void tablePrint(Node *t)
{
  if (t == NULL) die("Nodo nulo como argumento a tablePrint");
  if (t -> type != T_FUNCTION && t -> type != T_VARIABLE) die("Tipo incorrecto como argumento a tablePrint");

  Node *tmp = malloc(sizeof(Node));
  tmp = t;

  while (tmp != NULL) {
    switch (tmp -> type) {
      case T_VARIABLE:
        if (tmp -> subtype == INT) {
          printf("nombre: %s, tipo: %d, valor: %d\n", tmp -> name, tmp -> type, *tmp -> value -> val_int);
        } else {
          printf("nombre: %s, tipo: %d, valor: %.2f\n", tmp -> name, tmp -> type, *tmp -> value -> val_float);
        }
        break;
      case T_FUNCTION:
        printf("nombre: %s, retorno: %d\n", tmp -> name, tmp -> type );
        if (tmp -> op1 != NULL) {
          printf("tabla de símbolos\n"); 
          tablePrint(tmp -> op1);
        }
        if (tmp -> op2 != NULL) {
          printf("parámetros\n");
          tablePrint(tmp -> op2);
        }
        if (tmp -> op3 != NULL) {
          printf("cuerpo de función\n");
          treePrint(tmp -> op3, 1, 1);
        }
      break;
    }
    tmp = tmp -> next;
  }

  tmp = NULL;
  free(tmp);
}

// Busca en la table de símbolos la variable con el nombre especificado
Node* tableGet(Node **t, const char *name)
{
  if (t == NULL) die("El argumento t pasado a tableGet es NULL");
  if (*t == NULL) die_line("El argumento t pasado a tableGet apunta a NULL");
  if ((*t) -> type != T_VARIABLE && (*t) -> type != T_FUNCTION) die("El tipo de tabla pasado a tableGet es incorrecto");

  Node *tmp = *t; 
  while (tmp != NULL) {
    if (strcmp(tmp -> name, name) == 0)
      return tmp;
    tmp = tmp -> next;
  }

  return NULL;
}

Node* assignCheck(char *var_name, Node *n2)
{
  if (n2 == NULL) die("Valor nulo a assignCheck");
  if (n2 -> type != T_CONSTANT && n2 -> type != T_VARIABLE && n2 -> type != T_SENTENCE && n2 -> type != T_ARITHMETIC_OP) die("Se mando un valor de tipo incorrecto a assignCheck");
  if (!(n2 -> subtype >= SUM && n2 -> subtype <= GRETOREQ) && n2 -> subtype != INT && n2 -> subtype != FLOAT && n2 -> subtype != FUN_CALL) die("Se mando un valor de subtipo incorrecto a assignCheck");

  if (n2 -> subtype != FUN_CALL) {
    if (currentFunc != NULL && currentFunc -> op2 != NULL && tableGet(&currentFunc -> op2, var_name) != NULL && subtypeGetCommon(tableGet(&currentFunc -> op2, var_name) -> subtype, treeGetType(n2)) != -1) {
      return nodeNew(T_SENTENCE, ASSIGN, NULL, NULL, NULL, tableGet(&currentFunc -> op2, var_name), n2, NULL, NULL); 
    } else if (currentFunc != NULL && currentFunc -> op1 != NULL && tableGet(&currentFunc -> op1, var_name) != NULL && subtypeGetCommon(tableGet(&currentFunc -> op1, var_name) -> subtype, treeGetType(n2)) != -1) { 
      return nodeNew(T_SENTENCE, ASSIGN, NULL, NULL, NULL, tableGet(&currentFunc -> op1, var_name), n2, NULL, NULL); 
    } else if (symbolRoot != NULL && tableGet(&symbolRoot, var_name) != NULL && subtypeGetCommon(tableGet(&symbolRoot, var_name) -> subtype, treeGetType(n2)) != -1) {
      return nodeNew(T_SENTENCE, ASSIGN, NULL, NULL, NULL, tableGet(&symbolRoot, var_name), n2, NULL, NULL); 
    } else {
      die_line("Los 'subtypes' son distintos en ASSIGN o no se encontró la variable"); 
    }
  } else {
    if (currentFunc != NULL && currentFunc -> op2 != NULL && tableGet(&currentFunc -> op2, var_name) != NULL && n2 -> op1 -> subtype == tableGet(&currentFunc -> op2, var_name) -> subtype) {
      return nodeNew(T_SENTENCE, ASSIGN, NULL, NULL, NULL, tableGet(&currentFunc -> op2, var_name), n2, NULL, NULL); 
    } else if (currentFunc != NULL && currentFunc -> op1 != NULL && tableGet(&currentFunc -> op1, var_name) != NULL && n2 -> op1 -> subtype == tableGet(&currentFunc -> op1, var_name) -> subtype) { 
      return nodeNew(T_SENTENCE, ASSIGN, NULL, NULL, NULL, tableGet(&currentFunc -> op1, var_name), n2, NULL, NULL); 
    } else if (symbolRoot != NULL && tableGet(&symbolRoot, var_name) != NULL && n2 -> op1 -> subtype == tableGet(&symbolRoot, var_name) -> subtype) {
      return nodeNew(T_SENTENCE, ASSIGN, NULL, NULL, NULL, tableGet(&symbolRoot, var_name), n2, NULL, NULL); 
    } else {
      die_line("Los 'subtypes' de la función en assign no son compatibles");
    }
  }

  die("Error en assignCheck");
  return NULL;
}

// Crea un nuevo nodo y regresa un apuntador a este
Node* nodeNew(int type, int subtype, char *name, Value *value, Node *next, Node *op1, Node *op2, Node *op3, Node *op4)
{
  // Revisamos si el tipo y subtipo son validos
  if (type < T_SENTENCE || type > T_LOGICAL_OP) die("Tipo incorrecto como argumento a nodeNew");
  if ( !(subtype >= INT && subtype <= FLOAT) && !(subtype >= READ && subtype <= REPEAT) ) die("Subtipo incorrecto como argumento a nodeNew");

  Node *tmp = malloc(sizeof(Node));
  tmp -> type = type;
  tmp -> subtype = subtype;

  tmp -> value = value;
  tmp -> next = next;
  tmp -> op1 = op1;
  tmp -> op2 = op2;
  tmp -> op3 = op3;
  tmp -> op4 = op4;

  // Copiamos el nombre si no es NULL al apartado de nombre del struct
  if (name != NULL) {
    strcpy(tmp -> name, name);
  }
  return tmp;
}

// Añade el nodo que le mandamos a la table de símbolos, comprueba que sea de tipo variable para que no se agregue otro tipo de elemento a la tabla
void tableAddNode(Node **t, Node *n)
{
  if (t == NULL) die("t nulo como argumento a tableAddNode");
  if (n == NULL) die("Node nulo como argumento a tableAddNode");
  if (n -> type != T_VARIABLE && n -> type != T_FUNCTION && n -> type != T_CONSTANT && n -> type != T_ARITHMETIC_OP) die("Nodo incorrecto como argumento a tableAddNode");

  if (*t == NULL) {
    *t = n;
  } else {
    Node *tmp = *t;
    while (tmp -> next != NULL) {
      tmp = tmp -> next; 
    }
    tmp -> next = n;
  }
}

// Crea un nuevo Value y comprueba que su subtipo sea valido
Value* valueNew(int type, int val_int, double val_float)
{
  if (type != INT && type != FLOAT) die("El tipo pasado a valueNew no es INT ni FLOAT");

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

// Compara los dos tipos, si son iguales regresa el comun, si no tienen regresa -1
int typeGetCommon(int t1, int t2)
{
  // Comprobamos que se pasen argumentos dentro del enum hecho por bison
  if ( !(t1 >= BEGN && t1 <= ID) && !(t2 >= BEGN && t2 <= ID)) die("Tipos invalidos como argumento a typeGetCommon");

  if (t1 == t2)
    return t1;
  return -1;
}

// Compara los dos subtipos, si son iguales regresa el comun, si no tienen regresa -1
int subtypeGetCommon(int s1, int s2)
{
  // Comprobamos que se pasen argumentos dentro del enum hecho por bison
  if ( !(s1 >= BEGN && s1 <= ID) && !(s2 >= BEGN && s2 <= ID)) die("Subtipos invalidos como argumento a subtypeGetCommon");

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
  if (n -> type != T_VARIABLE && n -> type != T_CONSTANT && n -> type != T_ARITHMETIC_OP && n -> type != T_SENTENCE) die("Nodo invalido como argumento a treeGetType");

  if (n -> op1 == NULL && n -> op2 == NULL) {
    return n -> subtype;
  }
  else if (n -> subtype == FUN_CALL) {
    return treeGetType(tableGet(&funcRoot, n -> name) -> op2);
  }

  return subtypeGetCommon(treeGetType(n -> op1), treeGetType(n -> op2));
}

/*
* Lee entrada del usuario y la escribe dentro de una variable que le pasemos
*/
void varRead(Node *symb)
{
  if (symb -> subtype != INT && symb -> subtype != FLOAT) die("Se entró a varRead con un nodo incorrecto");

  char *str = malloc(sizeof(char) * MAX_NUM_LEN);
  char **tmp = NULL;
  int* val_int;
  double* val_float;
  
  if (symb -> subtype == INT) {
    val_int = malloc(sizeof(int));

    fgets(str, MAX_NUM_LEN, stdin);
    *val_int = strtoimax(str, tmp, 10);

    symb -> value -> val_int = val_int; 
  } else {
    val_float = malloc(sizeof(double));

    fgets(str, MAX_NUM_LEN, stdin);
    *val_float = strtod(str, tmp);

    symb -> value -> val_float = val_float; 
  }

  str = NULL;
  free(str);
}

/*
* Imprime un Value
*/
void valuePrint(Value* val)
{
  if (val == NULL) die("Entrado a valuePrint con val nulo");

  if (val -> type == INT) {
    printf("%d\n", *val -> val_int);
  } else {
    printf("%.2f\n", *val -> val_float);
  }
}

/*
* Regresa un nuevo Value que es la suma de los dos que pasamos como argumento
*/
Value* valueSum(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueSum llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    return valueNew(INT, *n1 -> val_int + *n2 -> val_int, 0);
  } else {
    return valueNew(FLOAT, 0, *n1 -> val_float + *n2 -> val_float);
  }

  return NULL;
}

/*
* Regresa un nuevo Value que es la resta de los dos que pasamos como argumento
*/
Value* valueRes(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueRes llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    return valueNew(INT, *n1 -> val_int - *n2 -> val_int, 0);
  } else {
    return valueNew(FLOAT, 0, *n1 -> val_float - *n2 -> val_float);
  }

  return NULL;
}

/*
* Regresa un nuevo Value que es la divición de los dos que pasamos como argumento
*/
Value* valueDiv(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueDiv llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    return valueNew(INT, *n1 -> val_int / *n2 -> val_int, 0);
  } else {
    return valueNew(FLOAT, 0, *n1 -> val_float / *n2 -> val_float);
  }

  return NULL;
}

/*
* Regresa un nuevo Value que es la multiplicación de los dos que pasamos como argumento
*/
Value* valueMul(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueMul llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    return valueNew(INT, *n1 -> val_int * *n2 -> val_int, 0);
  } else {
    return valueNew(FLOAT, 0, *n1 -> val_float * *n2 -> val_float);
  }

  return NULL;
}

/*
* Regresa un valor entero que es el resultado de la comparación de los dos valores que pasamos como argumento
* Básicamente es un '<'
*/
int valueLestn(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueLestn llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int < *n2 -> val_int) return 1 ; else return 0;
  } else {
    if (*n1 -> val_float < *n2 -> val_float) return 1 ; else return 0;
  }
  
  return 0;
}

/*
* Regresa un valor entero que es el resultado de la comparación de los dos valores que pasamos como argumento
* Básicamente es un '>'
*/
int valueGretn(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueGretn llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int > *n2 -> val_int) return 1 ; else return 0;
  } else {
    if (*n1 -> val_float > *n2 -> val_float) return 1 ; else return 0;
  }
  
  return 0;
}

/*
* Regresa un valor entero que es el resultado de la comparación de los dos valores que pasamos como argumento
* Básicamente es un '=='
*/
int valueEqual(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueEqual llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int == *n2 -> val_int) return 1 ; else return 0;
  } else {
    if (*n1 -> val_float == *n2 -> val_float) return 1 ; else return 0;
  }
  
  return 0;
}

/*
* Regresa un valor entero que es el resultado de la comparación de los dos valores que pasamos como argumento
* Básicamente es un '<='
*/
int valueLessOrEq(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueLessOrEq llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int <= *n2 -> val_int) return 1; else return 0;
  } else {
    if (*n1 -> val_float <= *n2 -> val_float) return 1; else return 0;
  }
  
  return 0;
}

/*
* Regresa un valor entero que es el resultado de la comparación de los dos valores que pasamos como argumento
* Básicamente es un '>='
*/
int valueGretOrEq(Value *n1, Value *n2)
{
  if (n1 -> type != INT && n1 -> type != FLOAT) die("valueGretOrEq llamado con nodos incorrectos");

  if (n1 -> type == INT) {
    if (*n1 -> val_int >= *n2 -> val_int) return 1; else return 0;
  } else {
    if (*n1 -> val_float >= *n2 -> val_float) return 1; else return 0;
  }
  
  return 0;
}

/*
* Función que interpreta el nodo IF
*/
Node* interpretIf(Node *node)
{
  if (node == NULL) die("interpretIf llamado con NULL");
  if (node -> subtype != IF) die("interpretIf llamado con nodo incorrecto");

  if (logicalOpEval(node -> op1)) {
    return interpretNode(node -> op2);
  }
  
  return NULL;
}

/*
* Función que interpreta el nodo IF_ELSE
*/
Node* interpretIfElse(Node *node)
{
  if (node == NULL) die("interpretIfElse llamado con NULL");
  if (node -> subtype != IF_ELSE) die("interpretIfElse llamado con nodo incorrecto");

  if (logicalOpEval(node -> op1)) {
    return interpretNode(node -> op2);
  } else {
    return interpretNode(node -> op3);
  }
  
  return NULL;
}

/*
* Función que interpreta el nodo WHILE
*/
void interpretWhile(Node *node)
{
  if (node == NULL) die("interpretWhile llamado con NULL");
  if (node -> subtype != WHILE) die("interpretWhile llamado con nodo incorrecto");

  while (logicalOpEval(node -> op1)) {
    // Es un nodo BEGN_END y por eso lo llamamos con su op1
    interpretNode(node -> op2 -> op1);
  }
}

/*
* Función que interpreta el nodo REPEAT
*/
void interpretRepeat(Node *node)
{
  if (node == NULL) die("interpretRepeat llamado con NULL");
  if (node -> subtype != REPEAT) die("interpretRepeat llamado con nodo incorrecto");

  do {
    interpretNode(node -> op1);    
  } while (logicalOpEval(node -> op2));
}

/*
* Función que interpreta el nodo FOR
*/
void interpretFor(Node *node)
{
  if (node == NULL) die("interpretFor llamado con NULL");
  if (node -> subtype != FOR) die("interpretFor llamado con nodo incorrecto");
  if (tableGet(&symbolRoot, node -> name) -> type != T_VARIABLE) die("Tipo de tabla incorrecto como argumento a interpretFor");

  int int_tmp;
  double float_tmp;
  Node *var; 
  if (currentFunc != NULL && tableGet(&currentFunc -> op2, node -> name)) {
    var = tableGet(&currentFunc -> op2, node -> name);
  } else if (currentFunc != NULL && tableGet(&currentFunc -> op1, node -> name) != NULL) {
    var = tableGet(&currentFunc -> op1, node -> name);
  } else if (tableGet(&symbolRoot, node -> name) != NULL) {
    var = tableGet(&symbolRoot, node -> name);
  } else {
    die("Variable no encontrada en for");
  }

  // Guardamos el valor de la variable para luego restaurarlo
  if (var -> subtype == INT) {
    int_tmp = *var -> value -> val_int;
  } else {
    float_tmp = *var -> value -> val_float;
  }

  // Asignamos a la variable apuntada por FOR el valor del op1
  valueAssign(var -> value, arithOpEval(node -> op1)); 

  // Revisamos que la condición se cumpla y realizamos la operación
  while (valueEqual(var -> value, arithOpEval(node -> op2)) != 1) {
    interpretNode(node -> op4);
    valueAssign(var -> value, valueSum(var -> value, arithOpEval(node->op3)));
  } 

  // Regresamos su valor anterior a la variable
  if (var -> subtype == INT) {
    valueAssign(var -> value, valueNew(INT, int_tmp, 0));
  } else {
    valueAssign(var -> value, valueNew(FLOAT, 0, float_tmp));
  }
}

Node* tableGetIndex(Node **t, int i)
{
  if (t == NULL) die("El argumento t pasado a tableGetIndex es NULL");
  if (*t == NULL) die_line("El argumento t pasado a tableGetIndex apunta a NULL");
  if ((*t) -> type != T_VARIABLE && (*t) -> type != T_FUNCTION && (*t) -> type != T_CONSTANT) die("El tipo de tabla pasado a tableGetIndex es incorrecto");

  int cnt = 0;
  Node *tmp = *t; 
  while (tmp != NULL && cnt != i) {
    tmp = tmp -> next;
    cnt++;
  }
  
  if (tmp == NULL) {
    die("Elemento no encontrado en tableGetIndex");
  }

  return tmp;
}

Node* interpretFunCall(Node *node)
{
  if (node == NULL) die("interpretFunCall llamado con argumento NULL");
  if (node -> subtype != FUN_CALL) die("interpretFunCall llamado con tipo de nodo incorrecto");

  Node *res = NULL;
  Node *fun = tableGet(&funcRoot, node -> name);

  Node *fun_pm_back = malloc(sizeof(Node) * (*fun -> value -> val_int));
  memcpy(fun_pm_back, fun -> op2, sizeof(Node) * (*fun -> value -> val_int));

  for (int i = 0; i < *fun -> value -> val_int; i++) {
    tableGetIndex(&fun->op2, i) -> value = arithOpEval(tableGetIndex(&node -> op1, i));
  }

  res = interpretNode(fun -> op3);

  fun -> op2 = fun_pm_back;
  fun_pm_back = NULL;

  return res;
}

/*
* Función que evalua las operaciones aritméticas
*/
Value* arithOpEval(Node *node)
{
  if (node == NULL) die("Mandado a llamar arithOpEval con NULL");
  if (node -> type != T_ARITHMETIC_OP && node -> type != T_CONSTANT && node -> type != T_VARIABLE && node -> type != T_SENTENCE) die("Expresión incorrecta como argumento a arithOpEval");

  Node *tmp = NULL;

  switch (node -> subtype) {
    case SUM:
      return valueSum(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break;
    case RES:
      return valueRes(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break; 
    case MUL:
      return valueMul(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break; 
    case DIV:
      return valueDiv(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break;
    case INT:
      return node -> value;
      break; 
    case FLOAT:
      return node -> value;
      break;
    case FUN_CALL:
      tmp = interpretFunCall(node);  
      if (tmp != NULL && tmp -> value != NULL) {
        return tmp -> value;
      } else {
        die("Error al ejecutar función");
      }
      break;
    default:
      die("Nodo con 'subtype' incorrecto en arithOpEval");
      break;
  }

  return NULL;
}

/*
* Función que evalua las operaciones lógicas
*/
int logicalOpEval(Node *node)
{
  if (node == NULL) die("Mandado a llamar logicalOpEval con NULL");
  if (node -> type != T_LOGICAL_OP) die("Expresión incorrecta para evaluar en logicalOpEval");

  switch (node -> subtype) {
    case LESTN:
      return valueLestn(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break;
    case GRETN:
      return valueGretn(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break;
    case EQUALS:
      return valueEqual(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break;
    case LESSOREQ:
      return valueLessOrEq(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break;
    case GRETOREQ:
      return valueGretOrEq(arithOpEval(node -> op1), arithOpEval(node -> op2));
      break;
    default:
      die("Expresión pasada a logicalOpEval con subtipo desconocido");
      break;
  }

  return 0;
}

/*
* Función que asigna un Value a otro
*/
void valueAssign(Value *var, Value *expr)
{
  if (var == NULL || expr == NULL) die("Valores nulos pasados a valueAssign");
  if (var -> type != expr -> type) die("Asignación incorrecta en valueAssign");

  if (var -> type == INT) {
    var -> val_int = expr -> val_int;
  } else {
    var -> val_float = expr -> val_float;
  }
  expr = NULL;
  free(expr);
}

void interpretAssign(Node *node)
{
  if (node == NULL) die("Nodo NULL como argumento a interpretAssign");
  if (node -> subtype != ASSIGN) die("Subtipo invalido como argumento a interpretAssign");

  valueAssign(node -> op1 -> value, arithOpEval(node -> op2));
}

void interpretPrint(Node *node)
{
  if (node == NULL) die("Nodo nulo como argumento a interpretPrint");
  if (node -> subtype != PRINT) die("Nodo incorrecto como argumento a interpretPrint");

  Node *tmp = node -> op1;

  if (tmp -> subtype == FUN_CALL) {
    interpretPrint(nodeNew(T_SENTENCE, PRINT, NULL, NULL, NULL, interpretFunCall(node -> op1), NULL, NULL, NULL));
  } else  {
    valuePrint(arithOpEval(node -> op1));
  }
}

Node* interpretReturn(Node *node)
{
  if (node == NULL) die("Argumento NULL pasado a interpretReturn");
  if (node -> subtype != RETURN) die("Argumento de subtipo distinto como argumento a interpretReturn");

  return nodeNew(T_CONSTANT, treeGetType(node -> op1), NULL, arithOpEval(node->op1), NULL, NULL, NULL, NULL, NULL);   
}

/*
* Función que 'corre' el programa. Dependiendo del subtipo de cada nodo llama a la función 'interpret*' correspondiente
* Es recursiva y termina cuando ya no hayan más instrucciones
*/
Node* interpretNode(Node *node)
{
  if (node == NULL) die("Llamado a interpretNode con NULL");

  switch (node -> subtype) {
    case ASSIGN:
      interpretAssign(node);
      break;
    case READ:
      varRead(node -> op1);
      break;
    case PRINT:
      interpretPrint(node);
      break;
    case IF:
      return interpretIf(node); 
      break;
    case IF_ELSE:
      return interpretIfElse(node);
      break;
    case WHILE:
      interpretWhile(node);
      break;
    case REPEAT:
      interpretRepeat(node);
      break;
    case FOR:
      interpretFor(node);
      break;
    case FUN_CALL:
      interpretFunCall(node);
      break; 
    case RETURN:
      return interpretReturn(node);
      break;
    default:
      die("Entrado a interpretNode con subtipo no valido");
      break;  
  }

  if (node -> next != NULL) return interpretNode(node -> next);
  return NULL;
}

int functionCheckValidArgs(Node *params, int params_size, Node *args, int args_size)
{
  if (params == NULL) die("Params nulo pasado a functionCheckValidArgs como argumento");
  if (args == NULL) die("Args nulo pasado a functionCheckValidArgs como argumento");
  if (params -> type != T_VARIABLE) die("Tipo de nodo incorrecto como argumento a functionCheckValidArgs en params");

  Node *tmp_params = params;
  Node *tmp_args = args;

  if (params_size == args_size) {
      for (int i = 0; i < params_size; i++) {
        if (subtypeGetCommon(treeGetType(tmp_args), tmp_params -> subtype) == -1) die_line("Argumentos invalidos a función");
        
        tmp_params = tmp_params -> next;
        tmp_args = tmp_args -> next;
      }
  } else {
    die_line("Número de argumentos distintos a función");
  }

  return 1;
}

// Función personalizada de yyerror, para que siempre se llame la función personalizada die_line
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
  interpretNode(treeRoot);
  return 0;
}
