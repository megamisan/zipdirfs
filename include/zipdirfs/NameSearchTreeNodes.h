/*
 * Copyright © 2012 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 *
 * This file is part of zipdirfs.
 *
 * zipdirfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * zipdirfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with zipdirfs.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
 */
#ifndef NAMESEARCHTREENODES_H_INCLUDED
#define NAMESEARCHTREENODES_H_INCLUDED

#include "zipdirfs/stream_indent.h"
#include <ostream>

namespace zipdirfs
{
	template < class ValueType, bool ownedValues >
	class NameSearchTree;

	namespace NameSearchTreeNodes
	{
		template < class ValueType >
		class NodeBase
		{
		protected:
			inline NodeBase()
			{
				name = NULL;
				next = NULL;
			}
		public:
			virtual ~NodeBase() {}
			const char* name;
			NodeBase* next;
			virtual bool isLeaf() const = 0;
		};

		template < class ValueType >
		class Node : public NodeBase<ValueType>
		{
		private:
			inline Node() : NodeBase<ValueType>()
			{
				child = NULL;
			}
		public:
			virtual ~Node() {}
			bool isLeaf() const
			{
				return false;
			}
			NodeBase<ValueType>* child;
			friend class zipdirfs::NameSearchTree<ValueType, true>;
			friend class zipdirfs::NameSearchTree<ValueType, false>;
		};

		template < class ValueType >
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
			friend class zipdirfs::NameSearchTree<ValueType, true>;
			friend class zipdirfs::NameSearchTree<ValueType, false>;
		};

		template < class ValueType >
		std::ostream& operator << (std::ostream& out, const NodeBase<ValueType>& node);

		template < class ValueType >
		std::ostream& operator << (std::ostream& out, const Node<ValueType>& node)
		{
			out << std::endl;

			if (node.child != NULL)
			{
				out << indent_inc << * (node.child) << indent_dec;
			}

			return out;
		}

		template < class ValueType >
		std::ostream& operator << (std::ostream& out, const Leaf<ValueType>& node)
		{
			out << " (" << node.value << ")" << std::endl;
			return out;
		}

		template < class ValueType >
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
}

#endif // NAMESEARCHTREENODES_H_INCLUDED

