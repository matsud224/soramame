%{
#include <stdio.h>
#include "parser.cc"

%}

%option c++
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
\"([^\\\"]|\\.)*\"		{yylval=strdup(yytext);return STRINGVAL;} /*�_�u���N�I�[�e�[�V�����t���̕������Ԃ����ƂɂȂ�*/
#.*[\n|\r|\r\n]			/*�R�����g����*/
[ \t\n\r(\r\n)]+		/*�󔒖���*/
.						{yylval=yytext[0];return yylval;}

%%

int main()
{
  FlexLexer* lexer = new yyFlexLexer();
  std::ifstream in("hoge.input" std::ios::binary);
  lexer->switch_stream(&in);
  lexer->parse();
  return 0;
}