#pragma once
#include "ast.h"
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <cstdint>

class StatementNode;

struct BaseSemanticType {
protected:
    int16_t stored_type;

    inline static const BaseSemanticType* decdef(int16_t type) {
        return new BaseSemanticType(type);
    }
public:
    inline static const BaseSemanticType* ERROR = decdef(-2);
    inline static const BaseSemanticType* NONE = decdef(-1);
    inline static const BaseSemanticType* VOID = decdef(0);

    BaseSemanticType(int16_t type): stored_type(type) {}
    virtual ~BaseSemanticType() = default;

    int16_t getStoredType() const { return stored_type; }
};

struct BaseSemanticTypeInfo {
protected:
    int16_t stored_type;

    const BaseSemanticType* curType;
    vector<BaseSemanticType*> paramTypes; // functions
    unordered_map<string, const BaseSemanticTypeInfo*> members; // class/objects
    bool isCallable;
    bool isLValue;
    
    inline static const BaseSemanticTypeInfo* decdef
        (int16_t type, const BaseSemanticType* curType, vector<BaseSemanticType*> paramTypes,
        unordered_map<string, const BaseSemanticTypeInfo*> members,
        bool isCallable, bool isLValue)
    {

        return new BaseSemanticTypeInfo(type, curType, paramTypes, members, isCallable, isLValue);
    }
public:
    BaseSemanticTypeInfo(int16_t type, const BaseSemanticType* curType, vector<BaseSemanticType*> paramTypes,
        unordered_map<string, const BaseSemanticTypeInfo*> members,
        bool isCallable, bool isLValue): stored_type(type), curType(curType), paramTypes(paramTypes), members(members), isCallable(isCallable), isLValue(isLValue) {}
    virtual ~BaseSemanticTypeInfo() = default;

    int16_t getStoredType() const { return stored_type; }
};

template<typename T>
class BaseASTVisitor {
public:
    BaseASTVisitor() = default;
    virtual ~BaseASTVisitor() = default;
protected:
    // Default visit method, for more derived StatementNode types, you can duplicate this function.
    // 
    // TODO: Implement it as you like.
    virtual T visit(StatementNode* node) = 0;

    friend class StatementNode;
};

class BaseSemanticAnalyzer: public BaseASTVisitor<const BaseSemanticType*> {
protected:
    vector<StatementNode*> statements;

    class semantic_analyzer_exception: exception {
    private:
        string str;
    public:
        explicit semantic_analyzer_exception(const string& msg): str("semantic_analyzer_exception: "+msg) {}
        
        const char* what() const noexcept override {
            return str.c_str();
        }
    };

    class semantic_error {
    private:
        string str;
    public:
        semantic_error(const string& msg): str("\033[1;31msemantic_error:\033[0m "+msg) {}

        void show() const noexcept {
            printf("%s\n", str.c_str());
        }
    };
    vector<semantic_error> errors;

    virtual void addError(const string& msg) {
        errors.push_back(semantic_error(msg));
    }

    virtual bool hasErrors() const {
        return !errors.empty();
    }

    vector<map<string, const BaseSemanticTypeInfo*>> scope;
public:
    BaseSemanticAnalyzer(): BaseASTVisitor(), scope({map<string, const BaseSemanticTypeInfo*>()}) {}
    BaseSemanticAnalyzer(vector<StatementNode*> statements): statements(statements), scope({map<string, const BaseSemanticTypeInfo*>()}) {}
    virtual ~BaseSemanticAnalyzer() {
        for (auto& m: scope) {
            for (auto [k, v] : m) {
                v = nullptr;
            }
            m.clear();
        }
        scope.clear();
    }

    virtual void analyze() {
        for (auto node: statements) {
            const BaseSemanticType* result = node->accept(this);

            if (result == BaseSemanticType::ERROR) {
                addError("failed to parse node.");
            }
        }

        if (!hasErrors()) return;

        for (auto& error : errors) {
            error.show();
        }

        throw new semantic_analyzer_exception("failed to analyze statements!");
    }

    virtual void pushScope() {
        scope.push_back(map<string, const BaseSemanticTypeInfo*>());
    }

    virtual void popScope() {
        if (!scope.empty())
            scope.pop_back();
    }

    virtual const BaseSemanticTypeInfo* lookupVariableType(const string& name) {
        for (auto& m : scope) {
            for (auto [k, v] : m) {
                if (k == name)
                    return v;
            }
        }
        return nullptr;
    }
protected:
    virtual const BaseSemanticType* visit(StatementNode* node) override {
        return BaseSemanticType::VOID;
    }
};