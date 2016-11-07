

template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *OA, bool ShareOA) : oa(OA), share(ShareOA) {
	if (!oa && !share)
		oa = new ObjectAllocator(sizeof(AVLTree), OAConfig());
}
