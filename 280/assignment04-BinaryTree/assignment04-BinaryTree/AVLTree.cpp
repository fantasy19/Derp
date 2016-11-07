

template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *OA, bool ShareOA) : oa(OA), share(ShareOA) {
	if (!oa && !share)
		oa = new ObjectAllocator(sizeof(AVLTree), OAConfig());
}

template <typename T>
AVLTree<T>::~AVLTree() {

}

template <typename T>
void AVLTree<T>::insert(const T& value) throw(BSTException) {
	std::stack<BinTree> stack_;
	insert_node(root_, value, stack_);

}

template <typename T>
void AVLTree<T>::remove(const T& value) {

}

template <typename T>
bool AVLTree<T>::ImplementedBalanceFactor(void) { return false; }

template <typename T>
void AVLTree<T>::RotateLeft(BinTree node) {
	BinTree temp = node;
	node = node->left;
	temp->right = node->left;
	node->left = temp;
}

template <typename T>
void AVLTree<T>::RotateRight(BinTree node) {
	BinTree temp = node;
	node = node->right;
	temp->left = node->right;
	node->right = temp;
}

template <typename T>
void AVLTree<T>::insert_node(BinTree & node, T value, std::stack<BinTree> & nodes) {
	if (node == 0) {
		node = make_node(value);
		BalanceAVLTree(nodes); // Balance it now
	}
	else if (value < node->data) {
		nodes.push(node); // save visited node
		insert_node(node->left, value, nodes);
	}
	else if (value > node->data) {
		nodes.push(node); // save visited node
		insert_node(node->right, value, nodes);
	}

}

template <typename T>
void AVLTree<T>::BalanceAVLTree(std::stack<BinTree> & nodes) {
	while (!nodes.empty()) {
		BinTree node = nodes.top();
		nodes.pop();

		if (tree_height(node->right)>(tree_height(node->left) + 1)) {
			if (tree_height(node->right->left) > tree_height(node->right->right)) {
				RotateRight(node->right->left);
				RotateLeft(node->right);
				//Promote v twice
			}
			else
				RotateLeft(node->right);
				//Promote u
		}
		
		if ((tree_height(node->right) + 1)<tree_height(node->left)) {
			if (tree_height(node->left)>tree_height(node->right))
				RotateLeft(node->left);
				//Promote u
			else {
				RotateLeft(node->left->right);
				RotateRight(node->left);
				//Promote w twice
			}
		}

		// implement algorithm using functions that
		// are already defined (Height, RotateLeft, RotateRight)
	}
}
