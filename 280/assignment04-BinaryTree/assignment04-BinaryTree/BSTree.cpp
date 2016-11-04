template <typename T>
BSTree<T>::BSTree(ObjectAllocator *OA, bool ShareOA) : oa(OA), share(ShareOA){
	if (!oa && !share)
		oa = new ObjectAllocator(sizeof(BinTreeNode), OAConfig());
}

template <typename T>
BSTree<T>::BSTree(const BSTree& rhs) {

	if (rhs.oa)
	{
		objAllocator_ = rhs.objAllocator_; // Use rhs' allocator
		 // We don't own it (won't free it)
		share = true;            // If a copy of 'this object' is made, share the allocator
	}
	else // No sharing, create our own personal allocator
	{
		OAConfig config(true); // Set UseCPPMemManager to true, default the others
		objAllocator_ = new ObjectAllocator(sizeof(BinTreeNode), config);
		// We own the allocator, we will have to free it
		share = false; // Do not share this allocator with any other list
	}
	
	copy_tree(root_, rhs.root_);
}

template <typename T>
BSTree<T>::~BSTree() {
	//clear()
	if (!share)
		delete oa;
}

template <typename T>
BSTree<T>& BSTree<T>::operator=(const BSTree& rhs) {
	if (rhs.oa)
	{
		objAllocator_ = rhs.objAllocator_; // Use rhs' allocator
										   // We don't own it (won't free it)
		share = true;            // If a copy of 'this object' is made, share the allocator
	}
	else // No sharing, create our own personal allocator
	{
		OAConfig config(true); // Set UseCPPMemManager to true, default the others
		objAllocator_ = new ObjectAllocator(sizeof(BinTreeNode), config);
		// We own the allocator, we will have to free it
		share = false; // Do not share this allocator with any other list
	}
	
	copy_tree(root_, rhs.root_);

	//insert?
	return *this;
}

template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::operator[](int index) const {

}

template <typename T>
void BSTree<T>::insert(const T& value) throw(BSTException) {
	if (root_ == 0)
		root_ = make_node(value);

	else if (value < root_->data)
		InsertItem(root_->left, value);
	else if (value > root_->data)
		InsertItem(root_->right, value);

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
BSTree<T>::BinTree BSTree<T>::make_node(const T& value) {

	BinTree node = 0;

	try{
		node = reinterpret_cast<BinTreeNode *>(objAllocator_->Allocate()); // Allocate memory for the object.
		node = new (mem) BinTreeNode(value); // Construct the object in the memory.
	}
	catch (const OAException &e){
		throw(BSTException(BSTException::E_NO_MEMORY, e.what()));
	}

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
		free_node(node);
	}
}

template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree &predecessor) const {
	if (tree == 0)
		return;

	if (predecessor->left->data == tree->left->data 
		|| predecessor->right->data == tree->right->data)
		return;

	find_predecessor(tree->left, predecessor);
	find_predecessor(tree->right, predecessor);
}

template <typename T>
void BSTree<T>::copy_tree(BinTree lhs, BinTree rhs) {
	if (rhs) {
		lhs = make_node(rhs->data);
		copy_tree(lhs->left, rhs->left);
		copy_tree(lhs->right, rhs->right);
	}
	else
		lhs = 0;
}