%{
  /*
***REMOVED***  * 'Parser' de lenguaje basado en pascal
  */
  #include "functions.c"
%}

%union {
  int val_int;
  float val_float;
  char *nombre;
  struct node *ptn;
}

%token BEGN PROGRM END ARROW THEN VAR INT FLOAT PRNTH1 PRNTH2 ELSE UNTIL STEP DO ERR COLON READ SUM RES MUL DIV LESTN GRETN EQUALS LESSOREQ GRETOREQ SEMCLN ASSIGN IF_ELSE IF WHILE FOR PRINT BEGN_END REPEAT
%token<val_int> NUM_INT
%token<val_float> NUM_FLT
%token<nombre> ID
%type<ptn> stmt expr factor expression opt_stmts stmt_lst term decl
%type<val_int> type
%start prog

/* Opciones de bison
* Se da mayor precedencia a THEN para eliminar los conflictos de 'shift/reduce'
*/
%precedence THEN
%precedence ELSE

%%

prog: PROGRM ID opt_decls BEGN opt_stmts END { treeRoot = $5; /* La raíz del árbol será el valor de retorno de opt_stmts*/ }
    ;

opt_decls: decl_lst 
         | 
         ;

decl_lst: decl SEMCLN decl_lst
        | decl
        ;

decl: VAR ID COLON type { addNodeToSymbolTable(&symbolRoot, newNode(T_VARIABLE, $4, $2, newValue($4, 0, 0), NULL, NULL, NULL, NULL, NULL));  /* Agregamos los nodos al apuntador de tabla de símbolos hecho al inicio del programa */ }
    ;

type: INT   { $$ = INT; }
    | FLOAT { $$ = FLOAT; }
    ;

stmt: ID ARROW expr {
                      // La condicional que aparece en esta regla junto con expr, term, factor y expression sirve para comprobar que los tipos de argumentos (variable y expresión en este caso) sean iguales
                      if (getCommonSubtype(getSymbolTableNode(&symbolRoot, $1) -> subtype, treeGetType($3)) != -1 ) { 
                        $$ = newNode(T_SENTENCE, ASSIGN, NULL, NULL, NULL, getSymbolTableNode(&symbolRoot, $1), $3, NULL, NULL); 
                      } else { 
                        die_line("Los 'subtypes' son distintos en ASSIGN"); 
                      }
                    }
    | IF expression THEN stmt { $$ = newNode(T_SENTENCE, IF, NULL, NULL, NULL, $2, $4, NULL, NULL); }
    | IF expression THEN stmt ELSE stmt { $$ = newNode(T_SENTENCE, IF_ELSE, NULL, NULL, NULL, $2, $4, $6, NULL); }
    | WHILE PRNTH1 expression PRNTH2 stmt { $$ = newNode(T_SENTENCE, WHILE, NULL, NULL, NULL, $3, $5, NULL, NULL); }
    | REPEAT stmt UNTIL PRNTH1 expression PRNTH2 { $$ = newNode(T_SENTENCE, REPEAT, NULL, NULL, NULL, $2, $5, NULL, NULL); }
    | FOR ID ARROW expr UNTIL expr STEP expr DO stmt { $$ = newNode(T_SENTENCE, FOR, $2, NULL, NULL, $4, $6, $8, $10); /* Se guarda el ID en el 'name' para su futura modificación*/ }
    | READ ID { $$ = newNode(T_SENTENCE, READ, NULL, NULL, NULL, getSymbolTableNode(&symbolRoot, $2), NULL, NULL, NULL); }
    | PRINT expr { $$ = newNode(T_SENTENCE, PRINT, NULL, NULL, NULL, $2, NULL, NULL, NULL); }
    | BEGN opt_stmts END { $$ = newNode(T_SENTENCE, BEGN_END, NULL, NULL, NULL, $2, NULL, NULL, NULL); }
    ;

opt_stmts: stmt_lst 
         |         { $$ = NULL; }
         ;


stmt_lst: stmt SEMCLN stmt_lst  { $1 -> next = $3; $$ = $1; }
        | stmt                  { $1 -> next = NULL; $$ = $1; }
        ;

expr: expr SUM term {
                      if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                        $$ = newNode(T_ARITHMETIC_OP, SUM, NULL, NULL, NULL, $1, $3, NULL, NULL);
                      } else {
                        die_line("Los 'subtypes' son distintos en expr");
                      }
                    }

    | expr RES term {
                      if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                        $$ = newNode(T_ARITHMETIC_OP, RES, NULL, NULL, NULL, $1, $3, NULL, NULL);
                      } else {
                        die_line("Los 'subtypes' son distintos en expr");
                      }
                    }
    | term ;

term: term MUL factor {
                        if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                          $$ = newNode(T_ARITHMETIC_OP, MUL, NULL, NULL, NULL, $1, $3, NULL, NULL);
                        } else {
                          die_line("Los 'subtypes' son distintos en term");
                        }
                      }

    | term DIV factor {
                        if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                          $$ = newNode(T_ARITHMETIC_OP, DIV, NULL, NULL, NULL, $1, $3, NULL, NULL);
                        } else {
                          die_line("Los 'subtypes' son distintos en term");
                        }
                      }
    | factor
    ;

factor: PRNTH1 expr PRNTH2 { $$ = $2; }
      | ID { $$ = getSymbolTableNode(&symbolRoot, $1); }
      | NUM_INT { $$ = newNode(T_CONSTANT, INT, NULL, newValue(INT, $1, 0), NULL, NULL, NULL, NULL, NULL); }
      | NUM_FLT { $$ = newNode(T_CONSTANT, FLOAT, NULL, newValue(FLOAT, 0, $1), NULL, NULL, NULL, NULL, NULL); }
      ;

expression: expr LESTN expr     {
                                  if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = newNode(T_LOGICAL_OP, LESTN, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }

          | expr GRETN expr     {
                                  if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = newNode(T_LOGICAL_OP, GRETN, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }

          | expr EQUALS expr    {
                                  if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = newNode(T_LOGICAL_OP, EQUALS, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }

          | expr LESSOREQ expr  {
                                  if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = newNode(T_LOGICAL_OP, LESSOREQ, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }

          | expr GRETOREQ expr  {
                                  if (getCommonSubtype(treeGetType($1), treeGetType($3)) != -1) {
                                    $$ = newNode(T_LOGICAL_OP, GRETOREQ, NULL, NULL, NULL, $1, $3, NULL, NULL);
                                  } else {
                                    die_line("Los 'subtypes' son distintos en expression");
                                  }
                                }
          ;
