/*
***REMOVED**** 'Lexer' de lenguaje basado en pascal
* La opción de yylineno es para llevar cuenta del número de linea
* La opción de noinput y nounput es para reducir errores
*/
%{
  #include "parser.tab.h"
  char** tmp = NULL;
%}

DIGIT [0-9]
NUM_INT {DIGIT}+
NUM_FLT {DIGIT}+"."{DIGIT}+
ID  [a-zA-Z_$]+{DIGIT}*[a-zA-Z_$0-9]*

%option yylineno
%option noinput nounput

%%

"program"   { return PROGRM;  }
"begin"     { return BEGN;    }
"end"       { return END;     }
"<-"        { return ARROW;   }
"if"        { return IF;      }
"("         { return PRNTH1;  }
")"         { return PRNTH2;  }
"else"      { return ELSE;    }
"for"       { return FOR;     }
"while"     { return WHILE;   }
"repeat"    { return REPEAT;  }
"until"     { return UNTIL;   }
"step"      { return STEP;    }
"do"        { return DO;      }
"then"      { return THEN;    }
"var"       { return VAR;     }
"int"       { return INT;     }
"float"     { return FLOAT;   }
"read"      { return READ;    }
"print"     { return PRINT;   }
":"         { return COLON;   }
";"         { return SEMCLN;  }
"+"         { return SUM;     }
"-"         { return RES;     }
"*"         { return MUL;     }
"/"         { return DIV;     }
"<"         { return LESTN;   }
">"         { return GRETN;   }
"<="        { return LESSOREQ;}
">="        { return GRETOREQ;}
"="         { return EQUALS;  }
{ID}        {
              yylval.nombre = (char *) calloc(strlen(yytext) + 1, sizeof(char));
              strcpy(yylval.nombre, yytext);
              return ID;
            }
{NUM_INT}   {
              yylval.val_int = strtoimax(yytext, tmp, 10);
              return NUM_INT;
            }
{NUM_FLT}   {
              yylval.val_float = strtod(yytext, tmp);
              return NUM_FLT;
            }
~{NUM_INT}  {
              yylval.val_int = strtoimax(yytext+1, tmp, 10) * -1; // Le sumamos 1 para que no lea el símbolo '~' y se detenga
              return NUM_INT;
            }
~{NUM_FLT}  {
              yylval.val_float = strtod(yytext+1, tmp) * -1; // Lo mismo que en el num_int negativo
              return NUM_FLT;
            }
[ \n\t]     {                 }
.           { return ERR;     }

%%
