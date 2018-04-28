/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

//template <class T> struct less {
//	bool operator() (const T& x, const T& y) const { return x<y; }
//	typedef T first_argument_type;
//	typedef T second_argument_type;
//	typedef bool result_type;
//};
template <class KEY, class T, class Compare = std::less<KEY>> //less is the compare function
class map
{
  public:
	/**
	* the internal type of data.
	* it should have a default constructor, a copy constructor.
	* You can use sjtu::map as value_type by typedef.
	*/
	// first； allocate space of KEY and T
	using value_type = pair<const KEY, T>;
	//using iterator_category = std::random_access_iterator_tag;
	//using pointer = T * ;
	//using reference = T & ;
	using zero = std::nullptr;

private:
#pragma region node
	struct node {
		//it should have a default constructor, a copy constructor.
		value_type *data;
		node *dad;
		node *lchild, *rchild;

		node *pre;
		node *next;

		node(node *dad = zero, node *lchild = zero, node *rchild = zero,
								 node *pre = zero, node *next = zero)
			:dad(dad), lchild(lchild), rchild(rchild), pre(pre), next(next){}
		construct(const value_type &value) {
			data = new value_type(value);
		}
		
	};
#pragma endregion
	node *root;
	node *build_tree(node *root, node *ot_root, node * dad = zero) {

		root = new node;
		root->construct( *(ot_root->data) );
		root->dad = dad;
		if (ot_root->lchild != zero) root->lchild = build_tree(root->lchild, ot_root->lchild, root);
		if (ot_root->rchild != zero) root->rchild = build_tree(root->rchild, ot_root->rchild, root);
		return root;
	}
	//node * build_iter(node *now, node *dad) {
	//	if (root->lchild != zero) root->pre = build_tree(root->lchild, root);
	//	dad->pre = root;
	//	if (root->rchild != zero) root->next = build_tree(root->lchild, root);

	//}
public:
#pragma region iterator
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */

	class const_iterator;
	class iterator {
	private:
		//node *root;
		node *cur;
	public:
		iterator(node *cur = zero):cur(cur) {}
		iterator(const iterator &other) {cur = other.cur;}
		iterator& operator=(const iterator &other) { cur = other.cur; }

		iterator operator++(int) {
			if (cur->next == zero) throw invalid_iterator();
			iterator tmp = *this;
			cur = cur->next;
			return tmp;
		}
		iterator & operator++() {
			if (cur->next == zero) throw invalid_iterator();
			cur = cur->next;
			return *this;
		}
		iterator operator--(int) {
			if (cur->pre == zero) throw invalid_iterator();
			iterator tmp = *this;
			cur = cur->pre;
			return tmp;
		}
		iterator & operator--() {
			if (cur->pre == zero) throw invalid_iterator();
			cur = cur->pre;
			return *this;
		}
		/**
		* return a new iterator which pointer n-next elements
		*   even if there are not enough elements, just return the answer.
		* as well as operator-
		*/
		iterator operator+(const ptrdiff_t &n) {
			iterator tmp = *this;
			if (n < 0) return (tmp - (-n));
			for (size_t i = 0; i < n; ++i) {
				if (tmp.cur->next == zero) throw invalid_iterator();
				tmp.cur = tmp.cur->next;
			}
			return tmp;
		}
		iterator operator-(const ptrdiff_t &n) {
			iterator tmp = *this;
			if (n < 0) return (tmp + (-n));
			for (size_t i = 0; i < n; ++i) {
				if (tmp.cur->pre == zero) throw invalid_iterator();
				tmp.cur = tmp.cur->pre;
			}
			return tmp;
		}
		ptrdiff_t operator-(const iterator &rhs) {
			ptrdiff_t det = 0;
			iterator tmp = *this;
			while (tmp->next != zero) {
				if (tmp == rhs) return det;
				++det;
				tmp = tmp->next;
			}
			//return (rhs - *this);
			det = 0;
			tmp = *this;
			while (tmp->pre != zero) {
				if (tmp == rhs) return det;
				++det;
				tmp = tmp->pre;
			}
			throw invalid_iterator();
		}
		//+=, -=, =, ->
		iterator & operator+=(const ptrdiff_t &n) {
			*this = *this + n;
			return *this;
		}
		iterator &opeartor -= (const ptrdiff_t &n) {
			*this = *this - n;
			return *this;
		}
		value_type & operator*() const { return *(cur->data); }
		value_type * operator->() const noexcept { return cur->data; }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		
		bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
		bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
		bool operator!=(const const_iterator &rhs) const { return cur != rhs.cur; }

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */

	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
		public:
			const_iterator() {
				// TODO
			}
			const_iterator(const const_iterator &other) {
				// TODO
			}
			const_iterator(const iterator &other) {
				// TODO
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};
#pragma endregion

#pragma region functions
	/**
	 * TODO two constructors
	 */
	map():root(zero) {}
	map(const map &other) {
		if (other.root == zero) return;
		root = build_tree(root, other.root);
		build_iter(root);
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
		
	}
	/**
	 * TODO Destructors
	 */
	~map() {}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const KEY &key) {}
	const T & at(const KEY &key) const {}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const KEY &key) {}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const KEY &key) const {}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {}
	const_iterator cbegin() const {}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {}
	const_iterator cend() const {}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {}
	/**
	 * clears the contents
	 */
	void clear() {}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const KEY &key) const {}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const KEY &key) {}
	const_iterator find(const KEY &key) const {}

#pragma endregion
};

}

#endif
