template <typename T>
BSTree<T>::BSTree(ObjectAllocator *OA, bool ShareOA) : oa(OA), share(ShareOA){
	root_ = 0;
	count = 0;
	if (!oa && !share) {
		try {
			oa = new ObjectAllocator(sizeof(BinTreeNode), OAConfig());
		}
		catch (const OAException &e) {
			throw(BSTException(BSTException::E_NO_MEMORY, e.what()));
		}
	}
}

template <typename T>
BSTree<T>::BSTree(const BSTree& rhs) {

	if (rhs.oa)
	{
		count = rhs.count;
		oa = rhs.oa; // Use rhs' allocator
		 // We don't own it (won't free it)
		share = true;            // If a copy of 'this object' is made, share the allocator
	}
	else // No sharing, create our own personal allocator
	{
		OAConfig config(true); // Set UseCPPMemManager to true, default the others
		oa = new ObjectAllocator(sizeof(BinTreeNode), config);
		// We own the allocator, we will have to free it
		share = false; // Do not share this allocator with any other list
	}
	
	copy_tree(root_, rhs.root_);
}

template <typename T>
BSTree<T>::~BSTree() {
	clear();
	if (!share) {
		delete oa;
	}
}

template <typename T>
BSTree<T>& BSTree<T>::operator=(const BSTree& rhs) {
	if (rhs.oa)
	{
		count = rhs.count;
		oa = rhs.oa; // Use rhs' allocator
										   // We don't own it (won't free it)
		share = true;            // If a copy of 'this object' is made, share the allocator
	}
	else // No sharing, create our own personal allocator
	{
		OAConfig config(true); // Set UseCPPMemManager to true, default the others
		oa = new ObjectAllocator(sizeof(BinTreeNode), config);
		// We own the allocator, we will have to free it
		share = false; // Do not share this allocator with any other list
	}
	
	copy_tree(root_, rhs.root_);

	return *this;
}

template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::operator[](int index) const {
	
	return sub_node(root_, index);
}

template <typename T>
void BSTree<T>::insert(const T& value) throw(BSTException) {
	insert_node(root_, value);
}

template <typename T>
void BSTree<T>::remove(const T& value) {
	delete_node(root_, value);
}

template <typename T>
void BSTree<T>::clear(void) {
	if (root_) {
		free_tree(root_);
		root_ = 0;
	}
	
}

template <typename T>
bool BSTree<T>::find(const T& value, unsigned &compares) const {

	return find_node(root_, value, compares);

}

template <typename T>
int BSTree<T>::height(void) const {
	return tree_height(root_);
}

template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root(void) const {
	return root_;
}

template <typename T>
bool BSTree<T>::empty(void) const {
	return (root_ == 0);
}

template <typename T>
unsigned int BSTree<T>::size(void) const {
	return count;
}

template <typename T>
typename BSTree<T>::BinTree& BSTree<T>::get_root(void) {
	return root_;
}

template <typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T& value) {

	BinTree node = 0;

	try{
		node = reinterpret_cast<BinTreeNode *>(oa->Allocate()); // Allocate memory for the object.
		node = new (node) BinTreeNode(value); // Construct the object in the memory.
		++count;
	}
	catch (const OAException &e){
		throw(BSTException(BSTException::E_NO_MEMORY, e.what()));
	}

	return node;
}

template <typename T>
int BSTree<T>::tree_height(BinTree tree) const {
	if (tree == 0)
		return -1;

	if (tree_height(tree->left) > tree_height(tree->right))
		return tree_height(tree->left) + 1;
	else
		return tree_height(tree->right) + 1;

}

template <typename T>
void BSTree<T>::free_node(BinTree node) {
		--count;
		node->~BinTreeNode();
		oa->Free(node);
	
}

template <typename T>
void BSTree<T>::free_tree(BinTree  node) {
	if (node) {
		free_tree(node->left);
		free_tree(node->right);
		
		if (node->left)
			node->left = 0;
		if (node->right)
			node->right= 0;
		
		free_node(node);
		node = 0;
	}
}

template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree &predecessor) const {
	predecessor = tree->left;
	while (predecessor->right != 0)
		predecessor = predecessor->right;
}

template <typename T>
void BSTree<T>::copy_tree(BinTree & lhs, BinTree rhs) {
	if (rhs) {

		lhs = make_node(rhs->data);
		lhs->count = rhs->count;
		copy_tree(lhs->left, rhs->left);
		copy_tree(lhs->right, rhs->right);
	}
	else
		lhs = 0;
}

template <typename T>
void BSTree<T>::delete_node(BinTree & Tree, const T& value) {
	if (Tree == 0)
		return;
	else if (value < Tree->data) {
		--Tree->count;
		delete_node(Tree->left, value);
	}
	else if (value > Tree->data) {
		--Tree->count;
		delete_node(Tree->right, value);
	}
	else {
		--Tree->count;
		if (!Tree->left) {
			BinTree temp = Tree;
			Tree = Tree->right;
			free_node(temp);
		}else

		if (!Tree->right) {
			BinTree temp = Tree;
			Tree = Tree->left;
			free_node(temp);
		}

		else{
			BinTree pred = 0;
			find_predecessor(Tree, pred);
			Tree->data = pred->data;
			delete_node(Tree->left, Tree->data);
		}

	}
}

template <typename T>
void BSTree<T>::insert_node(BinTree & tree, const T& value) {
	try {
		if (tree == 0) {
			tree = make_node(value);
			++tree->count;
		}

		else if (value < tree->data) {
			++tree->count;
			insert_node(tree->left, value);
		}
		else if (value > tree->data) {
			++tree->count;
			insert_node(tree->right, value);
		}
	}
	catch (const OAException &e)
	{
		throw(BSTException(BSTException::E_NO_MEMORY, e.what()));
	}
}

template <typename T>
bool BSTree<T>::find_node(BinTree tree,const T& value, unsigned& compares) const{
	
	++compares;

	if (!tree) {
		return false;
	}
	else
	if (tree->data == value) 
		return true;
	else if (value < root_->data)
		return find_node(tree->left, value, compares);
	else
		return find_node(tree->right, value, compares);
}

template <typename T>
const typename BSTree<T>::BinTreeNode* BSTree<T>::sub_node(BinTree tree, int compares) const {
	if (tree) {

		unsigned int LC = node_count(tree->left);

		if (LC > static_cast<unsigned>(compares))
			return sub_node(tree->left, compares);
		else
		if (LC < static_cast<unsigned>(compares))
			return sub_node(tree->right, compares - LC - 1);
		else
			return tree;

	}
	else
		return NULL;
}

template <typename T>
unsigned int BSTree<T>::node_count(BinTree tree) const{
	if (tree == NULL)
		return 0;

	return 1 + node_count(tree->left) + node_count(tree->right);
}

