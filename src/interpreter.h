#pragma once
#include "ast.h"
#include "stack.h"

class BaseInterpreter {
protected:
    BaseVariableStack* vstack;
    vector<StatementNode*> statements;

    class interpreter_exception: exception {
    private:
        string str;
    public:
        explicit interpreter_exception(const string& msg): str("interpreter_exception: "+msg) {}
        
        const char* what() const noexcept override {
            return str.c_str();
        }
    };
public:
    BaseInterpreter(): vstack() {};
    BaseInterpreter(const vector<StatementNode*>& statements): vstack(), statements(statements) {}
    virtual ~BaseInterpreter() {
        delete vstack;
        vstack = nullptr;

        for (auto statement : statements) {
            delete statement;
            statement = nullptr;
        }
    }

    virtual void run() {
        for (auto statement : statements) {
            vector<BaseStatementArg*> args = {
                new StatementArg<BaseVariableStack*>(vstack)
            };
            BaseStatementResult* result = statement->run(args);

            if (result->fail()) {
               for (auto a : args) delete a;
               args.clear();
               delete result;
               result = nullptr;
                throw new interpreter_exception("StatementNode* failed to run.");
            }

            // Handle data types if you want.

            // clean up.
            for (auto a : args) delete a;
            args.clear();
            delete result;
            result = nullptr;
        }
    }
};