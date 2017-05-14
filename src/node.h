#ifndef JIAOBENSCRIPT_NODE_H
#define JIAOBENSCRIPT_NODE_H

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <vector>

#include "sourcepos.h"
#include "unicode.h"
#include "string_fmt.hpp"
#include "repr.hpp"


// forward declarations
class S_Block;
class Program;
class S_DeclareList;
class S_Condition;
class S_While;
class S_Return;
class S_Break;
class S_Continue;
class S_Exp;
class S_Empty;
class E_Op;
class E_Var;
class E_Func;

template<class ValueType>
class _E_Value;

typedef _E_Value<bool> E_Bool;
typedef _E_Value<int64_t> E_Int;
typedef _E_Value<double> E_Float;
typedef _E_Value<ustring> E_String;

class E_List;
class E_Null;


class NodeVistor {
public:
    virtual ~NodeVistor() {}

    virtual void visit_block(S_Block &) {}
    virtual void visit_program(Program &) {}
    virtual void visit_declare_list(S_DeclareList &) {}
    virtual void visit_condition(S_Condition &) {}
    virtual void visit_while(S_While &) {}
    virtual void visit_return(S_Return &) {}
    virtual void visit_break(S_Break &) {}
    virtual void visit_continue(S_Continue &) {}
    virtual void visit_stmt_exp(S_Exp &) {}
    virtual void visit_stmt_empty(S_Empty &) {}
    virtual void visit_op(E_Op &) {}
    virtual void visit_var(E_Var &) {}
    virtual void visit_func(E_Func &) {}
    virtual void visit_bool(E_Bool &) {}
    virtual void visit_int(E_Int &) {}
    virtual void visit_float(E_Float &) {}
    virtual void visit_string(E_String &) {}
    virtual void visit_list(E_List &) {}
    virtual void visit_null(E_Null &) {}
};


struct Node {
    typedef std::unique_ptr<Node> Ptr;

    virtual ~Node() {}
    virtual bool operator==(const Node &rhs) const;
    bool operator!=(const Node &rhs) const;
    virtual std::string repr(uint32_t = 0) const {
        return "<Node>";
    }
    virtual void accept(NodeVistor &vis) = 0;

    SourcePos pos_start;
    SourcePos pos_end;
};


REPR(Node) {
    return value.repr();
}


struct S_Block : Node {
    struct AttrType {
        struct VarInfo {
            VarInfo(const ustring &name) : name(name) {}
            ustring name;

            bool operator==(const VarInfo &rhs) const {
                return this->name == rhs.name;
            }
        };

        struct NonLocalInfo {
            NonLocalInfo(S_Block *parent, int index) : parent(parent), index(index) {}

            S_Block *parent = nullptr;
            int index = -1;

            bool operator==(const NonLocalInfo &rhs) const {
                return this->parent == rhs.parent && this->index == rhs.index;
            }
        };

        S_Block *parent = nullptr;
        std::vector<VarInfo> local_info;
        std::vector<NonLocalInfo> nonlocal_indexes;

        // tmp
        std::map<ustring, int> name_to_local_index;
        std::map<ustring, int> name_to_nonlocal_index;
    };

    std::vector<Node::Ptr> stmts;
    AttrType attr {};

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_block(*this);
    }
};


struct Program : S_Block {
    virtual void accept(NodeVistor &vis) override {
        vis.visit_program(*this);
    }
};


struct A_DeclareList {
    int start_index = -1;
};


struct S_DeclareList : Node {
    struct PairType {
        PairType(const ustring name, Node::Ptr initial)
            : name(name), initial(std::move(initial))
        {}

        ustring name;
        Node::Ptr initial;
    };
    std::vector<PairType> decls;
    mutable A_DeclareList attr;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_declare_list(*this);
    }
};


struct S_Condition : Node {
    Node::Ptr condition;
    Node::Ptr then_block;
    Node::Ptr else_block;   // optional, S_Block or S_Condition

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_condition(*this);
    }
};


struct S_While : Node {
    Node::Ptr condition;
    Node::Ptr block;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_while(*this);
    }
};


// TODO: S_For, S_DoWhile


struct S_Return : Node {
    Node::Ptr value;    // optional

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_return(*this);
    }
};


struct S_Break : Node {
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_break(*this);
    }
};


struct S_Continue : Node {
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_continue(*this);
    }
};


struct S_Exp : Node {
    Node::Ptr value;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_stmt_exp(*this);
    }
};


struct S_Empty : Node {
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_stmt_empty(*this);
    }
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
    EXPLIST         = ',',
};


struct E_Op : Node {
    E_Op(OpCode op_code) : op_code(op_code) {}

    OpCode op_code;
    std::vector<Node::Ptr> args;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_op(*this);
    }
};


struct E_Var : Node {
    struct AttrType {
        bool is_local;
        int index = -1;
    };

    E_Var(const ustring &name) : name(name) {}

    ustring name;
    AttrType attr {};

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_var(*this);
    }
};


struct E_Func : Node {
    Node::Ptr args;     // S_DeclareList, optional
    Node::Ptr block;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_func(*this);
    }
};


template<class ValueType>
struct _E_Value : Node {
    typedef _E_Value<ValueType> _SelfType;
    _E_Value<ValueType>(const ValueType &value) : value(value) {}

    ValueType value;

    virtual bool operator==(const Node &rhs) const override {
        const _SelfType *other = dynamic_cast<const _SelfType *>(&rhs);
        return other != nullptr && this->value == other->value;
    }
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override;
};


template<class T>
inline std::string _my_to_string(const T &value) {
    return std::to_string(value);
}


template<>
inline std::string _my_to_string<ustring>(const ustring &value) {
    return u8_encode(value);
}


template<>
inline std::string _my_to_string<bool>(const bool &value) {
    return value ? "true" : "false";
}


template<class ValueType>
inline std::string _E_Value<ValueType>::repr(uint32_t) const {
    return _my_to_string(this->value);
}


template<>
inline void E_Bool::accept(NodeVistor &vis) {
    vis.visit_bool(*this);
}


template<>
inline void E_Int::accept(NodeVistor &vis) {
    vis.visit_int(*this);
}


template<>
inline void E_Float::accept(NodeVistor &vis) {
    vis.visit_float(*this);
}


template<>
inline void E_String::accept(NodeVistor &vis) {
    vis.visit_string(*this);
}


struct E_List : Node {
    std::vector<Node::Ptr> value;

    virtual bool operator==(const Node &rhs) const override;
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_list(*this);
    }
};


struct E_Null : Node {
    virtual std::string repr(uint32_t indent = 0) const;
    virtual void accept(NodeVistor &vis) override {
        vis.visit_null(*this);
    }
};


#endif //JIAOBENSCRIPT_NODE_H
