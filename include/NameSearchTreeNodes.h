#ifndef NAMESEARCHTREENODES_H_INCLUDED
#define NAMESEARCHTREENODES_H_INCLUDED

#include "stream_indent.h"
#include <ostream>

template <
class ValueType,
bool ownedValues
>
class NameSearchTree;

namespace NameSearchTreeNodes
{
	template <
	class ValueType
	>
	class NodeBase
	{
	protected:
		inline NodeBase() { name = NULL; next = NULL; }
	public:
		virtual ~NodeBase() {}
		const char* name;
		NodeBase* next;
		virtual bool isLeaf() const = 0;
	};

	template <
	class ValueType
	>
	class Node : public NodeBase<ValueType>
	{
	private:
		inline Node() : NodeBase<ValueType>() { child = NULL; }
	public:
		virtual ~Node() {}
		bool isLeaf() const
		{
			return false;
		}
		NodeBase<ValueType>* child;
		friend class ::NameSearchTree<ValueType, true>;
		friend class ::NameSearchTree<ValueType, false>;
	};

	template <
	class ValueType
	>
	class Leaf : public NodeBase<ValueType>
	{
	private:
		inline Leaf() : NodeBase<ValueType>() {}
	public:
		virtual ~Leaf() {}
		bool isLeaf() const
		{
			return true;
		};
		ValueType value;
		friend class ::NameSearchTree<ValueType, true>;
		friend class ::NameSearchTree<ValueType, false>;
	};

	template <
	class ValueType
	>
	std::ostream& operator << (std::ostream& out, const NodeBase<ValueType>& node);

	template <
	class ValueType
	>
	std::ostream& operator << (std::ostream& out, const Node<ValueType>& node)
	{
		out << std::endl;

		if (node.child != NULL)
		{
			out << indent_inc << * (node.child) << indent_dec;
		}

		return out;
	}

	template <
	class ValueType
	>
	std::ostream& operator << (std::ostream& out, const Leaf<ValueType>& node)
	{
		out << " (" << node.value << ")" << std::endl;
		return out;
	}

	template <
	class ValueType
	>
	std::ostream& operator << (std::ostream& out, const NodeBase<ValueType>& node)
	{
		const NodeBase<ValueType>* current = &node;

		while (current != NULL)
		{
			out << indent << current->name;

			if (current->isLeaf() )
			{
				out << static_cast<const Leaf<ValueType>&> (*current);
			}
			else
			{
				out << static_cast<const Node<ValueType>&> (*current);
			}

			current = current->next;
		}

		return out;
	}
}

#endif // NAMESEARCHTREENODES_H_INCLUDED

