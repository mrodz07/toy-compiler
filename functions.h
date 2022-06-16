/*
***REMOVED*** * Declaraciones de las funciones de interpretación y construcción del arbol sintáctico
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include "parser.tab.h"

// El tamaño máximo del nombre de un identificador
#define MAX_ID_LEN 50
// El tamaño máximo de los números
#define MAX_NUM_LEN 50

// Enumeración que indica el tipo del nodo
typedef enum {
  T_SENTENCE = 0,
  T_VARIABLE,
  T_CONSTANT,
  T_ARITHMETIC_OP,
  T_LOGICAL_OP
} tree_type;

// La estructura valor. Es una estructura que contienen los valores de tipo T_VARIABLE y T_CONSTANT. Guarda un tipo para saber si es INT o FLOAT
typedef struct {
  int    type; 
  int     *val_int;
  double  *val_float;
} Value;

// Se hace un typedef para poder usar node dentro de sí
typedef struct node Node;

/* 
* Estructura principal del programa. Se usa tanto para las tablas de símbolos como para los árboles sintácticos. Tiene un campo de subtipo que puede contener cualquier valor del enumerador hecho por bison para reconocer el lenguaje. Los campos de 'name' y 'value' se usan cuando se utiliza como elemento de tablas de símbolos, los campos 'op*' se usan cuando se utiliza como arbol sintáctico. El apuntador 'next' se usa para apuntar al siguiente elemento tanto en las tablas como en los árboles.
* 
* Ejemplo: Nodo de tabla: type = T_VARIABLE, subtype = INT, name = "varEjemplo", Value = val, next = addr ...
*          Nodo de árbol: type = T_SENTENCE, subtype = ASSIGN name = NULL, Value = NULL, next = addr, op1 = addr ... 
*/
struct node {
  int type;
  int subtype;
  char name[MAX_ID_LEN];
  Value *value;
  Node *next;
  Node *op1;
  Node *op2;
  Node *op3;
  Node *op4;
};

// 'externs' para usar funciones y variables proveidas por bison
extern int yylex();
extern char *yytext;
extern FILE *yyin;
extern int yylineno;
int yyerror(char const *s);

// Funciones para mandar errores y mensajes
void die(const char *s);
void die_line(const char *s);
void msg(const char *s);

// Funciones para tablas de símbolos
void addNodeToSymbolTable(Node **st, Node *n);
void printSymbolTable(Node *st);

// Funciones para el árbol sintáctico
void printTree(Node *node, int indent, int step);
int treeGetType(Node *n);

// Funciones para comprobar tipos de los nodos
int getCommonType(int t1, int t2);
int getCommonSubtype(int s1, int s2);

// Funciones para interpretar el arbol
void stmt_interpret(Node *node);
void stmt_readInt(Node *symb);
void stmt_readFloat(Node *symb);
void valuePrint(Value *val);
void stmt_if(Node *node);

// Funciones para interpretar las operaciones lógicas y aritméticas: +, -, *, /, <, >, >=, <=
int expression_eval(Node *node);
int lestnValues(Value *n1, Value *n2);
int gretnValues(Value *n1, Value *n2);
int equalsValues(Value *n1, Value *n2);
int lessoreqValues(Value *n1, Value *n2);
int gretoreqValues(Value *n1, Value *n2);
void valueAssign(Value **var, Value *expr);

// Funciones que regresan estructuras
Node* newNode(int type, int subtype, char *name, Value *value, Node *next, Node *op1, Node *op2, Node *op3, Node *op4);
Node* getSymbolTableNode(Node **st, const char *name);
Value* newValue(int type, int val_int, double val_float);
Value* expr_eval(Node *node);
Value* sumValues(Value *n1, Value *n2);
Value* resValues(Value *n1, Value *n2);
Value* divValues(Value *n1, Value *n2);
Value* mulValues(Value *n1, Value *n2);
