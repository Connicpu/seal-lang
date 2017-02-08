#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <memory>

namespace ast::nodes
{
    struct Type;
    struct Statement;
    struct Literal;
    struct Identifier;
    struct MemberAccess;
    struct BinaryOp;
    struct UnaryOp;
    struct FunctionCall;
    struct IfElse;
    struct 
    struct Expression;
    struct Statement;
    struct Block;
    struct Module;
    struct TypeDef;
    struct ConstDef;
    struct Struct;
    struct Trait;
    struct Impl;
    struct Function;
    struct Item;

    enum class SelfType
    {
        None,
        Self,
        MutSelf,
        RefSelf,
        RefMutSelf,
    };

    enum class BinOp
    {
        Add,
        Sub,
        Mul,
        Div,
        Rem,
        Shl,
        Shr,
        BitAnd,
        BitOr,
        BitXor,
    };

    enum class UnOp
    {
        Plus,  // +
        Minus, // -
        Not,   // !
        Heap,  // ~
        Deref, // *
        Addr,  // &
    };

    struct Node
    {
        std::string_view span;
    };

    struct NamedNode : Node
    {
        std::string path;
        std::string name;
    };

    struct Identifier : Node
    {
        std::vector<std::string_view> scope;
        std::string_view name;
    };

    struct HeapType
    {
        std::unique_ptr<TypeRef> inner;
    };

    struct ArrayType
    {
        std::unique_ptr<TypeRef> inner;
        std::unique_ptr<Expression> size; // optional
    };

    struct TupleType
    {
        std::vector<TypeRef> types;
    };

    struct ScopedType
    {
        Identifier type_id;
        std::vector<TypeRef> generic_types;
    };

    struct TypeRef : Node
    {
        std::variant<ScopedType, TupleType, ArrayType, HeapType> type;
    };

    struct Literal : Node
    {
        std::variant<
            int64_t,
            uint64_t,
            double,
            std::string_view,
            std::string
        > value;
    };

    struct MemberAccess : Node
    {
        std::unique_ptr<Expression> object;
        std::string_view member;
    };

    struct BinaryOp
    {
        BinOp op;
        std::unique_ptr<Expression> lhs, rhs;
    };

    struct UnaryOp
    {
        UnOp op;
        std::unique_ptr<Expression> expr;
    };

    struct FunctionCall
    {
        std::unique_ptr<Expression> object; // optional
        Identifier function;
        std::vector<Expression> arguments;
    };

    struct Expression : Node
    {
        std::variant<
            Literal,
            Identifier,
            MemberAccess,
            BinaryOp,
            UnaryOp,
            FunctionCall,
            IfElse,
            std::unique_ptr<Block>,
            void
        > value;
    };

    struct Block : Expression
    {
        std::optional<std::string_view> label;
    };

    struct Module : NamedNode
    {
        std::vector<std::string> module_imports;
        std::vector<Item> items;
    };

    struct WhereClause
    {
        TypeRef bound_type;
        TypeRef trait;
    };

    struct TypeDef : NamedNode
    {
        std::vector<std::string_view> generic_names;
        TypeRef type;
    };

    struct ConstDef : NamedNode
    {
        Expression expression;
    };

    struct Struct : NamedNode
    {
        std::vector<std::string_view> generic_names;
        std::vector<WhereClause> where_clauses;
        std::vector<std::string_view> field_names;
        std::vector<TypeRef> field_types;
    };

    struct Trait : NamedNode
    {
        std::vector<TypeDef> associated_types;
        std::vector<Function> functions;
    };

    struct Impl : Node
    {
        std::vector<std::string_view> generic_names;
        std::optional<TypeRef> trait;
        TypeRef type;
        std::vector<WhereClause> where_clauses;
    };

    struct Function : NamedNode
    {
        SelfType self_type;
        TypeRef return_type;
        std::vector<TypeRef> arguments;
        Block root_block;
    };

    struct Attribute : Node
    {
        std::string_view key;
        std::variant<
            std::unique_ptr<Attribute>,
            std::string_view,
            std::string
        > value;
    };

    struct Item : Node
    {
        bool pub;
        std::vector<Attribute> attributes;
        std::variant<
            Module,
            TypeDef,
            Struct,
            Trait,
            Impl,
            Function
        > node;
    };
}
