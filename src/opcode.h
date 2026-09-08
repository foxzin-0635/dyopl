#pragma once
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

struct BaseOpcodeArgType {
protected:
    int16_t stored_type;
    
    inline static const BaseOpcodeArgType* decdef(int16_t type) {
        return new BaseOpcodeArgType(type);
    }
public:
    inline static const BaseOpcodeArgType* NONE = decdef(0);

    BaseOpcodeArgType(int16_t type): stored_type(type) {}
    virtual ~BaseOpcodeArgType() = default;

    int16_t getStoredType() const { return stored_type; }
};

struct BaseOpcodeArgConfig {
protected:
    vector<const BaseOpcodeArgType*> argTypeOrder;
public:
    BaseOpcodeArgConfig() = default;
    BaseOpcodeArgConfig(vector<const BaseOpcodeArgType*> argTypeOrder): argTypeOrder(argTypeOrder) {}
    virtual ~BaseOpcodeArgConfig() {
        for (auto at: argTypeOrder) {
            delete at;
            at = nullptr;
        }

        argTypeOrder.clear();
    }

    virtual vector<const BaseOpcodeArgType*> getArgOrder() { return argTypeOrder; }
    virtual const BaseOpcodeArgType* argTypeAt(size_t offset) {
        if (offset >= argTypeOrder.size()) {
            return BaseOpcodeArgType::NONE;
        }

        return argTypeOrder.at(offset);
    }
};

struct BaseOpcodeConfig {
protected:
    string mnemonic;
    BaseOpcodeArgConfig* argConfig;
public:
    BaseOpcodeConfig(): mnemonic(""), argConfig() {}
    BaseOpcodeConfig(const string& mnemonic, BaseOpcodeArgConfig*& argConfig): mnemonic(mnemonic), argConfig(argConfig) {}
    virtual ~BaseOpcodeConfig() {
        delete argConfig;
        argConfig = nullptr;
    }

    string& getMnemonic() { return mnemonic; }
    BaseOpcodeArgConfig*& getArgConfig() { return argConfig; }

    const string& getMnemonic() const { return mnemonic; }
};

struct BaseParsedOpcodeArg {
protected:
    const BaseOpcodeArgType* type;
public:
    BaseParsedOpcodeArg(): type(BaseOpcodeArgType::NONE) {}
    BaseParsedOpcodeArg(const BaseOpcodeArgType* type): type(type) {}
    virtual ~BaseParsedOpcodeArg() {
        type = nullptr;
    }

    virtual const BaseOpcodeArgType* getType() { return type; }
};

template<typename T>
struct ParsedOpcodeArg: public BaseParsedOpcodeArg {
protected:
    T value;
public:
    using BaseParsedOpcodeArg::BaseParsedOpcodeArg;
    ParsedOpcodeArg(const BaseOpcodeArgType* type, const T& value): BaseParsedOpcodeArg(type), value(value) {}

    virtual T& getValue() { return value; }
};

struct BaseParsedOpcode {
protected:
    vector<BaseParsedOpcodeArg*> args;
    BaseOpcodeConfig* config;
    string instructionCode;
public:
    BaseParsedOpcode(): config() {}
    BaseParsedOpcode(BaseOpcodeConfig*& config, const string& instructionCode, const vector<BaseParsedOpcodeArg*>& args): config(config), instructionCode(instructionCode), args(args) {}
    virtual ~BaseParsedOpcode() {
        delete config;
        config = nullptr;
    }

    virtual BaseOpcodeConfig*& getConfig() { return config; }
    virtual string& getInstructionCode() { return instructionCode; }
    virtual vector<BaseParsedOpcodeArg*>& getArgs() { return args; }

    virtual const string& getInstructionCode() const { return instructionCode; }
    virtual const vector<BaseParsedOpcodeArg*>& getArgs() const { return args; }
};

class BaseOpcode {
protected:
    BaseParsedOpcode* parsedOpc;
public:
    BaseOpcode(): parsedOpc() {}
    BaseOpcode(BaseParsedOpcode* parsedOpc): parsedOpc(parsedOpc) {}
    virtual ~BaseOpcode() {
        delete parsedOpc;
        parsedOpc = nullptr;
    }

    virtual BaseParsedOpcode* getParsedOpc() { return parsedOpc; }
};

template<typename int_t>
class Opcode: public BaseOpcode {
protected:
    int_t raw;
public:
    using BaseOpcode::BaseOpcode;
    Opcode(): BaseOpcode(), raw(0) {}
    Opcode(int_t raw): BaseOpcode(), raw(raw) {}
    Opcode(BaseParsedOpcode* parsedOpc, int_t raw): BaseOpcode(parsedOpc), raw(raw) {}

    virtual int_t& getRaw() { return raw; }
};