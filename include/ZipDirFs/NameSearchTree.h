/*
 * Copyright © 2012-2019 Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
 */
#ifndef ZIPDIRFS_NAMESEARCHTREE_H
#define ZIPDIRFS_NAMESEARCHTREE_H

#include "ZipDirFs/NameSearchTreeNodes.h"
#include "ZipDirFs/NameSearchWalker.h"
#include "ZipDirFs/NotFoundException.h"
#include "ZipDirFs/stream_indent.h"
#include <cstring>
#include <ostream>

namespace ZipDirFs
{
	template < class ValueType, bool ownedValues >
	class NameSearchTree;

	template < class ValueType, bool ownedValues >
	std::ostream& operator << (std::ostream& out, const NameSearchTree<ValueType, ownedValues>& tree);

	/**
	 * \brief Represents a search tree for a name-value pair.
	 * The tree contains two node type: Node and Leaf.
	 * Each node type contains the name part for which it match and a link to the next node.
	 * A Node also contains a link to its child Node. A Leaf also contains the value.
	 * In each branches, Node and Leaf are sorted by name part.
	 *
	 * Using this structure, looking up an entry cost less than searching in an unordered list,
	 * but a little more(?) than searching in an ordered vector.
	 * It also cost more than just adding on top of an unordered list, but it does not have also
	 * the hassle of reallocating an array when capacity is reached neither the hassle of inserting
	 * in the middle of a vector.
	 * Removing is also as efficient as adding.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class ValueType, bool ownedValues = false >
	class NameSearchTree
	{
	public:
		typedef void (*free)(ValueType value);
		typedef NameSearchWalker<ValueType> iterator;
		typedef NameSearchTreeNodes::NodeBase<ValueType> nodebase_type;
		typedef NameSearchTreeNodes::Node<ValueType> node_type;
		typedef NameSearchTreeNodes::Leaf<ValueType> leaf_type;
		/** Default constructor */
		NameSearchTree() : root(NULL), freeFunction(NULL), count(0) {}
		NameSearchTree(free freeFunction) : root(NULL), freeFunction(freeFunction), count(0) {}
		/** Default destructor */
		virtual ~NameSearchTree()
		{
			this->recursiveDelete(this->root);
		}
		/**
		 * \brief Removes all entries from the tree.
		 */
		void clear()
		{
			nodebase_type* oldroot = this->root;
			this->root = NULL;
			this->recursiveDelete(oldroot);
		}
		/**
		 * \brief Tests if an entry exists.
		 * \param name The name of the searched entry.
		 * \return `true` if found. `false` otherwise.
		 */
		bool isset(const char* name) const
		{
			nodebase_type* node = this->findNode(name);
			return ((node != NULL) && (node->isLeaf()));
		}
		/**
		 * \brief Retrieves an entry.
		 * \param name The name of the searched entry.
		 * \return The searched entry's value.
		 * \throw NotFoundException If the entry is not found.
		 */
		ValueType& get(const char* name) const
		{
			nodebase_type* node = this->findNode(name);

			if ((node == NULL) || (!node->isLeaf()))
			{
				throw NotFoundException();
			}

			return static_cast<leaf_type*>(node)->value;
		}
		/**
		 * \brief Adds en entry to the tree.
		 * If the entry already exists, it fails.
		 * \param name The name of the entry.
		 * \param value The value of the entry.
		 * \return `true` on success`. `false` on failure.
		 */
		bool add(const char* name, ValueType& value)
		{
			return this->addNode(name, value);
		}
		/**
		 * \brief Removes an entry from the tree.
		 * If the entry does not exists, it fails.
		 * \param name The name of the entry.
		 * \return `true` on success. `false` on failure.
		 */
		bool remove(const char* name)
		{
			return this->removeNode(name);
		}
		/**
		 * \brief Retrieves the iterator to the beginning of the tree.
		 * \return The iterator positionned at the beginning of the tree.
		 */
		iterator begin() const
		{
			iterator it(this->root);
			return it;
		}
		/**
		 * \brief Retrieves the iterator to the end of the tree.
		 * \return The iterator to the end of the tree.
		 */
		iterator end() const
		{
			static iterator it(NULL);
			return it;
		}
		/**
		 * \brief Retrieves the number of elements in the tree.
		 * \return The number of elements in the tree.
		 */
		int size() const
		{
			return this->count;
		}
		friend std::ostream& operator << <> (std::ostream&, const NameSearchTree<ValueType>&);
	protected:
	private:
		nodebase_type* root;
		free freeFunction;
		int count;

		/**
		 * \brief Frees a branch.
		 * \param node The node to start freeing from.
		 */
		void recursiveDelete(nodebase_type* node)
		{
			nodebase_type* previous;

			while (node != NULL)
			{
				delete[] node->name;

				if (node->isLeaf())
				{
					if (ownedValues && (freeFunction != NULL))
					{
						this->freeFunction(static_cast<leaf_type*>(node)->value);
					}
				}
				else
				{
					this->recursiveDelete(static_cast<node_type*>(node)->child);
				}

				previous = node;
				node = node->next;
				delete previous;
			}
		}
		/**
		 * \brief Looks up for a node.
		 * \param name The name of the node to search.
		 * \return The found node. `NULL` if none.
		 */
		nodebase_type* findNode(const char* name) const
		{
			nodebase_type* where = this->root;

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
							where = static_cast<node_type*>(where)->child;
							name += match;
						}
						else
						{
							where = NULL;
						}
					}
					else
					{
						if (!where->isLeaf())
						{
							if ((static_cast<node_type*>(where)->child != NULL) &&
								(static_cast<node_type*>(where)->child->name[0] == 0) &&
								(static_cast<node_type*>(where)->child->isLeaf()))
							{
								where = static_cast<node_type*>(where)->child;
							}
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
		 * \brief Searches what in where.
		 * \param where The string to look in.
		 * \param what The string to look for.
		 * \return `n>0` => what is the start of where. `n<=0`s => (-n) is the number of matching character.
		 */
		int matchStart(const char* where, const char* what) const
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
		/**
		 * \brief Really adds an entry.
		 * It fails if the entry already exists.
		 * All needed nodes are created in the process.
		 * \param name The name of the entry to add.
		 * \param value The value of the entry to add.
		 * \return `true` on success. `false` on failure.
		 */
		bool addNode(const char* name, ValueType& value)
		{
#define CONSUME_LEVEL(chars) parent = where; where = & (static_cast<node_type*> (*where)->child); name += (chars)
			nodebase_type** parent = NULL;
			nodebase_type** where = &this->root;

			while (*where != NULL)
			{
				int match = this->matchStart(name, (*where)->name);

				if (match == 0)
				{
					where = & ((*where)->next);
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
							if ((static_cast<node_type*>(*where)->child != NULL) &&
								(static_cast<node_type*>(*where)->child->name[0] == 0) &&
								(static_cast<node_type*>(*where)->child->isLeaf()))
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
		/**
		 * \brief Creates a Leaf in a branch.
		 * \param name The name part of the Leaf.
		 * \param value The value of the Leaf.
		 * \param parent The pointer to the field containing the parent Node.
		 */
		void createNode(const char* name, ValueType& value, nodebase_type** parent)
		{
			nodebase_type** where = NULL;
			leaf_type* newNode = new leaf_type();
			char* remaining = new char[strlen(name) + 1];
			::strcpy(remaining, name);
			newNode->name = remaining;
			newNode->value = value;

			if (parent == NULL)
			{
				where = &this->root;
			}
			else
			{
				where = & (static_cast<node_type*>(*parent)->child);
			}

			while ((*where != NULL) && (strcmp((*where)->name, newNode->name) < 0))
			{
				where = & ((*where)->next);
			}

			newNode->next = *where;
			*where = newNode;
			this->count++;
		}
		/**
		 * \brief Creates a branch from a node.
		 * The name part of the original node is cut to create the new Node name part with the left string
		 * and replace the first node name part with the right string.
		 * \param which The Node or Leaf to move into a branch.
		 * \param at The position at which to cut the name part of the node.
		 * \return The newly created Node containing the passed node as a child.
		 */
		nodebase_type* splitNode(nodebase_type* which, int at) const
		{
			node_type* newNode = new node_type();
			newNode->next = which->next;
			which->next = NULL;
			char* tmp = new char[at + 1];
			::strncpy(tmp, which->name, at);
			tmp[at] = 0;
			newNode->name = tmp;
			tmp = new char[strlen(which->name) - at + 1];
			::strcpy(tmp, which->name + at);
			delete[] which->name;
			which->name = tmp;
			newNode->child = which;
			return newNode;
		}
		/**
		 * \brief Really removes an entry.
		 * If the entry is not found, it fails.
		 * \param name The name of the entry.
		 * \return `true` on success. `false` on failure.
		 */
		bool removeNode(const char* name)
		{
			nodebase_type** where = &this->root;

			while (*where != NULL)
			{
				int match = this->matchStart(name, (*where)->name);

				if (match == 0)
				{
					where = & ((*where)->next);
				}
				else if (match > 0)
				{
					if (name[match] != 0)
					{
						if (!(*where)->isLeaf())
						{
							where = & (static_cast<node_type*>(*where)->child);
							name += match;
							continue;
						}
						else
						{
							break;
						}
					}
					else if (!(*where)->isLeaf())
					{
						if ((static_cast<node_type*>(*where)->child != NULL) &&
							(static_cast<node_type*>(*where)->child->name[0] == 0) &&
							(static_cast<node_type*>(*where)->child->isLeaf()))
						{
							where = & (static_cast<node_type*>(*where)->child);
						}
						else
						{
							break;
						}
					}

					if ((*where)->isLeaf())
					{
						delete [](*where)->name;

						if (ownedValues && (freeFunction != NULL))
						{
							this->freeFunction(static_cast<leaf_type*>(*where)->value);
						}

						nodebase_type* next = (*where)->next;
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

	/**
	 * \brief Dumps the tree nodes to an output stream.
	 * If the tree is empty, the string `(empty)` is outputted. Otherwise `Root` and a newline is outputted followed by the nodes.
	 * In all cases, the output is terminated by a newline.
	 *
	 * Stream helper methods are defined in stream_indent.h.
	 * \param out The output stream.
	 * \param tree The tree to dump.
	 * \return The output stream.
	 * \author Pierrick Caillon <pierrick.caillon+zipdirfs@megami.fr>
	 */
	template < class ValueType, bool ownedValues >
	std::ostream& operator << (std::ostream& out, const NameSearchTree<ValueType, ownedValues>& tree)
	{
		using namespace NameSearchTreeNodes;

		if (tree.root == NULL)
		{
			out << "(empty)" << std::endl;
		}
		else
		{
			out << "Root" << std::endl << indent_inc << *tree.root << indent_dec;
		}

		return out;
	}
} // namespace ZipDirFs

#endif // ZIPDIRFS_NAMESEARCHTREE_H
