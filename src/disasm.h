#pragma once
#include "opcode.h"
#include "ast.h"
#include "register.h"

#include "utils.h"

class BaseAssembler {
protected:
    vector<StatementNode*> statements;

    // TODO: Implement it.
    virtual vector<BaseOpcode*> ast2opc() = 0;
public:
    BaseAssembler() = default;
    BaseAssembler(vector<StatementNode*> statements): statements(statements) {}
    virtual ~BaseAssembler() = default;
};

template<typename int_t>
class Assembler: public BaseAssembler {
public:
    using BaseAssembler::BaseAssembler;

    virtual vector<int_t> assemble() {
        vector<int_t> result;

        vector<BaseOpcode*> opcs = ast2opc();
        for (auto opc: opcs) {
            if (Opcode<int_t>* popc = dynamic_cast<Opcode<int_t>*>(opc))
                result.push_back(popc->getRaw());
        }

        return result;
    }
};

class BaseDisassembler {
protected:
    // TODO: Implement it.
    virtual vector<BaseOpcode*> raw2opc() = 0;
public:
    BaseDisassembler() = default;
    virtual ~BaseDisassembler() = default;

    // TODO: Implement it.
    virtual vector<BaseOpcode*> disassemble() = 0;
};

template<typename int_t>
class Disassembler: public BaseDisassembler {
protected:
    vector<int_t> raw;
public:
    using BaseDisassembler::BaseDisassembler;
    Disassembler(vector<int_t> raw): raw(raw) {}

    virtual vector<BaseOpcode*> disassemble() override {
        vector<BaseOpcode*> result = raw2opc();
        return result;
    }
};