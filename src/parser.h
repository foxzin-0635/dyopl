#pragma once
#include "ast.h"
#include "lexer.h"

class BaseParser {
protected:
    vector<BaseToken*> tokens;
    size_t curTokenPos;
public:
    BaseParser(): curTokenPos(0) {};
    BaseParser(vector<BaseToken*>& tokens): tokens(tokens), curTokenPos(0) {}
    virtual ~BaseParser() = default;
    
    virtual vector<StatementNode> parse() {
        // TODO: Implement this code in your parsers.
        /*vector<StatementNode> nodes;
        
        while (!check(BaseTokenType::EOF_)) {
            nodes.push_back(parseStatement());
        }
        
        return nodes;*/
        throw new parser_exception("not implemented.");
    }
protected:
    class parser_exception: exception {
    private:
        string str;
    public:
        explicit parser_exception(const string& msg): str("parser_exception: "+msg) {}
        
        const char* what() const noexcept override {
            return str.c_str();
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
    
    virtual bool check(const BaseTokenType* type) {
        return tokens[curTokenPos]->getType() == type;
    }
    
    virtual bool checkByLexeme(const string& lexeme, bool icase) {
        return regex_match(tokens[curTokenPos]->getLexeme(), ((icase) ? regex(lexeme, regex_constants::icase) : regex(lexeme)));
    }
    
    virtual BaseToken* consume(const BaseTokenType* type, const string& errorMessage) {
        if (check(type))
            return advance();
        
        throw new parser_exception("Token does not have the expected type.");
    }
    
    virtual BaseToken* consumeByLexeme(const string& lexeme, bool icase, const string& errorMessage) {
        if (checkByLexeme(lexeme, icase))
            return advance();
        
        throw new parser_exception("Token does not have the expected lexeme.");
    }
    
    virtual StatementNode parseStatement() {
        throw new parser_exception("not implemented.");
    }
    
    virtual BaseExpressionNode* parseExpression() {
        throw new parser_exception("not implemented.");
    }
};