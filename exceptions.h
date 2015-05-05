#pragma once

class RuntimeError{
};


class LexerException{
};

class NoMatchRule : public LexerException{
};

class ParserException{
};

class SyntaxError : public ParserException{
};
