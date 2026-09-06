#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <exception>

using namespace std;

class BaseVariable {
protected:
    string name;
    bool is_const = false;
public:
    BaseVariable(): name("") {}
    BaseVariable(const string& name): name(name) {}
    virtual ~BaseVariable() = default;
    
    virtual bool isConst() { return is_const; }
    virtual void switch_const() { is_const = !is_const; }
    
    virtual string& getName() { return name; }
};

template<typename T>
class Variable: public BaseVariable {
protected:
    T value;
public:
    using BaseVariable::BaseVariable;
    Variable(const string& name, T& value): BaseVariable(name), value(value) {}
    
    virtual T& getValue() { return value; }
};

class BaseVariableStack {
protected:
    vector<BaseVariable*> stack;

    class vstack_exception: exception {
    private:
        string str;
    public:
        explicit vstack_exception(const string& msg): str("vstack_exception: "+msg) {}
        
        const char* what() const noexcept override {
            return str.c_str();
        }
    };
public:
    BaseVariableStack(): stack(0) {}
    virtual ~BaseVariableStack() {
        clear();
        stack.clear();
    }
    
    virtual void push_back(BaseVariable* var) { stack.push_back(var); }
    virtual BaseVariable* pop_back() { BaseVariable* v = stack.back(); stack.pop_back(); return v; }
    virtual BaseVariable* pop(size_t offset) { BaseVariable* v = stack[offset]; stack.erase(remove(stack.begin(), stack.end(), v), stack.end()); return v; }
    
    template<typename T>
    inline Variable<T>* cast(BaseVariable* var) {
        if (auto* c = dynamic_cast<Variable<T>*>(var)) {
            return c;
        }

        throw new vstack_exception("variable cast type is invalid for 'BaseVariable* var'.");
    }
    
    virtual size_t size() { return stack.size(); }
    
    virtual BaseVariable* get(size_t offset) {
        return stack[offset];
    }

    virtual void clear() {
        for (auto ptr : stack) {
            delete ptr;
            ptr = nullptr;
        }

        stack.clear();
    }
};