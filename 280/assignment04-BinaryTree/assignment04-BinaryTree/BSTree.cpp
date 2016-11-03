template <typename T>
BSTree<T>::BSTree(ObjectAllocator *OA, bool ShareOA) {

}

template <typename T>
BSTree<T>::BSTree(const BSTree& rhs) {

}

template <typename T>
BSTree<T>::~BSTree() {

}

template <typename T>
BSTree<T>& BSTree<T>::operator=(const BSTree& rhs) {
	return *this;
}

template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::operator[](int index) const {

}

template <typename T>
void BSTree<T>::insert(const T& value) throw(BSTException) {

}

template <typename T>
void BSTree<T>::remove(const T& value) {
	if (root_ == 0)
		return;

	
}

template <typename T>
void BSTree<T>::clear(void) {
	if (!root_)
		return;

	free_tree(root_);
}

template <typename T>
bool BSTree<T>::find(const T& value, unsigned &compares) const {

	if (!root_)
		return false;

	if (root_->data == value) {
		++compares;
		return true;
	}
	
	if (num < root_->left->data)
		return find(root_->left,++compares);
	else
		return find(root_->right,++compares);

}

template <typename T>
int BSTree<T>::height(void) const {
	return tree_height(root_);
}

template <typename T>
BSTree<T>::BinTree BSTree<T>::root(void) const {
	return root_;
}

template <typename T>
bool BSTree<T>::empty(void) const {
	return (root_ == 0);
}

template <typename T>
unsigned int BSTree<T>::size(void) const {
	if (root_ == 0)
		return 0;
	else
		return 1 + size(root_->left) + size(root_->right);

}

template <typename T>
typename BSTree<T>::BinTree& BSTree<T>::get_root(void) {
	return root_;
}

template <typename T>
BSTree<T>::BinTree make_node(const T& value) {
	BinTreeNode * node = new BinTreeNode(value);
	return node;
}

template <typename T>
int BSTree<T>::tree_height(BinTree tree) const {
	if (tree == 0)
		return 0;

	if (tree_height(tree->left) > tree_height(tree->right))
		return tree_height(tree->left) + 1;
	else
		return tree_height(tree->right) + 1;

}

template <typename T>
void BSTree<T>::free_node(BinTree node) {
	if (node == 0)
		return;

}

template <typename T>
void BSTree<T>::free_tree(BinTree node) {
	if (node) {
		free_node(node->left);
		free_node(node->right);
		delete node;
	}
}