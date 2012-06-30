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
#ifndef NAMESEARCHTREE_H
#define NAMESEARCHTREE_H

#include "NameSearchTreeNodes.h"
#include "NameSearchWalker.h"
#include "NotFoundException.h"
#include "stream_indent.h"
#include <string.h>
#include <ostream>

template <
class ValueType,
bool ownedValues
>
class NameSearchTree;

template <
class ValueType,
bool ownedValues
>
std::ostream& operator << (std::ostream& out, const NameSearchTree<ValueType, ownedValues>& tree);

template <
class ValueType,
bool ownedValues = false
>
class NameSearchTree
{
public:
	typedef void(*free)(ValueType &value);
	typedef NameSearchWalker<ValueType> iterator;
	/** Default constructor */
	NameSearchTree() : root(NULL), freeFunction(NULL), count(0) {}
	NameSearchTree(free freeFunction) : root(NULL), freeFunction(freeFunction), count(0) {}
	/** Default destructor */
	virtual ~NameSearchTree()
	{
		this->recursiveDelete(this->root);
	}
	bool isset(const char *name) const throw(NotFoundException)
	{
		NameSearchTreeNodes::NodeBase<ValueType> *node = this->findNode(name);
		return ((node != NULL) && (node->isLeaf()));
	}
	ValueType &get(const char *name) const throw(NotFoundException)
	{
		NameSearchTreeNodes::NodeBase<ValueType> *node = this->findNode(name);
		if ((node == NULL) || (!node->isLeaf()))
		{
			throw NotFoundException();
		}
		return static_cast<NameSearchTreeNodes::Leaf<ValueType>*>(node)->value;
	}
	bool add(const char *name, ValueType &value)
	{
		return this->addNode(name, value);
	}
	bool remove(const char *name)
	{
		return this->removeNode(name);
	}
	iterator begin() const
	{
		iterator it(this->root);
		return it;
	}
	iterator end() const
	{
		static iterator it(NULL);
		return it;
	}
	int size() const { return this->count; }
	friend std::ostream& operator << <>(std::ostream&, const NameSearchTree<ValueType>&);
protected:
private:
	NameSearchTreeNodes::NodeBase<ValueType> *root;
	free freeFunction;
	int count;

	void recursiveDelete(NameSearchTreeNodes::NodeBase<ValueType> *node)
	{
		NameSearchTreeNodes::NodeBase<ValueType> *previous;
		while (node != NULL)
		{
			delete[] node->name;
			if (node->isLeaf())
			{
				if (ownedValues && (freeFunction != NULL))
				{
					this->freeFunction(static_cast<NameSearchTreeNodes::Leaf<ValueType>*>(node)->value);
				}
			}
			else
			{
				this->recursiveDelete(static_cast<NameSearchTreeNodes::Node<ValueType>*>(node)->child);
			}
			previous = node;
			node = node->next;
			delete previous;
		}
	}
	NameSearchTreeNodes::NodeBase<ValueType> * findNode(const char *name)
	{
		NameSearchTreeNodes::NodeBase<ValueType> *where = this->root;
		while (where != NULL)
		{
			int match = this->matchStart(name, where->name);
			if (match == 0)
			{
				where = where->next;
			}
			else if (match > 0)
			{
				if (name[match] != 0)
				{
					if (!where->isLeaf())
					{
						where = static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child;
						name += match;
					}
					else
					{
						where = NULL;
					}
				}
				else if (!where->isLeaf())
				{
					if ((static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child != NULL) &&
							(static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child->name[0] == 0) &&
							(static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child->isLeaf()))
					{
						where = static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child;
					}
					break;
				}
			}
			else
			{
				where = NULL;
			}
		}
		return where;
	}
	/**
	 * Searches what in where.
	 * @return int n>0 => what is the start of where. n<=0 => (-n) is the number of matching character.
	 */
	int matchStart(const char *where, const char *what)
	{
		int index = 0;
		while ((*what != 0) && (*where != 0) && (*what == *where))
		{
			++index;
			++what;
			++where;
		}
		if (*what == 0)
		{
			return index;
		}
		return -index;
	}
	bool addNode(const char *name, ValueType &value)
	{
#define CONSUME_LEVEL(chars) parent = where; where = &(static_cast<NameSearchTreeNodes::Node<ValueType>*>(*where)->child); name += (chars)
		NameSearchTreeNodes::NodeBase<ValueType> **parent = NULL;
		NameSearchTreeNodes::NodeBase<ValueType> **where = &this->root;
		while (*where != NULL)
		{
			int match = this->matchStart(name, (*where)->name);
			if (match == 0)
			{
				where = &((*where)->next);
			}
			else if (match > 0)
			{
				if (name[match] != 0)
				{
					if ((*where)->isLeaf())
					{
						*where = this->splitNode(*where, match);
						CONSUME_LEVEL(match);
						break;
					}
					CONSUME_LEVEL(match);
				}
				else
				{
					if ((*where)->isLeaf())
					{
						return false;
					}
					else
					{
						if ((static_cast<NameSearchTreeNodes::Node<ValueType>*>(*where)->child != NULL) &&
								(static_cast<NameSearchTreeNodes::Node<ValueType>*>(*where)->child->name[0] == 0) &&
								(static_cast<NameSearchTreeNodes::Node<ValueType>*>(*where)->child->isLeaf()))
						{
							return false;
						}
					}
					CONSUME_LEVEL(match);
					break;
				}
			}
			else
			{
				*where = this->splitNode(*where, -match);
				CONSUME_LEVEL(-match);
				break;
			}
		}
#undef CONSUME_LEVEL
		this->createNode(name, value, parent);
		return true;
	}
	void createNode(const char *name, ValueType value, NameSearchTreeNodes::NodeBase<ValueType> **parent)
	{
		NameSearchTreeNodes::NodeBase<ValueType> **where = NULL;
		NameSearchTreeNodes::Leaf<ValueType> *newNode = new NameSearchTreeNodes::Leaf<ValueType>();
		char *remaining = new char[strlen(name) + 1];
		strcpy(remaining, name);
		newNode->name = remaining;
		newNode->value = value;
		if (parent == NULL)
		{
			where = &this->root;
		}
		else
		{
			where = &(static_cast<NameSearchTreeNodes::Node<ValueType>*>(*parent)->child);
		}
		while ((*where != NULL) && (strcmp((*where)->name, newNode->name) < 0))
		{
			where = &((*where)->next);
		}
		newNode->next = *where;
		*where = newNode;
		this->count++;
	}
	NameSearchTreeNodes::NodeBase<ValueType> *splitNode(NameSearchTreeNodes::NodeBase<ValueType> *which, int at)
	{
		NameSearchTreeNodes::Node<ValueType> *newNode = new NameSearchTreeNodes::Node<ValueType>();
		newNode->next = which->next;
		which->next = NULL;
		char *tmp = new char[at + 1];
		strncpy(tmp, which->name, at);
		tmp[at] = 0;
		newNode->name = tmp;
		tmp = new char[strlen(which->name) - at + 1];
		strcpy(tmp, which->name + at);
		delete[] which->name;
		which->name = tmp;
		newNode->child = which;
		return newNode;
	}
	bool removeNode(const char *name)
	{
		NameSearchTreeNodes::NodeBase<ValueType> **where = &this->root;
		while (*where != NULL)
		{
			int match = this->matchStart(name, (*where)->name);
			if (match == 0)
			{
				where = &((*where)->next);
			}
			else if (match > 0)
			{
				if (name[match] != 0)
				{
					if (!(*where)->isLeaf())
					{
						where = &(static_cast<NameSearchTreeNodes::Node<ValueType>*>(*where)->child);
						name += match;
					}
					else
					{
						break;
					}
				}
				else if (!(*where)->isLeaf())
				{
					if ((static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child != NULL) &&
							(static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child->name[0] == 0) &&
							(static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child->isLeaf()))
					{
						where = static_cast<NameSearchTreeNodes::Node<ValueType>*>(where)->child;
					}
					else
					{
						break;
					}
				}
				if ((*where)->isLeaf())
				{
					delete [] (*where)->name;
					if (ownedValues && (freeFunction != NULL))
					{
						this->freeFunction(static_cast<NameSearchTreeNodes::Leaf<ValueType>*>(*where)->value);
					}
					NameSearchTreeNodes::NodeBase<ValueType> *next = (*where)->next;
					delete *where;
					*where = next;
					this->count--;
					return true;
				}
			}
			else
			{
				break;
			}
		}
		return false;
	}
};


template <
class ValueType,
bool ownedValues
>
std::ostream& operator << (std::ostream& out, const NameSearchTree<ValueType, ownedValues>& tree)
{
	using namespace NameSearchTreeNodes;
	if (tree.root == NULL)
	{
		out << "(empty)" << std::endl;
	}
	else
	{
		out << "Root" << indent_inc << std::endl << *tree.root;
	}
	return out;
}

#endif // NAMESEARCHTREE_H
