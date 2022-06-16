/*
***REMOVED**** 'Lexer' de lenguaje basado en pascal
* La opción de yylineno es para llevar cuenta del número de linea
*/
%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parser.tab.h"
%}

DIGIT [0-9]
NUM_INT_P {DIGIT}+
NUM_FLT_P {DIGIT}+"."{DIGIT}+
NUM_INT_N "~"{DIGIT}+
NUM_FLT_N "~"{DIGIT}+"."{DIGIT}+
ID  [a-zA-Z_$]+{DIGIT}*[a-zA-Z_$0-9]*

%option yylineno
%option noinput nounput

%%

"program" { return PROGRM;  }
"begin"   { return BEGN;    }
"end"     { return END;     }
"<-"      { return ARROW;   }
"if"      { return IF;      }
"("       { return PRNTH1;  }
")"       { return PRNTH2;  }
"else"    { return ELSE;    }
"for"     { return FOR;     }
"while"   { return WHILE;   }
"repeat"  { return REPEAT;  }
"until"   { return UNTIL;   }
"step"    { return STEP;    }
"do"      { return DO;      }
"then"    { return THEN;    }
"var"     { return VAR;     }
"int"     { return INT;     }
"float"   { return FLOAT;   }
"read"    { return READ;    }
"print"   { return PRINT;   }
":"       { return COLON;   }
";"       { return SEMCLN;  }
"+"       { return SUM;     }
"-"       { return RES;     }
"*"       { return MUL;     }
"/"       { return DIV;     }
"<"       { return LESTN;   }
">"       { return GRETN;   }
"<="      { return LESSOREQ;}
">="      { return GRETOREQ;}
"="       { return EQUALS;  }
{ID}      { yylval.nombre = (char *) calloc(strlen(yytext) + 1, sizeof(char)); strcpy(yylval.nombre, yytext); return ID;         }
{NUM_INT_P} { yylval.val_int = atoi(yytext); return NUM_INT_P;    }
{NUM_FLT_P} { yylval.val_float = atof(yytext); return NUM_FLT_P;  }
{NUM_INT_N} { yylval.val_int = atoi(yytext) * -1; return NUM_INT_N;    }
{NUM_FLT_N} { yylval.val_float = atof(yytext) * -1; return NUM_FLT_N;  }
[ \n\t]   {                 }
.         { return ERR;     }

%%
