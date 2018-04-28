#pragma once
#ifndef RBTREE_H  
#define RBTREE_H  

#include <assert.h>  
#include <string.h>  
#include <stdlib.h>  

template<typename Key, typename Value>
class Cmap
{
#pragma region rbnode  

	struct _rbnode
	{
		typedef enum {
			_red = 0,
			_black,
		}eColor;

		Key key;
		Value value;
		eColor color;

		_rbnode* right;
		_rbnode* left;
		_rbnode* parent;
	};

#pragma endregion  

private:
#pragma region rbnode pool  

	class _pool
	{
	public:
		_pool() : count(0)
		{

		}

		~_pool()
		{

		}

		int init(unsigned cnt)
		{
			count = cnt;
			memset(&buffer, 0, sizeof(__array));
			memset(&pool, 0, sizeof(__array));
			return increase();
		}

		int uninit()
		{
			for (unsigned i = 0; i < buffer.count; ++i)
			{
				void* buff = (void*)buffer[i];
				if (buff)
				{
					::free(buff);
					buff = 0;
				}
			}

			return true;
		}

		int increase()
		{
			_rbnode* buff = (_rbnode*)malloc(sizeof(_rbnode) * count);
			if (!buff)
				return false;
			buffer.push((intptr_t)buff);
			for (unsigned i = 0; i < count; ++i)
			{
				pool.push((intptr_t)buff);
				buff += 1;
			}
			return true;
		}

		_rbnode* alloc()
		{
			if (pool.count == 0)
			{
				if (!increase())
				{
					return 0;
				}
			}
			_rbnode* ret = (_rbnode*)pool[pool.count - 1];
			pool.count--;
			return ret;
		}

		void free(void* ptr)
		{
			if (ptr)
				pool.push((intptr_t)ptr);
		}

		struct __array
		{
			unsigned count;
			unsigned capacity;
			intptr_t* buffer;

			intptr_t operator[](const unsigned idx)
			{
				if (idx > count)
					return 0;
				return *(buffer + idx);
			}

			intptr_t push(intptr_t ptr)
			{
				if (capacity == count)
				{
					if (!increase())
						return 0;
				}
				buffer[count++] = ptr;
				return ptr;
			}

			int increase()
			{
				unsigned total = capacity << 1;
				if (total == 0)
					total = 4;

				unsigned char * tmp = (unsigned char *)malloc(total * sizeof(intptr_t));
				if (!tmp)
					return false;

				for (int i = 0; i < count; i++)
					*((intptr_t*)tmp + i) = *(buffer + i);

				::free(buffer);

				buffer = (intptr_t*)tmp;
				capacity = total;

				return true;
			}
		};
	private:
		__array buffer;
		__array pool;

		int count;
	};
#pragma endregion  

public:
#pragma region iterator  

	class Iterator
	{
		_rbnode* m_pNode;

	public:
		Iterator() : m_pNode(0) {}
		Iterator(_rbnode* rhs)
		{
			if (rhs == 0)
			{
				m_pNode = 0;
				return;
			}

			m_pNode = rhs;
		}
		Iterator& operator=(_rbnode* rhs)
		{
			if (rhs == 0)
			{
				m_pNode = 0;
				return *this;
			}

			m_pNode = rhs;
			return *this;
		}
		Iterator& operator=(const Iterator& rhs)
		{
			m_pNode = rhs.m_pNode;

			return *this;
		}
		Iterator& operator++()
		{
			m_pNode = NextTreeNode(m_pNode);

			if (m_pNode == 0) return *this;

			return *this;
		}
		Iterator& operator++(int)
		{
			m_pNode = NextTreeNode(m_pNode);

			return *this;
		}
		Iterator& operator--()
		{
			m_pNode = PrevTreeNode(m_pNode);

			return *this;
		}
		Iterator& operator--(int)
		{
			m_pNode = PrevTreeNode(m_pNode);

			return *this;
		}

		bool operator==(const Iterator& rhs)
		{
			return m_pNode == rhs.m_pNode;
		}

		bool operator!=(const Iterator& rhs)
		{
			return m_pNode != rhs.m_pNode;
		}

		bool operator!()
		{
			if (m_pNode == NULL)
				return true;
			return false;
		}

	public:
		Key GetKey() const
		{
			if (m_pNode == 0)
				return 0;
			return m_pNode->key;
		}
		Value GetValue() const
		{
			if (m_pNode == 0)
				return 0;
			return m_pNode->value;
		}
	};
#pragma endregion  

public:
	Cmap() : root(0)
	{
		rbnodeAllocator.init(128);
	}
	Cmap(unsigned uCountInLine) : root(0)
	{
		rbnodeAllocator.init(uCountInLine);
	}
	~Cmap()
	{
		rbnodeAllocator.uninit();
		root = 0;
	}

	Iterator find(const Key& key)
#pragma region find  
	{
		_rbnode* pNode = root;
		Iterator itr;
		if (root == 0)
			return itr;

		while (pNode)
		{
			if (key == pNode->key)
			{
				itr = pNode;
				break;
			}
			else if (key > pNode->key)
				pNode = pNode->right;
			else
				pNode = pNode->left;
		}
		return itr;
	}
#pragma endregion  

	Iterator set(const Key& key, const Value& value)
#pragma region set  
	{
		_rbnode * pSearchNode = root;
		_rbnode * pInsertedNode;
		_rbnode * pCurrent;

		Iterator itr;

		if (root == 0)
		{
			root = (_rbnode*)rbnodeAllocator.alloc();

			memset(root, 0, sizeof(_rbnode));
			root->color = _rbnode::eColor::_black;
			root->key = key;
			root->value = value;

			itr = root;

			return itr;
		}

		while (1)
		{
			if (key == pSearchNode->key)
			{
				pSearchNode->value = value;
				return itr;
			}
			else if (key < pSearchNode->key)
			{
				if (pSearchNode->left == 0)
				{
					pInsertedNode = (_rbnode *)rbnodeAllocator.alloc();

					pInsertedNode->color = _rbnode::eColor::_red;
					pInsertedNode->right = 0;
					pInsertedNode->left = 0;
					pInsertedNode->parent = pSearchNode;

					pInsertedNode->key = key;

					pInsertedNode->value = value;

					pSearchNode->left = pInsertedNode;
					break;
				}
				else
					pSearchNode = pSearchNode->left;
			}
			else
			{
				if (pSearchNode->right == 0)
				{
					pInsertedNode = (_rbnode *)rbnodeAllocator.alloc();

					pInsertedNode->color = _rbnode::eColor::_red;
					pInsertedNode->right = 0;
					pInsertedNode->left = 0;
					pInsertedNode->parent = pSearchNode;

					pInsertedNode->key = key;
					pInsertedNode->value = value;

					pSearchNode->right = pInsertedNode;
					break;
				}
				else
					pSearchNode = pSearchNode->right;
			}
		}

		pCurrent = pInsertedNode;
		itr = pInsertedNode;

		while (1)
		{
			_rbnode * pParent;
			_rbnode * pGrandParent;
			_rbnode * pUncle;

			if (pCurrent->parent == 0)
			{
				if (pCurrent->color == _rbnode::eColor::_red)
					pCurrent->color = _rbnode::eColor::_black;

				root = pCurrent;

				return itr;
			}

			pParent = pCurrent->parent;
			if (pParent->color == _rbnode::eColor::_black)
				return itr;

			pGrandParent = pParent->parent;

			if (pGrandParent->left == pParent)
			{
				pUncle = pGrandParent->right;

				if (!pUncle || pUncle->color == _rbnode::eColor::_black)
				{
					if (pCurrent == pParent->left)
					{
						pParent->color = _rbnode::eColor::_black;
						pGrandParent->color = _rbnode::eColor::_red;

						RightRotate(&pGrandParent);

						if (pParent->parent == 0)
							root = pParent;
					}
					else
					{
						LeftRotate(&pParent);

						pCurrent->color = _rbnode::eColor::_black;
						pGrandParent->color = _rbnode::eColor::_red;

						RightRotate(&pGrandParent);

						if (pCurrent->parent == 0)
							root = pCurrent;
					}

					return itr;
				}
			}
			else
			{
				pUncle = pGrandParent->left;

				if (!pUncle || pUncle->color == _rbnode::eColor::_black)
				{
					if (pCurrent == pParent->right)
					{
						pParent->color = _rbnode::eColor::_black;
						pGrandParent->color = _rbnode::eColor::_red;

						LeftRotate(&pGrandParent);

						if (pParent->parent == 0)
							root = pParent;
					}
					else
					{
						RightRotate(&pParent);

						pCurrent->color = _rbnode::eColor::_black;
						pGrandParent->color = _rbnode::eColor::_red;

						LeftRotate(&pGrandParent);

						if (pCurrent->parent == 0)
							root = pCurrent;
					}

					return itr;
				}
			}

			if (pUncle && pUncle->color == _rbnode::eColor::_red)
			{
				pParent->color = _rbnode::eColor::_black;
				pUncle->color = _rbnode::eColor::_black;
				pGrandParent->color = _rbnode::eColor::_red;

				pCurrent = pGrandParent;
			}
		}
		return itr;
	}
#pragma endregion  

	Iterator erase(Key key)
#pragma region erase  
	{
		_rbnode * pDeleteNode = root;
		_rbnode * pRealDeleteNode;

		_rbnode * pCurrent;
		_rbnode * pParent;

		Iterator itr;

		if (root == 0)
			return itr;

		while (1)
		{
			if (key == pDeleteNode->key)
				break;
			else if (key < pDeleteNode->key)
			{
				if (pDeleteNode->left == 0)
					return itr;
				else
					pDeleteNode = pDeleteNode->left;
			}
			else
			{
				if (pDeleteNode->right == 0)
					return itr;
				else
					pDeleteNode = pDeleteNode->right;
			}
		}

		if (pDeleteNode->right == 0)
			pRealDeleteNode = pDeleteNode;
		else
		{
			pRealDeleteNode = pDeleteNode->right;

			while (1)
			{
				if (pRealDeleteNode->left == 0)
					break;
				else
					pRealDeleteNode = pRealDeleteNode->left;
			}
		}

		pDeleteNode->key = pRealDeleteNode->key;
		pDeleteNode->value = pRealDeleteNode->value;

		itr = NextTreeNode(pDeleteNode);

		pCurrent = pRealDeleteNode;
		pParent = pCurrent->parent;

		if ((pParent == 0) && (pCurrent->left == 0) && (pCurrent->right == 0))
		{
			root = 0;

			rbnodeAllocator.free(pCurrent);

			return itr;
		}

		if (pCurrent->color == _rbnode::eColor::_red)
		{
			if (pParent->left == pCurrent)
				pParent->left = 0;
			else
				pParent->right = 0;

			rbnodeAllocator.free(pCurrent);

			return itr;
		}

		if (pCurrent->right != 0)
		{
			if (pCurrent->right->color == _rbnode::eColor::_red)
			{
				pCurrent->key = pCurrent->right->key;

				rbnodeAllocator.free(pCurrent->right);

				pCurrent->right = 0;

				return itr;
			}
			else
			{
				//printf("unscienceble!!\n");  
				//return 1;  
				return itr;
			}
		}
		else if (pCurrent->left != 0)
		{
			pCurrent->key = pCurrent->left->key;

			rbnodeAllocator.free(pCurrent->left);

			pCurrent->left = 0;
			//return 0;  
			return itr;
		}
		else        //左右子节点为空，颜色为黑的节点  
		{
			while (1)
			{
				if (pCurrent == pParent->left)       //左孩子  
				{
					_rbnode * pSibling = pParent->right;

					if (pSibling->color == _rbnode::eColor::_black)      //兄弟节点为黑  
					{
						_rbnode * sibling_left = pSibling->left;
						_rbnode * sibling_right = pSibling->right;

						if (sibling_right && sibling_right->color == _rbnode::eColor::_red)//兄弟右子节点为红  
						{
							pSibling->color = pParent->color;
							sibling_right->color = _rbnode::eColor::_black;
							pParent->color = _rbnode::eColor::_black;

							LeftRotate(&pParent);

							if (pParent->parent)
							{
								if (pParent->parent->parent == nullptr)
									root = pSibling;
							}

							pParent->left = 0;

							rbnodeAllocator.free(pCurrent);

							//return 0;  
							return itr;
						}

						if (sibling_left && sibling_left->color == _rbnode::eColor::_red)//兄弟左子节点为红  
						{
							RightRotate(&pSibling);

							pSibling = pParent->right;

							pSibling->color = pParent->color;
							pParent->color = _rbnode::eColor::_black;

							LeftRotate(&pParent);

							pParent->left = 0;

							if (pParent->parent)
							{
								if (pParent->parent->parent == nullptr)
									root = pParent->parent;
							}

							rbnodeAllocator.free(pCurrent);

							//return 0;  
							return itr;
						}

						if (pParent->color == _rbnode::eColor::_red)//父节点为红  
						{
							pSibling->color = _rbnode::eColor::_red;
							pParent->color = _rbnode::eColor::_black;

							pParent->left = 0;

							rbnodeAllocator.free(pCurrent);

							//return 0;  
							return itr;
						}
						else//父节点为黑，兄弟两个子节点为黑  
						{
							pSibling->color = _rbnode::eColor::_red;

							pParent->left = 0;

							rbnodeAllocator.free(pCurrent);

							pCurrent = pParent;
							pParent = pCurrent->parent;

							while (1)
							{
								if (pParent == 0)
									break;

								if (pCurrent == pParent->left)//当前节点为左子节点  
								{
									pSibling = pParent->right;

									if (pSibling->color == _rbnode::eColor::_black)
									{
										sibling_left = pSibling->left;
										sibling_right = pSibling->right;

										if (sibling_right && sibling_right->color == _rbnode::eColor::_red)
										{
											pSibling->color = pParent->color;
											sibling_right->color = _rbnode::eColor::_black;
											pParent->color = _rbnode::eColor::_black;

											LeftRotate(&pParent);

											if (pParent->parent)
											{
												if (pParent->parent->parent == nullptr)
													root = pParent->parent;
											}

											break;
										}

										if (sibling_left && sibling_left->color == _rbnode::eColor::_red)
										{
											RightRotate(&pSibling);

											pSibling = pParent->right;

											pSibling->color = pParent->color;
											pParent->color = _rbnode::eColor::_black;

											LeftRotate(&pParent);

											if (pParent->parent)
											{
												if (pParent->parent->parent == nullptr)
													root = pParent->parent;
											}

											break;
										}

										if (pParent->color == _rbnode::eColor::_red)
										{
											pSibling->color = _rbnode::eColor::_red;
											pParent->color = _rbnode::eColor::_black;

											break;
										}
										else//黑兄二黑侄  
										{
											pSibling->color = _rbnode::eColor::_red;
											pParent->color = pCurrent->color;
											pCurrent->color = _rbnode::eColor::_black;

											if (pParent->parent == nullptr)
												pParent->color = _rbnode::eColor::_black;

											pCurrent = pParent;
											pParent = pCurrent->parent;
											continue;
										}
									}
									else//兄弟节点为红色  
									{
										pParent->color = _rbnode::eColor::_red;
										pSibling->color = _rbnode::eColor::_black;

										LeftRotate(&pParent);

										if (pParent->parent)
										{
											if (pParent->parent->parent == nullptr)
												root = pParent->parent;
										}
										continue;
									}
								}
								else//当前节点为右子节点  
								{
									pSibling = pParent->left;

									if (pSibling->color == _rbnode::eColor::_black)
									{
										_rbnode * pSiblingLeftChild = pSibling->left;
										_rbnode * pSiblingRightChild = pSibling->right;

										if (pSiblingLeftChild && pSiblingLeftChild->color == _rbnode::eColor::_red)
										{
											pSibling->color = pParent->color;
											pSiblingLeftChild->color = _rbnode::eColor::_black;
											pParent->color = _rbnode::eColor::_black;

											RightRotate(&pParent);

											if (pParent->parent)
											{
												if (pParent->parent->parent == nullptr)
													root = pParent->parent;
											}

											break;
										}

										if (pSiblingRightChild && pSiblingRightChild->color == _rbnode::eColor::_red)
										{
											LeftRotate(&pSibling);

											pSibling = pParent->left;

											pSibling->color = pParent->color;
											pParent->color = _rbnode::eColor::_black;

											RightRotate(&pParent);

											if (pParent->parent)
											{
												if (pParent->parent->parent == nullptr)
													root = pParent->parent;
											}

											break;
										}

										if (pParent->color == _rbnode::eColor::_red)
										{
											pSibling->color = _rbnode::eColor::_red;
											pParent->color = _rbnode::eColor::_red;

											break;
										}
										else
										{
											pSibling->color = _rbnode::eColor::_red;
											pParent->color = pCurrent->color;
											pCurrent->color = _rbnode::eColor::_black;

											pCurrent = pParent;
											pParent = pCurrent->parent;
											continue;
										}
									}
									else//兄弟节点为红色  
									{
										pParent->color = _rbnode::eColor::_red;
										pSibling->color = _rbnode::eColor::_black;

										RightRotate(&pParent);

										if (pParent->parent)
										{
											if (pParent->parent->parent == nullptr)
												root = pParent->parent;
										}
										continue;
									}
								}
							}
							return itr;
						}
					}
					else            //兄弟节点为红  
					{
						pParent->color = _rbnode::eColor::_red;
						pSibling->color = _rbnode::eColor::_black;

						LeftRotate(&pParent);

						if (pParent->parent)
						{
							if (pParent->parent->parent == nullptr)
								root = pParent->parent;
						}

						continue;
					}
				}
				else        //右孩子  
				{
					_rbnode * pSibling = pParent->left;

					if (pSibling->color == _rbnode::eColor::_black)      //兄弟节点为黑  
					{
						_rbnode * pSiblingLeftChild = pSibling->left;
						_rbnode * pSiblingRightChild = pSibling->right;

						if (pSiblingLeftChild && pSiblingLeftChild->color == _rbnode::eColor::_red)//兄弟左子节点为红  
						{
							pSibling->color = pParent->color;
							pSiblingLeftChild->color = _rbnode::eColor::_black;
							pParent->color = _rbnode::eColor::_black;

							RightRotate(&pParent);

							pParent->right = 0;

							if (pParent->parent)
							{
								if (pParent->parent->parent == nullptr)
									root = pSibling;
							}

							rbnodeAllocator.free(pCurrent);

							//return 0;  
							return itr;
						}

						if (pSiblingRightChild && pSiblingRightChild->color == _rbnode::eColor::_red)//兄弟右子节点为红  
						{
							LeftRotate(&pSibling);

							pSibling = pParent->left;

							pSibling->color = pParent->color;
							pParent->color = _rbnode::eColor::_black;

							RightRotate(&pParent);

							pParent->right = 0;

							if (pParent->parent)
							{
								if (pParent->parent->parent == nullptr)
									root = pParent->parent;
							}

							rbnodeAllocator.free(pCurrent);

							//return 0;  
							return itr;
						}


						if (pParent->color == _rbnode::eColor::_red)//父节点为红  
						{
							pSibling->color = _rbnode::eColor::_red;
							pParent->color = _rbnode::eColor::_black;

							pParent->right = 0;

							rbnodeAllocator.free(pCurrent);

							//return 0;  
							return itr;
						}
						else//父节点为黑，兄弟两个子节点为黑  
						{
							pSibling->color = _rbnode::eColor::_red;

							pParent->right = 0;

							rbnodeAllocator.free(pCurrent);

							pCurrent = pParent;
							pParent = pCurrent->parent;

							while (1)
							{
								if (pParent == 0)
									break;

								if (pCurrent == pParent->left)//当前节点为父节点左孩子  
								{
									pSibling = pParent->right;

									if (pSibling->color == _rbnode::eColor::_black)
									{
										pSiblingLeftChild = pSibling->left;
										pSiblingRightChild = pSibling->right;

										if (pSiblingRightChild && pSiblingRightChild->color == _rbnode::eColor::_red)
										{
											pSibling->color = pParent->color;
											pSiblingRightChild->color = _rbnode::eColor::_black;
											pParent->color = _rbnode::eColor::_black;

											LeftRotate(&pParent);

											if (pParent->parent)
											{
												if (pParent->parent->parent == nullptr)
													root = pParent->parent;
											}

											break;
										}

										if (pSiblingLeftChild && pSiblingLeftChild->color == _rbnode::eColor::_red)
										{
											RightRotate(&pSibling);

											pSibling = pParent->right;

											pSibling->color = pParent->color;
											pParent->color = _rbnode::eColor::_black;

											LeftRotate(&pParent);

											if (pParent->parent)
											{
												if (pParent->parent->parent == nullptr)
													root = pParent->parent;
											}

											break;
										}

										if (pParent->color == _rbnode::eColor::_red)
										{
											pSibling->color = _rbnode::eColor::_red;
											pParent->color = _rbnode::eColor::_black;

											break;
										}
										else
										{
											pSibling->color = _rbnode::eColor::_red;
											pParent->color = pCurrent->color;
											pCurrent->color = _rbnode::eColor::_black;

											if (pParent->parent == nullptr)
												pParent->color = _rbnode::eColor::_black;

											pCurrent = pParent;
											pParent = pCurrent->parent;
											continue;
										}
									}
									else            //兄弟节点为红  
									{
										pParent->color = _rbnode::eColor::_red;
										pSibling->color = _rbnode::eColor::_black;

										LeftRotate(&pParent);

										if (pParent->parent)
										{
											if (pParent->parent->parent == nullptr)
												root = pParent->parent;
										}
										continue;
									}
								}
								else//当前节点为父节点右孩子  
								{
									pSibling = pParent->left;

									if (pSibling->color == _rbnode::eColor::_black)
									{
										_rbnode * pSiblingLeftChild = pSibling->left;
										_rbnode * pSiblingRightChild = pSibling->right;

										if (pSiblingLeftChild && pSiblingLeftChild->color == _rbnode::eColor::_red)
										{
											pSibling->color = pParent->color;
											pSiblingLeftChild->color = _rbnode::eColor::_black;
											pParent->color = _rbnode::eColor::_black;

											RightRotate(&pParent);

											if (pParent->parent)
											{
												if (pParent->parent->parent == nullptr)
													root = pParent->parent;
											}

											break;
										}

										if (pSiblingRightChild && pSiblingRightChild->color == _rbnode::eColor::_red)
										{
											LeftRotate(&pSibling);

											pSibling = pParent->left;

											pSibling->color = pParent->color;
											pParent->color = _rbnode::eColor::_black;

											RightRotate(&pParent);

											if (pParent->parent)
											{
												if (pParent->parent->parent == nullptr)
													root = pParent->parent;
											}

											break;
										}

										if (pParent->color == _rbnode::eColor::_red)
										{
											pSibling->color = _rbnode::eColor::_red;
											pParent->color = _rbnode::eColor::_black;

											break;
										}
										else
										{
											pSibling->color = _rbnode::eColor::_red;
											pParent->color = pCurrent->color;
											pCurrent->color = _rbnode::eColor::_black;

											pCurrent = pParent;
											pParent = pCurrent->parent;
											continue;
										}
									}
									else            //兄弟节点为红  
									{
										pParent->color = _rbnode::eColor::_red;
										pSibling->color = _rbnode::eColor::_black;

										RightRotate(&pParent);

										if (pParent->parent)
										{
											if (pParent->parent->parent == nullptr)
												root = pParent->parent;
										}

										continue;
									}
								}

							}
							return itr;
						}
					}
					else//兄弟节点为红  
					{
						pParent->color = _rbnode::eColor::_red;
						pSibling->color = _rbnode::eColor::_black;

						RightRotate(&pParent);

						if (pParent->parent)
						{
							if (pParent->parent->parent == nullptr)
								root = pParent->parent;
						}

						continue;
					}
				}
			}
		}
		return itr;
	}
#pragma endregion  

	Iterator begin()
	{
		Iterator RetIterator;

		if (root == 0)
			goto _EXIT;

		_rbnode* pCurrent = root;

		while (pCurrent->left)
			pCurrent = pCurrent->left;

		RetIterator = pCurrent;

	_EXIT:
		return RetIterator;
	}
	Iterator rbegin()
	{
		Iterator RetIterator;

		if (root == 0)
			goto EXIT_SUCCESS;

		_rbnode* pCurrent = root;

		while (pCurrent->right)
			pCurrent = pCurrent->right;

		RetIterator = pCurrent;

	EXIT_SUCCESS:
		return RetIterator;
	}

	Iterator end()
	{
		return Iterator();
	}
	Iterator rend()
	{
		return Iterator();
	}

private:
	static _rbnode* PrevTreeNode(_rbnode* pNode)
	{
		if (pNode == 0)
			return 0;

		if (pNode->m_pLeft != 0)
		{
			_rbnode* pCurrentNode = pNode->m_pLeft;

			while (pCurrentNode->m_pRight)
				pCurrentNode = pCurrentNode->m_pRight;

			return pCurrentNode;
		}
		else
		{
			_rbnode* pParent = pNode->m_pParent;

			if (pParent == 0)
				return 0;

			if (pNode == pParent->m_pRight)
				return pParent;

			pNode = pParent;
			pParent = pNode->m_pParent;

			while (pParent)
			{
				if (pNode == pParent->m_pRight)
					return pParent;
				else
				{
					pNode = pParent;
					pParent = pNode->m_pParent;
				}
			}

			return 0;
		}
	}

	static _rbnode* NextTreeNode(_rbnode* pNode)
	{
		if (pNode == 0)
			return 0;

		if (pNode->right != 0)
		{
			_rbnode* pCurrentNode = pNode->right;

			while (pCurrentNode->left)
				pCurrentNode = pCurrentNode->left;

			return pCurrentNode;
		}
		else
		{
			_rbnode* pParent = pNode->parent;

			if (pParent == 0)
				return 0;

			if (pNode == pParent->left)
				return pParent;

			pNode = pParent;
			pParent = pNode->parent;

			while (pParent)
			{
				if (pNode == pParent->left)
					return pParent;
				else
				{
					pNode = pParent;
					pParent = pNode->parent;
				}
			}

			return 0;
		}
	}

	void RightRotate(_rbnode ** ppCurrent)
	{
		_rbnode * pLeftChild = (*ppCurrent)->left;

		if (pLeftChild == 0) return;

		(*ppCurrent)->left = pLeftChild->right;

		if ((*ppCurrent)->left != 0)
			(*ppCurrent)->left->parent = (*ppCurrent);

		pLeftChild->right = (*ppCurrent);
		pLeftChild->parent = (*ppCurrent)->parent;

		(*ppCurrent)->parent = pLeftChild;

		if (pLeftChild->parent != 0)
		{
			if (pLeftChild->parent->left == (*ppCurrent))
				pLeftChild->parent->left = pLeftChild;
			else
				pLeftChild->parent->right = pLeftChild;
		}
	}
	void LeftRotate(_rbnode ** ppCurrent)
	{
		_rbnode * pRightChild = (*ppCurrent)->right;

		if (pRightChild == 0) return;

		(*ppCurrent)->right = pRightChild->left;

		if ((*ppCurrent)->right != 0)
			(*ppCurrent)->right->parent = (*ppCurrent);

		pRightChild->left = (*ppCurrent);
		pRightChild->parent = (*ppCurrent)->parent;
		(*ppCurrent)->parent = pRightChild;

		if (pRightChild->parent != 0)
		{
			if (pRightChild->parent->left == (*ppCurrent))
				pRightChild->parent->left = pRightChild;
			else
				pRightChild->parent->right = pRightChild;
		}
	}

public:
	_pool rbnodeAllocator;

	_rbnode * root;
};

#endif 