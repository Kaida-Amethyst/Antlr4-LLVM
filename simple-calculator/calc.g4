grammar calc;

program: prog;

prog: (with_stmt ':')? expr ';' ;

with_stmt : 'with' Var (',' Var )* ;

expr : mul ('+' | '-') mul
     | mul
     ;

mul  : primary ('*' | '/') primary
     | primary
     ;

primary : Var
        | Num
        | '(' expr ')'
        ;

Num : [0-9]+;

Var : [a-zA-Z]+;

WS  : [ \t\r\n]+ -> skip;


