

template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *OA, bool ShareOA) : oa(OA), share(ShareOA) {
	/*if (!oa && !share)
		oa = new ObjectAllocator(sizeof(AVLTree), OAConfig());*/
}

template <typename T>
AVLTree<T>::~AVLTree() {

}

template <typename T>
void AVLTree<T>::insert(const T& value) throw(BSTException) {

	insert_begin(BSTree<T>::get_root(), value);
	node_count(BSTree<T>::get_root());
}

template <typename T>
void AVLTree<T>::insert_begin(typename BSTree<T>::BinTree &tree, const T& value) {
	std::stack<typename BSTree<T>::BinTree*> stack_;
	insert_node(tree, value, stack_);

}


template <typename T>
void AVLTree<T>::remove(const T& value) {
	remove_begin(BSTree<T>::get_root(), value);
	node_count(BSTree<T>::get_root());
}

template <typename T>
void AVLTree<T>::remove_begin(typename BSTree<T>::BinTree &tree, const T& value) {
	std::stack<typename BSTree<T>::BinTree *> stack_;
	remove_node(tree, value, stack_);
}

template <typename T>
bool AVLTree<T>::ImplementedBalanceFactor(void) { return false; }

template <typename T>
void AVLTree<T>::RotateLeft(typename BSTree<T>::BinTree & node) {

		typename BSTree<T>::BinTree temp = node;
		node = node->right;
		temp->right = node->left;
		node->left = temp;
	
		node->count = temp->count;

		unsigned leftCount = (temp->left) ? temp->left->count : 0;
		unsigned rightCount = (temp->right) ? temp->right->count : 0;

		temp->count = leftCount + rightCount + 1;
}

template <typename T>
void AVLTree<T>::RotateRight(typename BSTree<T>::BinTree & node) {

		typename BSTree<T>::BinTree temp = node;
		node = node->left;
		temp->left = node->right;
		node->right = temp;
		
		node->count = temp->count;

		unsigned leftCount = (temp->left) ? temp->left->count : 0;
		unsigned rightCount = (temp->right) ? temp->right->count : 0;

		temp->count = leftCount + rightCount + 1;
}

template <typename T>
void AVLTree<T>::insert_node(typename BSTree<T>::BinTree & node, T value, std::stack<typename BSTree<T>::BinTree*> & nodes) {
	if (node == 0) {
		node = BSTree<T>::make_node(value);
		BalanceAVLTree(nodes); // Balance it now
	}
	else if (value < node->data) {
		nodes.push(&node); // save visited node
		insert_node(node->left, value, nodes);
	}
	else if (value > node->data) {
		nodes.push(&node); // save visited node
		insert_node(node->right, value, nodes);
	}
	else
		std::cout << "Error, duplicate item" << std::endl;
}

template<typename T>
void AVLTree<T>::remove_node(typename BSTree<T>::BinTree &tree, const T& value,
	std::stack<typename BSTree<T>::BinTree*>& nodes) {
	if (tree == 0)
		return;
	else if (value < tree->data) {
		nodes.push(&tree);
		remove_node(tree->left, tree->data, nodes);
	}
	else if (value > tree->data) {
		nodes.push(&tree);
		remove_node(tree->right, tree->data, nodes);
	}
	else {
		if (!tree->left) {
			typename BSTree<T>::BinTree temp = tree;
			tree = tree->right;
			BSTree<T>::free_node(temp);
		}
		else

			if (!tree->right) {
				typename BSTree<T>::BinTree temp = tree;
				tree = tree->left;
				BSTree<T>::free_node(temp);
			}

			else {
				typename BSTree<T>::BinTree pred = 0;
				BSTree<T>::find_predecessor(tree, pred);
				tree->data = pred->data;
				remove_node(tree->left, tree->data, nodes);
				BalanceAVLTree(nodes);
			}

	}
}

template <typename T>
void AVLTree<T>::BalanceAVLTree(std::stack<typename BSTree<T>::BinTree*> & nodes) {
	while (!nodes.empty()) {
		typename BSTree<T>::BinTree * nodetop = nodes.top();
		nodes.pop();

		typename BSTree<T>::BinTree &node = *nodetop;

		int RH = BSTree<T>::tree_height(node->right);
		int LH = BSTree<T>::tree_height(node->left);

		if (RH > (LH + 1)) {
			if (BSTree<T>::tree_height(node->right->left) > BSTree<T>::tree_height(node->right->right)) {
				RotateRight(node->right);
				RotateLeft(node);
				//Promote v twice
			}
			else
				RotateLeft(node);
				//Promote u
		}
		else
		if ((RH + 1)< LH ) {
			if (BSTree<T>::tree_height(node->left->left) > BSTree<T>::tree_height(node->left->right)) {
				RotateRight(node);
				node_count(BSTree<T>::get_root());
				//Promote u
			}
			else {
				RotateLeft(node->left);
				RotateRight(node);
				//Promote w twice
			}
		}

		// implement algorithm using functions that
		// are already defined (Height, RotateLeft, RotateRight)
	}
}

template <typename T>
unsigned int AVLTree<T>::node_count(typename BSTree<T>::BinTree & tree) const {
	if (tree == NULL)
		return 0;

	tree->count = 1 + node_count(tree->left) + node_count(tree->right);

	return tree->count;
}