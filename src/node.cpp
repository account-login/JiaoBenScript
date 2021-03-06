#include "node.h"
#include "visitor.h"


bool Node::operator==(const Node &rhs) const {
    return typeid(*this) == typeid(rhs);
}


bool Node::operator!=(const Node &rhs) const {
    return !(*this == rhs);
}


static bool node_list_eq(const std::vector<Node::Ptr> &a, const std::vector<Node::Ptr> &b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (*a[i] != *b[i]) {
            return false;
        }
    }
    return true;
}


#define _TO_OTHER(Type) \
    const Type *other = dynamic_cast<const Type *>(&rhs); \
    if (other == nullptr) { return false; }


bool S_Block::operator==(const Node &rhs) const {
    _TO_OTHER(S_Block);
    return node_list_eq(this->stmts, other->stmts);
}


bool S_DeclareList::operator==(const Node &rhs) const {
    _TO_OTHER(S_DeclareList);
    if (this->decls.size() != other->decls.size()) {
        return false;
    }
    for (size_t i = 0; i < this->decls.size(); ++i) {
        if (this->decls[i].name != other->decls[i].name) {
            return false;
        }
        const Node::Ptr &lhs_item = this->decls[i].initial;
        const Node::Ptr &rhs_item = other->decls[i].initial;
        if (!(
            (!lhs_item && !rhs_item)    // l == r == nullptr
            || *lhs_item == *rhs_item)) // l == r
        {
            return false;
        }
    }
    return true;
}


#define _ATTR_EQ(name) (*this->name == *other->name)
#define _ATTR_EQ_OPT(name) (this->name ? _ATTR_EQ(name) : !other->name)


bool S_Condition::operator==(const Node &rhs) const {
    _TO_OTHER(S_Condition);
    return _ATTR_EQ(condition) && _ATTR_EQ(then_block) && _ATTR_EQ_OPT(else_block);
}


bool S_While::operator==(const Node &rhs) const {
    _TO_OTHER(S_While);
    return _ATTR_EQ(condition) && _ATTR_EQ(block);
}


bool S_Return::operator==(const Node &rhs) const {
    _TO_OTHER(S_Return);
    return _ATTR_EQ_OPT(value);
}


bool S_Exp::operator==(const Node &rhs) const {
    _TO_OTHER(S_Exp);
    return _ATTR_EQ(value);
}


bool E_Op::operator==(const Node &rhs) const {
    _TO_OTHER(E_Op);
    return this->op_code == other->op_code && node_list_eq(this->args, other->args);
}


bool E_Var::operator==(const Node &rhs) const {
    _TO_OTHER(E_Var);
    return this->name == other->name;
}


bool E_Func::operator==(const Node &rhs) const {
    _TO_OTHER(E_Func);
    return _ATTR_EQ_OPT(args) && _ATTR_EQ(block);
}


bool E_List::operator==(const Node &rhs) const {
    _TO_OTHER(E_List);
    return node_list_eq(this->value, other->value);
}


#undef _TO_OTHER
#undef _ATTR_EQ
#undef _ATTR_EQ_OPT


void S_Block::accept(NodeVisitor &vis) {
    vis.visit_block(*this);
}


void S_DeclareList::accept(NodeVisitor &vis) {
    vis.visit_declare_list(*this);
}


void S_Condition::accept(NodeVisitor &vis) {
    vis.visit_condition(*this);
}


void S_While::accept(NodeVisitor &vis) {
    vis.visit_while(*this);
}


void S_Return::accept(NodeVisitor &vis) {
    vis.visit_return(*this);
}


void S_Exp::accept(NodeVisitor &vis) {
    vis.visit_stmt_exp(*this);
}


void E_Op::accept(NodeVisitor &vis) {
    vis.visit_op(*this);
}


void E_Var::accept(NodeVisitor &vis) {
    vis.visit_var(*this);
}


void E_Func::accept(NodeVisitor &vis) {
    vis.visit_func(*this);
}


void E_List::accept(NodeVisitor &vis) {
    vis.visit_list(*this);
}


void Program::accept(NodeVisitor &vis) {
    vis.visit_program(*this);
}


void S_Break::accept(NodeVisitor &vis) {
    vis.visit_break(*this);
}


void S_Continue::accept(NodeVisitor &vis) {
    vis.visit_continue(*this);
}


void S_Empty::accept(NodeVisitor &vis) {
    vis.visit_stmt_empty(*this);
}


template<>
void E_Bool::accept(NodeVisitor &vis) {
    vis.visit_bool(*this);
}


template<>
void E_Int::accept(NodeVisitor &vis) {
    vis.visit_int(*this);
}


template<>
void E_Float::accept(NodeVisitor &vis) {
    vis.visit_float(*this);
}


template<>
void E_String::accept(NodeVisitor &vis) {
    vis.visit_string(*this);
}


void E_Null::accept(NodeVisitor &vis) {
    vis.visit_null(*this);
}
