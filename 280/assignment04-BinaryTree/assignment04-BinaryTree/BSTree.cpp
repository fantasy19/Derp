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

}

template <typename T>
void BSTree<T>::clear(void) {

}

template <typename T>
bool BSTree<T>::find(const T& value, unsigned &compares) const {

}

template <typename T>
int BSTree<T>::height(void) const {
	if (root_ == 0)
		return -1;

	//if (height(root_->left) > height(root_->right))
	//	return height(root_->left) + 1;
	//else
	//	return height(root_->right) + 1;

}

template <typename T>
BSTree<T>::BinTree BSTree<T>::root(void) const {

}

template <typename T>
bool BSTree<T>::empty(void) const {
	return 0;
}

template <typename T>
unsigned int BSTree<T>::size(void) const {
	return 0;
}

template <typename T>
typename BSTree<T>::BinTree& BSTree<T>::get_root(void) {
	return root_;
}

template <typename T>
BSTree<T>::BinTree make_node(const T& value) {

}