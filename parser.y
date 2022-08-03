%{
  /*
  * 
  * 
  * 'Parser' de lenguaje basado en pascal
  */
  #include "functions.c"
%}

%union {
  int val_int;
  float val_float;
  char *nombre;
  struct node *ptn;
}

%token BEGN PROGRM END FUN COMMA ARROW THEN VAR INT FLOAT PRNTH1 PRNTH2 ELSE UNTIL STEP DO ERR COLON READ SUM RES MUL DIV LESTN GRETN EQUALS LESSOREQ GRETOREQ SEMCLN ASSIGN IF_ELSE IF WHILE FOR PRINT BEGN_END RETURN FUN_CALL REPEAT
%token<val_int> NUM_INT
%token<val_float> NUM_FLT
%token<nombre> ID
%type<ptn> stmt expr expr_tmp factor expression opt_stmts stmt_lst term decl opt_exprs expr_lst
%type<val_int> type
%start prog

/* 
* Opciones de bison
* Se da mayor precedencia a THEN para eliminar los conflictos de 'shift/reduce'
*/
%precedence THEN
%precedence ELSE

%%

prog: PROGRM ID opt_decls { symbolRoot = currentSymbolTable; currentSymbolTable = NULL; } opt_fun_decls BEGN opt_stmts END { treeRoot = $7; /* La raíz del árbol será el valor de retorno de opt_stmts*/ }
    ;

opt_decls: decl_lst 
         | 
         ;

decl_lst: decl SEMCLN decl_lst
        | decl
        ;

decl: VAR ID COLON type { tableAddNode(&currentSymbolTable, nodeNew(T_VARIABLE, $4, $2, valueNew($4, 0, 0), NULL, NULL, NULL, NULL, NULL));  /* Agregamos los nodos al apuntador actual */ }
    ;

type: INT   { $$ = INT; }
    | FLOAT { $$ = FLOAT; }
    ;

opt_fun_decls: fun_decls
             |
             ;

fun_decls: fun_decls fun_decl
         | fun_decl
         ;

fun_decl: FUN ID PRNTH1 oparams PRNTH2 COLON type opt_decls 
        { 
          /* 
          *  Agregamos la función a la tabla de símbolos, después la poblaremos con el cuerpo de la función.
          *  Nos aseguramos de que el número de argumentos sea el correcto.
          */
          if (funcRoot != NULL && tableGet(&funcRoot, $2) != NULL) {
            if (tableGet(&funcRoot, $2) -> op1 == NULL) { 
              tableGet(&funcRoot, $2) -> op1 = currentSymbolTable;
              currentFunc = tableGet(&funcRoot, $2);
            } else {
              die_line("Múltiple definición de función");
            }
          } else {
            tableAddNode(&funcRoot, nodeNew(T_FUNCTION, $7, $2, valueNew(INT, paramCounter, 0), NULL, currentSymbolTable, currentParamTable, NULL, NULL));
            currentFunc = tableGet(&funcRoot, $2);
            paramCounter = 0;
          }
        }
BEGN opt_stmts END
        {
          tableGet(&funcRoot, $2) -> op3 = $11;
          currentSymbolTable = NULL;
          currentParamTable = NULL;
        }

        | FUN ID PRNTH1 oparams PRNTH2 COLON type SEMCLN
        {
          if (funcRoot != NULL && tableGet(&funcRoot, $2) != NULL) {
            die_line("Múltiple definición de función");
          } else {
            tableAddNode(&funcRoot, nodeNew(T_FUNCTION, $7, $2, valueNew(INT, paramCounter, 0), NULL, NULL, currentParamTable, NULL, NULL));
          }
          paramCounter = 0;
          currentParamTable = NULL;
        }
        ;

oparams: params
       |
       ;

params: param COMMA params
      | param
      ;

param: ID COLON type { tableAddNode(&currentParamTable, nodeNew(T_VARIABLE, $3, $1, valueNew($3, 0, 0), NULL, NULL, NULL, NULL, NULL)); paramCounter++; /* Por cada parámetro sumamos a paramCounter, para tener en cuenta cuantos argumentos recibe la función */}

stmt: ID ARROW expr {
                      // La condicional que aparece en esta regla junto con expr, term, factor y expression sirve para comprobar que los tipos de argumentos (variable y expresión en este caso) sean iguales. También buscan el nombre de la variable donde se guardará el valor resultante. Primeramente en la lista de parámetros, después en las variables declaradas localmente y después en la lista de variables globales.
                      $$ = assignCheck($1, $3);
                    }
    | IF expression THEN stmt { $$ = nodeNew(T_SENTENCE, IF, NULL, NULL, NULL, $2, $4, NULL, NULL); }
    | IF expression THEN stmt ELSE stmt { $$ = nodeNew(T_SENTENCE, IF_ELSE, NULL, NULL, NULL, $2, $4, $6, NULL); }
    | WHILE PRNTH1 expression PRNTH2 stmt { $$ = nodeNew(T_SENTENCE, WHILE, NULL, NULL, NULL, $3, $5, NULL, NULL); }
    | REPEAT stmt UNTIL PRNTH1 expression PRNTH2 { $$ = nodeNew(T_SENTENCE, REPEAT, NULL, NULL, NULL, $2, $5, NULL, NULL); }
    | FOR ID ARROW expr UNTIL expr STEP expr DO stmt { $$ = nodeNew(T_SENTENCE, FOR, $2, NULL, NULL, $4, $6, $8, $10); /* Se guarda el ID en el 'name' para su futura modificación*/ }
    | READ ID { 
                if (currentFunc != NULL && currentFunc -> op2 != NULL && tableGet(&currentFunc -> op2, $2) != NULL) {
                  $$ = nodeNew(T_SENTENCE, READ, NULL, NULL, NULL, tableGet(&currentFunc -> op2, $2), NULL, NULL, NULL);
                } else if (currentFunc != NULL && currentFunc -> op1 != NULL && tableGet(&currentFunc -> op1, $2)) {
                  $$ = nodeNew(T_SENTENCE, READ, NULL, NULL, NULL, tableGet(&currentFunc -> op1, $2), NULL, NULL, NULL);
                } else if (symbolRoot != NULL && tableGet(&symbolRoot, $2) != NULL) {
                  $$ = nodeNew(T_SENTENCE, READ, NULL, NULL, NULL, tableGet(&symbolRoot, $2), NULL, NULL, NULL);
                } else {
                  die_line("ID no encontrada en stmt");
                }
              }
    | PRINT expr { $$ = nodeNew(T_SENTENCE, PRINT, NULL, NULL, NULL, $2, NULL, NULL, NULL); }
    | RETURN expr { $$ = nodeNew(T_SENTENCE, RETURN, NULL, NULL, NULL, $2, NULL, NULL, NULL); }
    | BEGN opt_stmts END { $$ = nodeNew(T_SENTENCE, BEGN_END, NULL, NULL, NULL, $2, NULL, NULL, NULL); }
    ;

opt_stmts: stmt_lst 
         |         { $$ = NULL; }
         ;


stmt_lst: stmt SEMCLN stmt_lst  { $1 -> next = $3; $$ = $1; }
        | stmt                  { $1 -> next = NULL; $$ = $1; }
        ;

expr: expr SUM term {
                      if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                        $$ = nodeNew(T_ARITHMETIC_OP, SUM, NULL, NULL, NULL, $1, $3, NULL, NULL);
                      } else {
                        die_line("Los 'subtypes' son distintos en expr");
                      }
                    }

    | expr RES term {
                      if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                        $$ = nodeNew(T_ARITHMETIC_OP, RES, NULL, NULL, NULL, $1, $3, NULL, NULL);
                      } else {
                        die_line("Los 'subtypes' son distintos en expr");
                      }
                    }
    | term ;

term: term MUL factor {
                        if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                          $$ = nodeNew(T_ARITHMETIC_OP, MUL, NULL, NULL, NULL, $1, $3, NULL, NULL);
                        } else {
                          die_line("Los 'subtypes' son distintos en term");
                        }
                      }

    | term DIV factor {
                        if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                          $$ = nodeNew(T_ARITHMETIC_OP, DIV, NULL, NULL, NULL, $1, $3, NULL, NULL);
                        } else {
                          die_line("Los 'subtypes' son distintos en term");
                        }
                      }
    | factor
    ;

factor: PRNTH1 expr PRNTH2 { $$ = $2; }
      | ID { 
              // La misma lógica que en statement
              if (currentFunc != NULL && currentFunc -> op2 != NULL && tableGet(&currentFunc -> op2, $1) != NULL) {
                $$ = tableGet(&currentFunc -> op2, $1);   
              } else if (currentFunc != NULL && currentFunc -> op1 != NULL && tableGet(&currentFunc -> op1, $1)) {
                $$ = tableGet(&currentFunc -> op1, $1);
              } else if (symbolRoot != NULL && tableGet(&symbolRoot, $1) != NULL) {
                $$ = tableGet(&symbolRoot, $1);
              } else {
                die_line("ID no encontrada en factor");
              }
           }
      | NUM_INT { $$ = nodeNew(T_CONSTANT, INT, NULL, valueNew(INT, $1, 0), NULL, NULL, NULL, NULL, NULL); }
      | NUM_FLT { $$ = nodeNew(T_CONSTANT, FLOAT, NULL, valueNew(FLOAT, 0, $1), NULL, NULL, NULL, NULL, NULL); }
      | ID PRNTH1 opt_exprs PRNTH2 { 
                                      // Al llamar la función comprobamos que los argumentos sean validos
                                      if (funcRoot != NULL && tableGet(&funcRoot, $1) != NULL) {
                                        if (functionCheckValidArgs(tableGet(&funcRoot, $1) -> op2, *tableGet(&funcRoot, $1) -> value -> val_int, currentParamTable, argCounter)) {
                                          $$ = nodeNew(T_SENTENCE, FUN_CALL, $1, valueNew(INT, argCounter, 0), NULL, currentParamTable, NULL, NULL, NULL); 
                                          argCounter = 0;
                                          currentParamTable = NULL;
                                        } else {
                                          die_line("Error en los argumentos pasados a la función");
                                        }
                                      } else {
                                        die_line("Función no encontrada");
                                      }
                                   }
      ;

opt_exprs: expr_lst
         |          { $$ = NULL; }
         ;

expr_lst: expr_tmp COMMA expr_lst
        | expr_tmp
        ;

expr_tmp: expr {
                  // Al hacer la lista de argumentos para llamar a una función hacemos nodos nuevos, esto para evitar un error donde un nodo apunta a sí mismo. 
                  if ($1 -> type != T_VARIABLE) {
                    tableAddNode(&currentParamTable, $1);
                  } else {
                    Node *tmp = malloc(sizeof(Node));
                    memcpy(tmp, $1, sizeof(Node));
                    tmp -> next = NULL;
                    tableAddNode(&currentParamTable, tmp);
                    tmp = NULL;
                  }
                  argCounter++;
               }

expression: expr LESTN expr     {
                                  if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = nodeNew(T_LOGICAL_OP, LESTN, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }

          | expr GRETN expr     {
                                  if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = nodeNew(T_LOGICAL_OP, GRETN, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }

          | expr EQUALS expr    {
                                  if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = nodeNew(T_LOGICAL_OP, EQUALS, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }

          | expr LESSOREQ expr  {
                                  if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = nodeNew(T_LOGICAL_OP, LESSOREQ, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }

          | expr GRETOREQ expr  {
                                  if (subtypeGetCommon(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = nodeNew(T_LOGICAL_OP, GRETOREQ, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }
          ;
