#pragma once
#include "lexer.h"

class BaseParser {
protected:
    vector<BaseToken*> tokens;
    size_t curTokenPos;
public:
    BaseParser(): curTokenPos(0) {};
    BaseParser(vector<BaseToken*>& tokens): tokens(tokens), curTokenPos(0) {}
    virtual ~BaseParser() = default;
    
protected:
    class parser_exception {
    public:
        parser_exception(const char* msg, int errc) {
            fprintf(stderr, "parser_exception: %s", msg);
            exit(errc);
        }
    };
    
    virtual BaseToken* peek() {
        if (curTokenPos >= tokens.size())
            return new BaseToken();
        
        return tokens[curTokenPos];
    }
    
    virtual BaseToken* peekNext() {
        if (curTokenPos + 1 >= tokens.size())
            return new BaseToken();
        
        return tokens[curTokenPos+1];
    }
    
    virtual BaseToken* peekPrev() {
        if (curTokenPos - 1 < 0)
            return new BaseToken();
        
        return tokens[curTokenPos-1];
    }
    
    virtual BaseToken* peekWithOffset(size_t offset) {
        if (curTokenPos + offset >= tokens.size())
            return new BaseToken();
        if (curTokenPos + offset < 0)
            return new BaseToken();
        
        return tokens[curTokenPos+offset];
    }
    
    virtual BaseToken* advance() {
        if (curTokenPos + 1 >= tokens.size())
            return new BaseToken();
        
        return tokens[curTokenPos++];
    }
    
    virtual BaseToken* advanceNTimes(size_t n) {
        if (curTokenPos + n >= tokens.size())
            return new BaseToken();
        if (curTokenPos + n < 0)
            return new BaseToken();
        
        curTokenPos += n;
        return tokens[curTokenPos];
    }
    
    virtual bool check(BaseTokenType* type) {
        return tokens[curTokenPos]->getType() == type;
    }
    
    virtual bool checkByLexeme(const string& lexeme, bool icase) {
        return regex_match(tokens[curTokenPos]->getLexeme(), ((icase) ? regex(lexeme, regex_constants::icase) : regex(lexeme)));
    }
    
    virtual BaseToken* consume(BaseTokenType* type, const string& errorMessage) {
        if (check(type))
            return advance();
        
        throw new parser_exception("Token does not have the expected type.", 1);
    }
    
    virtual BaseToken* consumeByLexeme(const string& lexeme, bool icase, const string& errorMessage) {
        if (checkByLexeme(lexeme, icase))
            return advance();
        
        throw new parser_exception("Token does not have the expected lexeme.", 1);
    }
    
    virtual
};