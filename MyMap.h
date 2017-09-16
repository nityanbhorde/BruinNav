
// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#ifndef Mymap_h
#define Mymap_h

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap() {
		m_size = 0;
		m_root = nullptr;
	}
	~MyMap() { clear(); }
	void clear() {
		deleteTree(m_root);
	}
	int size() const {
		return m_size;
	}
	void associate(const KeyType& key, const ValueType& value) {
		if (m_root == nullptr) { //if its a nullptr we will create a new root node 
			m_root = new Node(key, value);
			m_size++;
			return;
		}
		Node * cur = m_root;
		for (;;) { // sorts through until we find the right place to insert the value by checking if the value is less than the current node's
			if (key == cur->key) { // if we find the samekey, replace the value and return
				cur->m_value = value;
				return;
			}
			if (key < cur->key) { // if its left of the  we check if there is something there
				if (cur->left != nullptr) { // if we already have a value we set our search pointer to it for our next round of for loop
					cur = cur->left;
				}
				else { //otherwise we will just make a node here
					cur->left = new Node(key, value);
					m_size++;
					return;
				}
			}
			else if (key > cur->key) { // same concept with the right as with the left
				if (cur->right != nullptr) {
					cur = cur->right;;
				}
				else {
					cur->right = new Node(key, value);
					m_size++;
					return;
				}
			}
		}
	}
	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const {
		Node * cur = m_root;
		while (cur != nullptr) { // iterate through looking for the key
			if (key == cur->key) {
				return &cur->m_value; // if we find it we return its value 
			}
			else if (key < cur->key) { // if its less than we will go to our left part of our tree
				cur = cur->left;
			} // otherwise we will search in the right part of our tree
			else {
				cur = cur->right;
			}
		}
		return nullptr;
	}
	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}
	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;
private:
	int m_size;
	struct Node {
		Node(KeyType key, ValueType val) {
			m_value = val;
			this->key = key;
			left = right = nullptr;
		}
		Node *left, *right;
		ValueType  m_value;
		KeyType  key;
	};
	void deleteTree(Node * cur) { // private member function that will delete the tree for me recursively
		if (cur == nullptr) {
			return;
		}
		deleteTree(cur->left);
		deleteTree(cur->right);
		delete cur;
	}
	Node* m_root;
};
#endif // !Mymap_h