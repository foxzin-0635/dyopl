#pragma once
#include <cstddef>
#include <functional>
#include <vector>
#include <string>

#include "astsemantic.h"

using namespace std;

// Base node
class ASTNode {
protected:
    size_t start;
    size_t end;
public:
    ASTNode() = default;
    ASTNode(size_t start, size_t end): start(start), end(end) {}
    
    virtual size_t getStart() { return start; }
    virtual const size_t getStart() const { return start; }
    
    virtual size_t getEnd() { return end; }
    virtual const size_t getEnd() const { return end; }
};

// Base class for StatementResult<T>.
class BaseStatementResult {
protected:
    bool failed;
public:
    BaseStatementResult() = default;
    virtual ~BaseStatementResult() = default;

    virtual bool& fail() { return failed; }
};

// Base class for StatementArg<T>.
class BaseStatementArg {
public:
    BaseStatementArg() = default;
    virtual ~BaseStatementArg() = default;
};

// Holds arguments for StatementNodes.
template<typename T>
class StatementArg : public BaseStatementArg {
protected:
    T value;
public:
    StatementArg(T& value): BaseStatementArg(), value(value) {}
    
    virtual T& getValue() { return value; }
};

// Holds the result for StatementNodes.
template<typename T>
class StatementResult : public BaseStatementResult {
protected:
    T value;
public:
    StatementResult(T& value): BaseStatementResult(), value(value) {}
    
    virtual T& getValue() { return value; }
};

// The statements node.
class StatementNode : public ASTNode {
protected:
    function<BaseStatementResult*(vector<BaseStatementArg*>)> logic;

    friend class BaseASTVisitor<const BaseSemanticType*>;
public:
    StatementNode(): ASTNode(), logic([](vector<BaseStatementArg*> args) {StatementResult<const string&>* v = new StatementResult<const string&>("not implemented."); v->fail() = true; return v;}) {}
    StatementNode(const function<BaseStatementResult*(vector<BaseStatementArg*>)>& logic, size_t start, size_t end): ASTNode(start, end), logic(logic) {}
    virtual ~StatementNode() = default;
    
    virtual BaseStatementResult* run(vector<BaseStatementArg*> args) { return logic(args); }
    
    template<typename T = const BaseSemanticType*>
    T accept(BaseASTVisitor<T>* visitor) {
        return visitor->visit(this);
    }
};

// Base class for ExpressionNode<T>.
class BaseExpressionNode: public ASTNode {
public:
    BaseExpressionNode(): ASTNode() {}
    BaseExpressionNode(size_t start, size_t end): ASTNode(start, end) {}
    virtual ~BaseExpressionNode() = default;
};

// The expressions node.
template<typename T>
class ExpressionNode: public BaseExpressionNode {
protected:
    T value;
public:
    ExpressionNode(): BaseExpressionNode() {}
    ExpressionNode(T& value, size_t start, size_t end): BaseExpressionNode(start, end), value(value) {}
    
    virtual T& getValue() { return value; }
};

// You can add more nodes with those base classes above.
// If you want to add a value node, use ExpressionNode<T>,
// If you want to add a functional node, use StatementNode.