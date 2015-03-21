%{
#include <stdio.h>
#include "y.tab.h"
%}

%state COMMENT

%%

<INITIAL>#\{			{BEGIN COMMENT;}
<COMMENT>.				
<COMMENT>#\}			{BEGIN INITIAL;}

var						return VAR;
fun						return FUN;
if						return IF;
else					return ELSE;
return					return RETURN;
true					{yylval=true;return BOOLVAL;}
false					{yylval=false;return BOOLVAL;}
=>						return ARROW;
[a-zA-Z_][a-zA-Z_]*		{yylval=strdup(yytext); return IDENT;}
(\-)?[1-9][0-9]*		{yylval=atoi(yytext);return INTVAL;}
[%=~|^^+*/<>&!]{1,3}	{yylval=strdup(yytext);return OPERATOR;}
\"([^\\\"]|\\.)*\"		{yylval=strdup(yytext);return STRINGVAL;} /*ダブルクオーテーション付きの文字列を返すことになる*/
#.*[\n|\r|\r\n]			/*コメント無視*/
[ \t\n\r(\r\n)]+		/*空白無視*/
.						{yylval=yytext[0];return yylval;}

%%

