#pragma once

class RuntimeError{
};


class LexerException{
};

class NoMatchRule : public LexerException{
};

class OnigurumaException : public LexerException{
public:
	unsigned char *Message;
	OnigurumaException(unsigned char *message):Message(message){}
};


class ParserException{
};

class SyntaxError : public ParserException{
};
