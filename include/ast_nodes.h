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
	struct Expression;
	struct Block;
	struct Module;
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

	struct Node
	{
		std::string_view span;
	};

	struct NamedNode : Node
	{
		std::string path;
		std::string name;
	};

	struct TypeRef : Node
	{
		std::string local_name;
		std::string fully_qualified_name;
	};

	struct Expression : Node
	{
		TypeRef return_type;
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

	struct Struct : NamedNode
	{
		std::vector<std::string_view> field_names;
		std::vector<TypeRef> field_types;
	};

	struct Trait : NamedNode
	{

	};

	struct Impl : Node
	{

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
			Struct,
			Trait,
			Impl,
			Function
		> node;
	};
}
