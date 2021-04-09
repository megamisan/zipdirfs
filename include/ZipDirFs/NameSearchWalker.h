/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
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
 */
#ifndef NAMESEARCHWALKER_H
#define NAMESEARCHWALKER_H

#include "ZipDirFs/NameSearchTreeNodes.h"
#include <iterator>
#include <deque>
#include <string.h>

namespace ZipDirFs
{
	template < class ValueType, bool ownedValues >
	class NameSearchTree;

	/**
	 * \brief Iterator on NameSearchTree Leaf nodes.
	 * Walks the tree hierarchy to provide a flat view of all available entry names.
	 * Removing the entry corresponding to the current name from the tree will result in application crash (SEGFAULT).
	 * Removing any other entry will not cause any failure. Adding a new entry may cause not walked entries depending
	 * on the operations needed for the change.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class ValueType >
	class NameSearchWalker : public std::iterator<std::input_iterator_tag, const std::string>
	{
	public:
		NameSearchWalker(const NameSearchWalker& it) : parents(it.parents), current(it.current), name(it.name) { }
		NameSearchWalker& operator = (const NameSearchWalker& it)
		{
			parents = it.parents;
			current = it.current;
			name = it.name;
			return *this;
		}
		~NameSearchWalker() {}
		NameSearchWalker& operator ++()
		{
			this->advance();
			return *this;
		}
		NameSearchWalker operator ++ (int)
		{
			NameSearchWalker it(*this);
			operator ++();
			return it;
		}
		bool operator == (const NameSearchWalker& it) const
		{
			return (current == it.current) && (parents == it.parents);
		}
		bool operator != (const NameSearchWalker& it) const
		{
			return (current != it.current) || (parents != it.parents);
		}
		reference operator * () const
		{
			return name;
		}
		pointer operator -> () const
		{
			return &name;
		}
		friend class NameSearchTree<ValueType, false>;
		friend class NameSearchTree<ValueType, true>;
	protected:
	private:
		std::deque<const NameSearchTreeNodes::NodeBase<ValueType>*> parents; /**< \brief The stack of parent nodes. */
		const NameSearchTreeNodes::NodeBase<ValueType> *current; /**< \brief The current tree node. */
		std::string name; /**< \brief The current built name. */
		NameSearchWalker(const NameSearchTreeNodes::NodeBase<ValueType> *root) : current(root)
		{
			this->advanceFirst();
		}
		/**
		 * \brief Walks to the first Leaf.
		 */
		void advanceFirst()
		{
			this->advanceToLeaf();
		}
		/**
		 * \brief Walks to the next Leaf
		 */
		void advance()
		{
			this->advanceToNext();
			this->advanceToLeaf();
		}
		/**
		 * \brief Walks to the next node.
		 * The next node can be in the same branch or a parent branch.
		 * This step enables the definition of advanceToLeaf.
		 */
		void advanceToNext()
		{
			if (current != NULL)
			{
				name.resize(name.size() - ::strlen(current->name));
				current = current->next;
			}

			this->parentWalkIfNeeded();
		}
		/**
		 * \brief Walks to the next Leaf.
		 * Nothing is done in the case of an invalid current,
		 * as we have been previously positionned in a search state,
		 * it means we arrived at the end or were already there.
		 */
		void advanceToLeaf()
		{
			while (current != NULL)
			{
				if (current->isLeaf())
				{
					break;
				}
				else
				{
					parents.push_back(current);
					name.append(current->name);
					current = static_cast<const NameSearchTreeNodes::Node<ValueType>*>(current)->child;
				}

				this->parentWalkIfNeeded();
			}

			if (current != NULL)
			{
				name.append(current->name);
			}
		}
		/**
		 * \brief Go back in the herarchy to find the next usable node.
		 * While the current node is not valid, go to the next node of the topmost parent node in the stack.
		 */
		inline void parentWalkIfNeeded()
		{
			while ((current == NULL) && (!parents.empty()))
			{
				current = parents.back();
				parents.pop_back();
				name.resize(name.size() - ::strlen(current->name));
				current = current->next;
			}
		}
	};
}

#endif // NAMESEARCHWALKER_H
