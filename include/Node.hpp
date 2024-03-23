#pragma once

#include "ASTNode.hpp"
#include "ScopeNode.hpp"

#include <iostream>
#include <unordered_map>
#include <functional>

//---------------------------------------------------------
class NumNode final: public ASTNode {
    int value;

    public:
    NumNode(int value, std::shared_ptr<ScopeNode> scope): value(value), ASTNode(scope, NodeType::Num) { }; 

    inline int calculate() { return value; }
    inline void dump_ast(std::ofstream& dump_file) {
        dump_file << "Num Node \n{ type: value_node \n    { value: " << value << "\n    }\n}" << std::endl; 
    }

    ~NumNode() {};
};
//---------------------------------------------------------
class IdNode final: public ASTNode  {
    int id;

    public:
    IdNode(int id, std::shared_ptr<ScopeNode> scope): id(id), ASTNode(scope, NodeType::Id) {};

    inline int calculate() { return id; }
    
    
    inline void dump_ast(std::ofstream& dump_file) {
        dump_file << "Id Node \n{" << "\n   ID: " << id << "\n}"<< std::endl;
    };

    ~IdNode() {};
};
//---------------------------------------------------------
inline std::shared_ptr<ASTNode> createSetIdNode(const std::string name, std::shared_ptr<ScopeNode> scope)
{
    int id = scope->getIdFromSetVariable(name);
    return std::shared_ptr<ASTNode>(new IdNode(id, scope));
};
//---------------------------------------------------------
inline std::shared_ptr<ASTNode> createGetIdNode(const std::string name, std::shared_ptr<ScopeNode> scope)
{
    int id = scope->getIdFromGetVariable(name);
    return std::shared_ptr<ASTNode>(new IdNode(id, scope));
};
//---------------------------------------------------------
class BinaryNode final: public ASTNode {
    BinaryOp Op;
    std::shared_ptr<ASTNode> l;
    std::shared_ptr<ASTNode> r;

    static std::unordered_map<BinaryOp, std::function<int(int, int)>> OperationMap;

    int calculate() override;
    inline void dump_ast(std::ofstream& dump_file) override {
        dump_file << "Operator Node \n{ " << std::endl;
        l->dump_ast(dump_file);
        r->dump_ast(dump_file);
        dump_file << " \n}" << std::endl;
    };

    public:
    BinaryNode(std::shared_ptr<ASTNode> l, BinaryOp Op, std::shared_ptr<ASTNode> r,
    std::shared_ptr<ScopeNode> scope): l(l), Op(Op), r(r), ASTNode(scope, NodeType::Operation) { 

    };

    ~BinaryNode() {};
};
//---------------------------------------------------------
class IfNode final: public ASTNode {
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> body;
    std::shared_ptr<ASTNode> else_block;

    public:
    IfNode(std::shared_ptr<ASTNode> condition, std::shared_ptr<ASTNode> body, std::shared_ptr<ASTNode> else_block, 
    std::shared_ptr<ScopeNode> scope): condition(condition), body(body), else_block(else_block), 
    ASTNode(scope, NodeType::If) {};

    int calculate() override {

        if(condition->calculate()) {
            if(body != 0) {
                return body->calculate();
            }
        }
        else {
            if(else_block != 0) {
                return else_block->calculate();
            }
        }
        return 0;
    }

    void dump_ast(std::ofstream& dump_file) override {
        dump_file << "If Node \n{ " << std::endl;

        condition->dump_ast(dump_file);

        if(body != 0) { body->dump_ast(dump_file); }

        if(else_block != 0) { else_block->dump_ast(dump_file); }
        
        dump_file << " \n}" << std::endl;
    }

    ~IfNode() {};
};
//---------------------------------------------------------
class WhileNode final: public ASTNode {
    std::shared_ptr<ASTNode> condition;
    std::shared_ptr<ASTNode> block;

    public:
    WhileNode(std::shared_ptr<ASTNode> condition, std::shared_ptr<ASTNode> block, 
    std::shared_ptr<ScopeNode> scope): condition(condition), block(block), 
    ASTNode(scope, NodeType::While) {};

    int calculate() override {
        while(condition->calculate()) {
            block->calculate();
        }
        return 0;
    }
    void dump_ast(std::ofstream& dump_file) override {
        dump_file << "While Node \n{" << std::endl;
        condition->dump_ast(dump_file); 
        if(block != 0) { block->dump_ast(dump_file); }
        dump_file << " \n}" << std::endl;
    }

    ~WhileNode() {};
};
//---------------------------------------------------------
class AssignmentNode: public ASTNode {
    std::shared_ptr<ASTNode> lval;
    std::shared_ptr<ASTNode> right;

    inline int calculate() override;
    inline void dump_ast(std::ofstream& dump_file) override { 
        dump_file << "{ Assignment Node \n{ " << std::endl;
        lval->dump_ast(dump_file);
        right->dump_ast(dump_file);
        dump_file << " \n}\n}" << std::endl;
    };

    public:
    AssignmentNode(std::shared_ptr<ASTNode>  lval, std::shared_ptr<ASTNode> exprNode, 
    std::shared_ptr<ScopeNode> scope): right(exprNode), lval(lval), 
    ASTNode(scope, NodeType::Assign) {};

    ~AssignmentNode() {};
};
//---------------------------------------------------------
inline int AssignmentNode::calculate() {
    scope->lookup(lval->calculate())->value = ((right->get_type() == NodeType::Id) ? 
    scope->lookup(right->calculate())->value : right->calculate());
    return 1;
};
//---------------------------------------------------------
class UnaryNode: public ASTNode {
    UnaryOp Op;
    std::shared_ptr<ASTNode> operand;

    public:
    UnaryNode(UnaryOp Op, std::shared_ptr<ASTNode> operand, std::shared_ptr<ScopeNode> scope): Op(Op), 
    operand(operand), ASTNode(scope, NodeType::Operation) {};

    int calculate() override;

    void dump_ast(std::ofstream& dump_file) override {

    };
};
//---------------------------------------------------------
class InputNode: public ASTNode {
    int value;

    public:
    InputNode(std::shared_ptr<ScopeNode> scope): ASTNode(scope, NodeType::Input) {};

    int calculate() override {
        std::cin >> value;
        return value;
    };

    void dump_ast(std::ofstream& dump_file) override {
        dump_file << "Input Node" << std::endl;
    };

};
//---------------------------------------------------------
class OutputNode: public ASTNode {
    std::shared_ptr<ASTNode> child;
    
    public:
    OutputNode(std::shared_ptr<ASTNode> child, std::shared_ptr<ScopeNode> scope): 
    child(child), ASTNode(scope, NodeType::Output) {};

    int calculate() override {
        if(child->calculate() == -1) { return 0;}
        auto result = (child->get_type() == NodeType::Id) ? scope->lookup(child->calculate())->value :
        child->calculate();
        std::cout << result << std::endl;
        return 0;
    };

    void dump_ast(std::ofstream& dump_file) override {
        dump_file << "Output Node" << std::endl;
    };

};
//---------------------------------------------------------