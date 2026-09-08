#pragma once
#include "ast.h"
#include "stack.h"
#include "opcode.h"
#include "instruction.h"

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

class BaseOpcodeInterpreter {
protected:
    BaseVariableStack* vstack;
    vector<BaseOpcode*> opcodes;
    vector<BaseInstructionConfig*> instsConfig;

    // TODO: Implement it.
    virtual bool isBothTypeEqual(const BaseOpcodeArgType* type0, const BaseInstructionArgType* type1) = 0;
    // TODO: Implement it.
    virtual BaseParsedInstructionArg* opcarg2instarg(BaseParsedOpcodeArg* arg) = 0;

    virtual vector<BaseInstruction*> opc2inst(const vector<BaseInstructionConfig*>& config) {
        vector<BaseInstruction*> insts;

        size_t i = 0;
        for (auto opc: opcodes) {
            BaseInstruction* inst = new BaseInstruction();
            
            if (config[i]->getArgConfig()->getArgOrder().size() == opc->getParsedOpc()->getConfig()->getArgConfig()->getArgOrder().size()) {
                size_t k = 0;
                for (auto argt: opc->getParsedOpc()->getConfig()->getArgConfig()->getArgOrder()) {
                    if (isBothTypeEqual(argt, config[i]->getArgConfig()->getArgOrder()[k])) {
                        inst->getParsedInst()->getArgs().push_back(opcarg2instarg(opc->getParsedOpc()->getArgs()[k]));
                    } else {
                        delete inst;
                        inst = nullptr;

                        throw new opcode_interpreter_exception("InstructionConfig::InstructionArgConfig::argOrder::InstructionArgType is not equal to OpcodeConfig::OpcodeArgConfig::argOrder::OpcodeArgType!");
                    }
                    k++;
                }

                inst->getParsedInst()->getConfig()->getLogic() = config[i]->getLogic();
                inst->getParsedInst()->getConfig()->getReturnType() = config[i]->getReturnType();
                inst->getParsedInst()->getInstructionCode() = opc->getParsedOpc()->getInstructionCode();

                insts.push_back(inst);
            } else {
                delete inst;
                inst = nullptr;

                throw new opcode_interpreter_exception("InstructionConfig::InstructionArgConfig::argOrder's size is not equal to OpcodeConfig::OpcodeArgConfig::argOrder!");
            }

            i++;
        }

        return insts;
    }

    class opcode_interpreter_exception: exception {
    private:
        string str;
    public:
        explicit opcode_interpreter_exception(const string& msg): str("opcode_interpreter_exception: "+msg) {}
        
        const char* what() const noexcept override {
            return str.c_str();
        }
    };
public:
    BaseOpcodeInterpreter(): vstack() {};
    BaseOpcodeInterpreter(const vector<BaseOpcode*>& opcodes, const vector<BaseInstructionConfig*>& instsConfig): vstack(), opcodes(opcodes), instsConfig(instsConfig) {}
    virtual ~BaseOpcodeInterpreter() {
        delete vstack;
        vstack = nullptr;

        for (auto opcode : opcodes) {
            delete opcode;
            opcode = nullptr;
        }
    }

    // For simple and registered interactions.
    virtual void run() {
        vector<BaseInstruction*> insts = opc2inst(instsConfig);
        for (auto inst : insts) {
            vector<BaseInstructionArg*> args = {
                new InstructionArg<BaseVariableStack*>(vstack)
            };
            BaseInstructionResult* result = inst->getParsedInst()->run(args);

            if (result->fail()) {
               for (auto a : args) delete a;
               args.clear();
               delete result;
               result = nullptr;
                throw new opcode_interpreter_exception("BaseInstruction* failed to run.");
            }

            // Handle data types if you want.

            // clean up.
            for (auto a : args) delete a;
            args.clear();
            delete result;
            result = nullptr;
        }
    }

    // For direct interactions.
    virtual void run(const vector<BaseInstruction*>& insts) {
        for (auto inst : insts) {
            vector<BaseInstructionArg*> args = {
                new InstructionArg<BaseVariableStack*>(vstack)
            };
            BaseInstructionResult* result = inst->getParsedInst()->run(args);

            if (result->fail()) {
                for (auto a : args) delete a;
                args.clear();
                delete result;
                result = nullptr;
                throw new opcode_interpreter_exception("BaseInstruction* failed to run.");
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