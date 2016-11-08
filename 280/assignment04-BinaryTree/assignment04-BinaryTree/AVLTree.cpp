/***************************************************************************/
/*!
\fn template <typename T> AVLTree<T>::AVLTree(ObjectAllocator *OA, bool ShareOA) : BSTree<T>(OA, ShareOA)
\brief
Constructor of the AVLTree

\param OA
object allocator for the tree's nodes

\param ShareOA
boolean for sharing object allcoator among trees

\return
none

*/
/***************************************************************************/

template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *OA, bool ShareOA) : BSTree<T>(OA, ShareOA) {}

/***************************************************************************/
/*!
\fn template <typename T> AVLTree<T>::~AVLTree()
\brief
Destructor of AVLTree
\return
none
*/
/***************************************************************************/

template <typename T>
AVLTree<T>::~AVLTree() {}

/***************************************************************************/
/*!
\fn template <typename T> void AVLTree<T>::insert(const T& value) throw(BSTException)
\brief
insert a node into the tree

\param value
the value of node to be inserted

\return
none
*/
/***************************************************************************/

template <typename T>
void AVLTree<T>::insert(const T& value) throw(BSTException) {

	insert_begin(BSTree<T>::get_root(), value);
	node_count(BSTree<T>::get_root());
}

/***************************************************************************/
/*!
\fn template <typename T> void AVLTree<T>::insert_begin(typename BSTree<T>::BinTree &tree, const T& value)
\brief
the function to begin the recursion process to insert node

\param tree
the value of node to be inserted

\param value
the value of node to be inserted

\return
none
*/
/***************************************************************************/

template <typename T>
void AVLTree<T>::insert_begin(typename BSTree<T>::BinTree &tree, const T& value) {
	std::stack<typename BSTree<T>::BinTree*> stack_;
	insert_node(tree, value, stack_);

}

/***************************************************************************/
/*!
\brief
helper function insert a node into the tree by recursion

\param node
the tree/subtree for node to be inserted

\param value
the value of node to be inserted

\param nodes
the stack to push/pop nodes

\return
none
*/
/***************************************************************************/

template <typename T>
void AVLTree<T>::insert_node(typename BSTree<T>::BinTree & node, const T& value, std::stack<typename BSTree<T>::BinTree*> & nodes) {
	if (node == 0) {
		node = BSTree<T>::make_node(value);
		BalanceAVLTree(nodes); 
	}
	else if (value < node->data) {
		nodes.push(&node);
		insert_node(node->left, value, nodes);
	}
	else if (value > node->data) {
		nodes.push(&node); 
		insert_node(node->right, value, nodes);
	}
	else
		std::cout << "Error, duplicate item" << std::endl;
}

/***************************************************************************/
/*!
\fn template <typename T> void AVLTree<T>::remove(const T& value)
\brief
remove a node in the tree

\param value
the value of node to be removed

\return
none
*/
/***************************************************************************/

template <typename T>
void AVLTree<T>::remove(const T& value) {
	remove_begin(BSTree<T>::get_root(), value);
	node_count(BSTree<T>::get_root());
}

/***************************************************************************/
/*!
\fn template <typename T> void AVLTree<T>::remove_begin(typename BSTree<T>::BinTree &tree, const T& value)
\brief
the function to begin the recursion process to remove node

\param tree
the value of node to be removed

\param value
the value of node to be removed

\return
none
*/
/***************************************************************************/

template <typename T>
void AVLTree<T>::remove_begin(typename BSTree<T>::BinTree &tree, const T& value) {
	std::stack<typename BSTree<T>::BinTree *> stack_;
	remove_node(tree, value, stack_);
}

/***************************************************************************/
/*!
\fn template<typename T> void AVLTree<T>::remove_node(typename BSTree<T>::BinTree &tree, const T& value, std::stack<typename BSTree<T>::BinTree*>& nodes)
\brief
helper function to remove for the correct node to delete by recursion.

\param tree
the tree to remove node from

\param value
the value to match for removal

\param nodes
the stack deal with

\return
none
*/
/***************************************************************************/

template<typename T>
void AVLTree<T>::remove_node(typename BSTree<T>::BinTree &tree, const T& value,
	std::stack<typename BSTree<T>::BinTree*>& nodes) {

	if (tree == 0)
		return;

	else if (value < tree->data){
		nodes.push(&tree);
		remove_node(tree->left, value, nodes);
	}
	else if (value > tree->data){
		nodes.push(&tree);
		remove_node(tree->right, value, nodes);
	}
	else{ // match fit

		if (tree->left == 0){
			typename BSTree<T>::BinTree temp = tree;
			tree = tree->right;
			BSTree<T>::free_node(temp);
		}
		else if (tree->right == 0){
			typename BSTree<T>::BinTree temp = tree;
			tree = tree->left;
			BSTree<T>::free_node(temp);
		}
		else{ // if node has 2 children
			typename BSTree<T>::BinTree pred = 0;
			BSTree<T>::find_predecessor(tree, pred);
			tree->data = pred->data;
			remove_node(tree->left, tree->data, nodes);
			BalanceAVLTree(nodes);
		}
	}
}

/***************************************************************************/
/*!
\fn template <typename T> void AVLTree<T>::RotateLeft(typename BSTree<T>::BinTree & node)
\brief
rotate the tree around the node to the left

\param node
the node to rotate about

\return
none
*/
/***************************************************************************/

template <typename T>
void AVLTree<T>::RotateLeft(typename BSTree<T>::BinTree & node) {

	typename BSTree<T>::BinTree temp = node;
	node = node->right;
	temp->right = node->left;
	node->left = temp;
	
	node->count = temp->count;
	//recount node count
	unsigned leftCount = (temp->left) ? temp->left->count : 0;
	unsigned rightCount = (temp->right) ? temp->right->count : 0;

	temp->count = leftCount + rightCount + 1;
}

/***************************************************************************/
/*!
\fn template <typename T> void AVLTree<T>::RotateRight(typename BSTree<T>::BinTree & node)
\brief
rotate the tree around the node to the right

\param node
the node to rotate about

\return
none
*/
/***************************************************************************/

template <typename T>
void AVLTree<T>::RotateRight(typename BSTree<T>::BinTree & node) {

		typename BSTree<T>::BinTree temp = node;
		node = node->left;
		temp->left = node->right;
		node->right = temp;
		
		node->count = temp->count;
		//recount node count
		unsigned leftCount = (temp->left) ? temp->left->count : 0;
		unsigned rightCount = (temp->right) ? temp->right->count : 0;

		temp->count = leftCount + rightCount + 1;
}

/***************************************************************************/
/*!
\fn template <typename T> void AVLTree<T>::BalanceAVLTree(std::stack<typename BSTree<T>::BinTree*> & nodes)
\brief
balances the avl tree

\param nodes
the stack of nodes used to balance the tree

\return
none
*/
/***************************************************************************/

template <typename T>
void AVLTree<T>::BalanceAVLTree(std::stack<typename BSTree<T>::BinTree*> & nodes) {
	while (!nodes.empty()) {
		typename BSTree<T>::BinTree * topnode = nodes.top();
		nodes.pop();

		typename BSTree<T>::BinTree &node = *topnode;

		int RH = BSTree<T>::tree_height(node->right);
		int LH = BSTree<T>::tree_height(node->left);

		if (RH > (LH + 1)) {
			//promote twice
			if (BSTree<T>::tree_height(node->right->left) > BSTree<T>::tree_height(node->right->right)) {
				RotateRight(node->right);
				RotateLeft(node);
			}
			else
				RotateLeft(node); //promote once
		}
		else
		if ((RH + 1)< LH ) {
			//promote once
			if (BSTree<T>::tree_height(node->left->left) > BSTree<T>::tree_height(node->left->right)) {
				RotateRight(node);
				node_count(BSTree<T>::get_root());
			}
			else { //promote twice
				RotateLeft(node->left);
				RotateRight(node);
			}
		}

	}
}

/***************************************************************************/
/*!
\fn template <typename T> unsigned int AVLTree<T>::node_count(typename BSTree<T>::BinTree & tree) const
\brief
extra credit for efficient balancing

\return
total node count of a tree
*/
/***************************************************************************/

template <typename T>
unsigned int AVLTree<T>::node_count(typename BSTree<T>::BinTree & tree) const {
	if (tree == NULL)
		return 0;

	tree->count = 1 + node_count(tree->left) + node_count(tree->right);

	return tree->count;
}

/***************************************************************************/
/*!
\fn template <typename T> bool AVLTree<T>::ImplementedBalanceFactor(void)
\brief
extra credit for efficient balancing

\return
none
*/
/***************************************************************************/

template <typename T>
bool AVLTree<T>::ImplementedBalanceFactor(void) { return false; }