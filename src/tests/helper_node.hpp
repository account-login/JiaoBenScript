#ifndef HELPER_NODE_HPP
#define HELPER_NODE_HPP

#include "../node.h"


Node *make_ops(OpCode opcode, const std::vector<Node *> &args) {
    E_Op *exp = new E_Op(opcode);
    for (Node *item : args) {
        exp->args.emplace_back(item);
    }
    return exp;
}


Node *make_ops(uint32_t opcode, const std::vector<Node *> &args) {
    return make_ops(static_cast<OpCode>(opcode), args);
}


Node *make_binop(uint32_t opcode, Node *lhs, Node *rhs) {
    return make_ops(opcode, {lhs, rhs});
}


Node *T(int value) {
    return new E_Int(value);
}


Node *V(const std::string &name) {
    return new E_Var(u8_decode(name));
}


Node *make_list(const std::vector<Node *> &args) {
    E_List *list = new E_List();
    for (Node *item : args) {
        list->value.emplace_back(item);
    }
    return list;
}


S_DeclareList *make_decl_list(const std::vector<std::pair<std::string, Node *>> &pairs) {
    S_DeclareList *decls = new S_DeclareList();
    for (const auto &p : pairs) {
        decls->decls.emplace_back(u8_decode(p.first), Node::Ptr(p.second));
    }
    return decls;
}

S_Block *make_block(const std::vector<Node *> stmts) {
    S_Block *block = new S_Block();
    for (Node *stmt : stmts) {
        block->stmts.emplace_back(stmt);
    }
    return block;
}


E_Func *make_func(Node *args, Node *block) {
    E_Func *func = new E_Func();
    func->args.reset(args);
    func->block.reset(block);
    return func;
}


S_Condition *make_cond(Node *test, Node *then_block, Node *else_block) {
    S_Condition *cond = new S_Condition();
    cond->condition.reset(test);
    cond->then_block.reset(then_block);
    cond->else_block.reset(else_block);
    return cond;
}


S_While *make_while(Node *test, Node *block) {
    S_While *wh = new S_While();
    wh->condition.reset(test);
    wh->block.reset(block);
    return wh;
}


#endif  // HELPER_NODE_HPP
