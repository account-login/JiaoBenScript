#ifndef JIAOBENSCRIPT_NODE_H
#define JIAOBENSCRIPT_NODE_H

#include <cassert>
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

#include "sourcepos.h"
#include "unicode.h"
#include "string_fmt.hpp"
#include "utils.hpp"


struct Node {
    typedef std::unique_ptr<Node> Ptr;

    virtual ~Node() {}
    virtual bool operator==(const Node &rhs) const;
    bool operator!=(const Node &rhs) const;
    virtual std::string repr(uint32_t indent = 0) const {
        return "<Node>";
    }

    SourcePos pos_start;
    SourcePos pos_end;
};


REPR(Node) {
    return value.repr();
}


struct S_Block : Node {
    std::vector<Node::Ptr> stmts;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


struct S_DeclareList : Node {
    typedef std::tuple<ustring, Node::Ptr> PairType;
    std::vector<PairType> decls;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


struct S_Condition : Node {
    Node::Ptr condition;
    Node::Ptr then_block;
    Node::Ptr else_block;   // optional, S_Block or S_Condition

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


struct S_While : Node {
    Node::Ptr condition;
    Node::Ptr block;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


// TODO: S_For, S_DoWhile


struct S_Return : Node {
    Node::Ptr value;    // optional

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


struct S_Break : Node {
    virtual std::string repr(uint32_t indent = 0) const;
};


struct S_Continue : Node {
    virtual std::string repr(uint32_t indent = 0) const;
};


struct S_Exp : Node {
    Node::Ptr value;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


struct S_Empty : Node {
    virtual std::string repr(uint32_t indent = 0) const;
};


enum class OpCode : uint32_t {
    // copied from TokenCode
    PLUS            = '+',
    MINUS           = '-',
    STAR            = '*',
    SLASH           = '/',
    PERCENT         = '%',
    LESS            = '<',
    LESSEQ          = '<=',
    GREAT           = '>',
    GREATEQ         = '>=',
    EQ              = '==',
    NEQ             = '!=',
    NOT             = '!',
    AND             = '&&',
    OR              = '||',
    ASSIGN          = '=',
    PLUS_ASSIGN     = '+=',
    MINUS_ASSIGN    = '-=',
    STAR_ASSIGN     = '*=',
    SLASH_ASSIGN    = '/=',
    PERCENT_ASSIGN  = '%=',

    CALL            = '()',
    SUBSCRIPT       = '[]',
    SETITEM         = '[]=',
    EXPLIST         = ',',
};


struct E_Op : Node {
    OpCode op_code;
    std::vector<Node::Ptr> args;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


struct E_Var : Node {
    ustring name;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


struct E_Func : Node {
    Node::Ptr args;     // S_DeclareList
    Node::Ptr block;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


template<class ValueType>
struct _E_Value : Node {
    ValueType value;

    virtual bool operator==(const Node &rhs) const override {
        const _E_Value<ValueType> *other = dynamic_cast<_E_Value<ValueType> *>(&rhs);
        return other != nullptr && this->value == other->value;
    }
    virtual std::string repr(uint32_t indent = 0) const;
};


typedef _E_Value<bool> E_Bool;
typedef _E_Value<int64_t> E_Int;
typedef _E_Value<double> E_Float;
typedef _E_Value<ustring> E_String;


struct E_List : Node {
    std::vector<Node::Ptr> value;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
};


struct E_Null : Node {
    virtual std::string repr(uint32_t indent = 0) const;
};


#endif //JIAOBENSCRIPT_NODE_H