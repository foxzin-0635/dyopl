#pragma once
#include <cstdio>
#include <string>
#include <cstdint>
#include <regex>
#include <vector>
#include <cstddef>

using namespace std;

// Base Enum for all token types.
struct BaseTokenType {
protected:
    int16_t stored_type;
    
    inline static const BaseTokenType* decdef(int16_t type) {
        return new BaseTokenType(type);
    }
public:
    inline static const BaseTokenType* EOF_ = decdef(-1);
    inline static const BaseTokenType* WHITESPACE = decdef(0);
    inline static const BaseTokenType* IDENTIFIER = decdef(1);
    
    BaseTokenType(int16_t type): stored_type(type) {}
    virtual ~BaseTokenType() = default;

    int16_t getStoredType() const { return stored_type; }
};

// Base config for all tokens.
struct BaseTokenConfig {
protected:
    const BaseTokenType* type;
    string regex;
public:
    BaseTokenConfig(): type(BaseTokenType::EOF_), regex("") {}
    BaseTokenConfig(const BaseTokenType* type, const string& regex): type(type), regex(regex) {}
    virtual ~BaseTokenConfig() {
        type = nullptr; // prevent freeing the const one
    }
    
    virtual const BaseTokenType* getType() { return type; }
    virtual const BaseTokenType* getType() const { return type; }
    
    virtual bool matches(smatch& match, const string& str) {
        return regex_match(str, match, std::regex(regex));
    }
    virtual bool finds(smatch& match, const string& str) {
        return regex_search(str, match, std::regex("^"+regex));
    }
    
    virtual bool matches(smatch& match, const string& str) const {
        return regex_match(str, match, std::regex(regex));
    }
    virtual bool finds(smatch& match, const string& str) const {
        return regex_search(str, match, std::regex("^"+regex));
    }
};

struct BaseToken {
protected:
    const BaseTokenType* type;
    string lexeme;
    vector<string> extraMatches; // it's a new thing btw.
    size_t start;
    size_t end;
public:
    BaseToken(): type(BaseTokenType::EOF_), lexeme(""), start(0), end(0) {}
    BaseToken(const BaseTokenType* type, const string& lexeme, 
              vector<string> extraMatches, 
              size_t start, size_t end):
                  type(type), lexeme(lexeme),
                  extraMatches(extraMatches),
                  start(start), end(end) {}
                  
    virtual const BaseTokenType* getType() { return type; }
    virtual string& getLexeme() { return lexeme; }
    virtual vector<string> getExtraMatches() { return extraMatches; }
    virtual size_t getStart() { return start; }
    virtual size_t getEnd() { return end; }
    
    virtual const BaseTokenType* getType() const { return type; }
    virtual const string& getLexeme() const { return lexeme; }
    virtual vector<string> getExtraMatches() const { return extraMatches; }
    virtual size_t getStart() const { return start; }
    virtual size_t getEnd() const { return end; }
};

// The Base Lexer!
class BaseLexer {
protected:
    vector<BaseTokenConfig*> tconf;
    bool ignoreWhitespaces = true; // for frameworks that requires a second
                                   // text frame for highlighting.
public:
    BaseLexer() {
        tconf = {
            new BaseTokenConfig(BaseTokenType::WHITESPACE, "\\s+"),
            // Tokens must go between those two.
            new BaseTokenConfig(BaseTokenType::IDENTIFIER, "[a-zA-Z_][a-zA-Z0-9_]*")
        };
    }
    BaseLexer(const vector<BaseTokenConfig*>& tconf, bool ignoreWhitespaces): tconf(tconf), ignoreWhitespaces(ignoreWhitespaces) {}
    virtual ~BaseLexer() {
        for (auto c : tconf) { delete c; }
        tconf.clear();
    }

    virtual void setIgnoreWhitespaces(bool value) { ignoreWhitespaces = value; }
    
    virtual vector<BaseToken*> tokenize(const string& source) {
        vector<BaseToken*> tokens;
        size_t pos = 0;
        
        while (pos < source.length()) {
            bool matched = false;
            
            for (auto& conf : tconf) {
                string strip = source.substr(pos);
                smatch match;
                
                if (conf->finds(match, strip)) {
                    string lexeme = match[0].str();
                    size_t start = pos;
                    size_t end = start + lexeme.length();
                    vector<string> extraMatches;
                    
                    if (match.size() > 1) {
                        for (size_t i = 1; i < match.size(); i++) {
                            extraMatches.push_back(match[i].str());
                        }
                    }
                    
                    if (ignoreWhitespaces) {
                        if (conf->getType() != BaseTokenType::WHITESPACE)
                            tokens.push_back(new BaseToken(conf->getType(), lexeme, extraMatches, start, end));
                    }
                    else
                        tokens.push_back(new BaseToken(conf->getType(), lexeme, extraMatches, start, end));
                    
                    pos = end;
                    matched = true;
                    break;
                }
            }
            
            if (!matched)
                pos++; // you can change this part
                       // to handle unknown tokens.
        }
        
        return tokens;
    }
};