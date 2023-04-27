grammar calc;

program: expr;

expr : mul '+' mul 
     | mul '-' mul
     | mul
     ;

mul  : pri '*' pri
     | pri '/' pri
     | pri
     ;

pri  : NUM 
     | '(' expr ')' 
     ;

NUM  : [0-9]+;

WS   : [ \t\r\n]+ -> skip;
