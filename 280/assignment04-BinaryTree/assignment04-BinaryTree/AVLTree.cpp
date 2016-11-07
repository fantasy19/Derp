

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
		node = node->right;
		temp->right = node->left;
		node->left = temp;
	
		node->count = temp->count;

		unsigned leftCount = (temp->left) ? temp->left->count : 0;
		unsigned rightCount = (temp->right) ? temp->right->count : 0;

		temp->count = leftCount + rightCount + 1;
}

template <typename T>
void AVLTree<T>::RotateRight(BinTree node) {

		BinTree temp = node;
		node = node->left;
		temp->left = node->right;
		node->right = temp;
		
		node->count = temp->count;

		unsigned leftCount = (temp->left) ? temp->left->count : 0;
		unsigned rightCount = (temp->right) ? temp->right->count : 0;

		temp->count = leftCount + rightCount + 1;
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
	else
		std::cout << "Error, duplicate item" << std::endl;
}

template <typename T>
void AVLTree<T>::BalanceAVLTree(std::stack<BinTree> & nodes) {
	while (!nodes.empty()) {
		BinTree node = nodes.top();
		nodes.pop();

		int RH = tree_height(node->right);
		int LH = tree_height(node->left);

		if (RH > (LH + 1)) {
			if (tree_height(node->right->left) > tree_height(node->right->right)) {
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
			if (tree_height(node->left->left) > tree_height(node->left->right)) {
				RotateRight(node);
				node_count(get_root());
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
unsigned int AVLTree<T>::node_count(BinTree tree) const {
	if (tree == NULL)
		return 0;

	return 1 + node_count(tree->left) + node_count(tree->right);
}