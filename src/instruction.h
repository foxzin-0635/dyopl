#pragma once
#include <vector>
#include <functional>
#include <string>
#include <cstdint>

using namespace std;

struct BaseInstructionArgType {
protected:
    int16_t stored_type;
    
    inline static const BaseInstructionArgType* decdef(int16_t type) {
        return new BaseInstructionArgType(type);
    }
public:
    inline static const BaseInstructionArgType* NONE = decdef(-1); // used in argument types.
    inline static const BaseInstructionArgType* VOID = decdef(0); // used in return types.

    BaseInstructionArgType(int16_t type): stored_type(type) {}
    virtual ~BaseInstructionArgType() = default;

    int16_t getStoredType() const { return stored_type; }
};

class BaseInstructionResult {
protected:
    bool failed;
public:
    BaseInstructionResult() = default;
    virtual ~BaseInstructionResult() = default;

    virtual bool& fail() { return failed; }
};

class BaseInstructionArg {
public:
    BaseInstructionArg() = default;
    virtual ~BaseInstructionArg() = default;
};

template<typename T>
class InstructionArg : public BaseInstructionArg {
protected:
    T value;
public:
    InstructionArg(T& value): BaseInstructionArg(), value(value) {}
    
    virtual T& getValue() { return value; }
};

template<typename T>
class InstructionResult : public BaseInstructionResult {
protected:
    T value;
public:
    InstructionResult(T& value): BaseInstructionResult(), value(value) {}
    
    virtual T& getValue() { return value; }
};

struct BaseInstructionArgConfig {
protected:
    vector<const BaseInstructionArgType*> argTypeOrder;
public:
    BaseInstructionArgConfig() = default;
    BaseInstructionArgConfig(const vector<const BaseInstructionArgType*>&  argTypeOrder): argTypeOrder(argTypeOrder) {}
    virtual ~BaseInstructionArgConfig() {
        for (auto at: argTypeOrder) {
            at = nullptr;
        }

        argTypeOrder.clear();
    }

    virtual vector<const BaseInstructionArgType*> getArgOrder() { return argTypeOrder; }
    virtual const BaseInstructionArgType* argTypeAt(size_t offset) {
        if (offset >= argTypeOrder.size()) {
            return BaseInstructionArgType::NONE;
        }

        return argTypeOrder.at(offset);
    }
};

struct BaseInstructionConfig {
protected:
    BaseInstructionArgConfig* argConfig;
    function<BaseInstructionResult*(vector<BaseInstructionArg*> args)> logic;
    const BaseInstructionArgType* returnType;
public:
    BaseInstructionConfig(): logic([](vector<BaseInstructionArg*> args) {
        int v = 0;
        InstructionResult<int>* res = new InstructionResult<int>(v);

        res->fail() = true;
        return res;
    }), returnType(BaseInstructionArgType::VOID), argConfig() {}
    BaseInstructionConfig(const function<BaseInstructionResult*(vector<BaseInstructionArg*> args)>& logic, const BaseInstructionArgType* returnType, BaseInstructionArgConfig* argConfig): logic(logic), returnType(returnType), argConfig(argConfig) {}
    virtual ~BaseInstructionConfig() {
        delete argConfig;
        argConfig = nullptr;

        returnType = nullptr;
    }

    function<BaseInstructionResult*(vector<BaseInstructionArg*> args)>& getLogic() { return logic; }
    const BaseInstructionArgType*& getReturnType() { return returnType; }
    BaseInstructionArgConfig* getArgConfig() { return argConfig; }

    const function<BaseInstructionResult*(vector<BaseInstructionArg*> args)>& getLogic() const { return logic; }
};

struct BaseParsedInstructionArg {
protected:
    const BaseInstructionArgType* type;
public:
    BaseParsedInstructionArg(): type(BaseInstructionArgType::NONE) {}
    BaseParsedInstructionArg(const BaseInstructionArgType* type): type(type) {}
    virtual ~BaseParsedInstructionArg() {
        type = nullptr;
    }

    virtual const BaseInstructionArgType* getType() { return type; }
    virtual BaseInstructionArg* toIntepreterArg() {
        BaseInstructionArg* iarg = new BaseInstructionArg();

        return iarg;
    }
};

template<typename T>
struct ParsedInstructionArg: public BaseParsedInstructionArg {
protected:
    T value;
public:
    using BaseParsedInstructionArg::BaseParsedInstructionArg;
    ParsedInstructionArg(const BaseOpcodeArgType* type, const T& value): BaseParsedInstructionArg(type), value(value) {}

    virtual T& getValue() { return value; }
    virtual BaseInstructionArg* toIntepreterArg() override {
        InstructionArg<T>* iarg = new InstructionArg<T>(value);
        
        return iarg;
    }
};

struct BaseParsedInstruction {
protected:
    vector<BaseParsedInstructionArg*> args;
    BaseInstructionConfig* config;
    string instructionCode;
public:
    BaseParsedInstruction(): config() {}
    BaseParsedInstruction(BaseInstructionConfig* config, const string& instructionCode, const vector<BaseParsedInstructionArg*>& args): config(config), instructionCode(instructionCode), args(args) {}
    virtual ~BaseParsedInstruction() {
        delete config;
        config = nullptr;
    }

    virtual BaseInstructionResult* run(const vector<BaseInstructionArg*>& args) {
        return config->getLogic()(args);
    }
    virtual const BaseInstructionArgType*& getReturnType() {
        return config->getReturnType();
    }

    virtual BaseInstructionConfig* getConfig() {
        return config;
    }

    virtual string& getInstructionCode() { return instructionCode; }
    virtual const string& getInstructionCode() const { return instructionCode; }

    virtual vector<BaseParsedInstructionArg*>& getArgs() { return args; }
    virtual const vector<BaseParsedInstructionArg*>& getArgs() const { return args; }
};

class BaseInstruction {
protected:
    BaseParsedInstruction* parsedInst;
public:
    BaseInstruction(): parsedInst() {}
    BaseInstruction(BaseParsedInstruction* parsedInst): parsedInst(parsedInst) {}
    virtual ~BaseInstruction() {
        delete parsedInst;
        parsedInst = nullptr;
    }

    virtual BaseParsedInstruction* getParsedInst() { return parsedInst; }
};