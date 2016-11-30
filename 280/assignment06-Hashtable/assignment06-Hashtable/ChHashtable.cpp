
template<typename T>
ChHashTable<T>::ChHashTable(const HTConfig& Config, ObjectAllocator* allocator) : 
	oa(0), htc(Config), hts(), head(0), loadFactor(0){

	hts.TableSize_ = htc.InitialTableSize_;
	hts.HashFunc_ = htc.HashFunc_;

	oa = allocator;
	head = new ChHTHeadNode[hts.TableSize_];
	head->Count = htc.InitialTableSize_;

	loadFactor = static_cast<double>(hts.TableSize_) * htc.MaxLoadFactor_;

}

template<typename T>
ChHashTable<T>::~ChHashTable(){
	clear();
	delete[] head;
}

template<typename T>
void ChHashTable<T>::insert(const char *Key, const T& Data) throw(HashTableException){

	ChHTNode* newNode;

	if (hts.Count_ + 1 > loadFactor){
		try{
			GrowTable();
		}
		catch (const HashTableException& e){
			throw e;
		}

		loadFactor = static_cast<double>(hts.TableSize_) * htc.MaxLoadFactor_;
	}

	// using pre-existing  hashing functions given by the driver
	unsigned i = hts.HashFunc_(Key, hts.TableSize_);

	if (IsSameNode(Key, i)) 
		throw HashTableException(HashTableException::E_DUPLICATE, "Duplicate key.");
	else {
		try {
			newNode = CreateNode(Key, Data);
			//whether inside of head node is empty
			if (!head[i].Nodes)
				head[i].Nodes = newNode;
			else{
				newNode->Next = head[i].Nodes;
				head[i].Nodes = newNode;
			}
			++hts.Count_;
		}
		catch (const HashTableException& e) {
			throw e;
		}
	}
}

template<typename T>
typename ChHashTable<T>::ChHTNode* ChHashTable<T>::CreateNode(const char *Key, const T& Data)
throw(HashTableException){
	ChHTNode* newNode;
	try{

		if (!oa)
			newNode = new ChHTNode(Data);
		else{
			newNode = (ChHTNode*)oa->Allocate();
			newNode->Data = Data;
		}

		std::strcpy(newNode->Key, Key);
		newNode->Next = 0;
	}
	catch (const HashTableException& e){
		throw HashTableException(e.code(), e.what());
	}

	return newNode;
}

template<typename T>
void ChHashTable<T>::GrowTable() throw(HashTableException){

	double factor = std::ceil(hts.TableSize_ * htc.GrowthFactor_);
	unsigned cpts = GetClosestPrime(static_cast<unsigned>(factor));

	try{
		HTConfig newHtc = htc;
		newHtc.InitialTableSize_ = cpts;

		ChHashTable<T> NewHT(newHtc, oa);

		//insert happening for per row in table
		for (unsigned i = 0; i < hts.TableSize_; ++i){
			ChHTNode* iterNode = head[i].Nodes;

			while (iterNode){ // inserting existing nodes
				NewHT.insert(iterNode->Key, iterNode->Data);
				iterNode = iterNode->Next;
			}
		}

		clear();

		++hts.Expansions_;
		std::swap(head, NewHT.head);
		std::swap(hts, NewHT.hts);
		hts.Probes_ += NewHT.hts.Probes_;
		hts.Expansions_ += NewHT.hts.Expansions_;
	}
	catch (const OAException& e){
		throw HashTableException(e.code(), e.what());
	}
}

template<typename T>
void ChHashTable<T>::clear(void){

	for (unsigned i = 0; i < hts.TableSize_; ++i){
		ChHTNode* iterNode = head[i].Nodes;

		while (iterNode){
			ChHTNode* nextNode = iterNode->Next;

			if (oa)
				oa->Free(iterNode);
			else
				delete iterNode;

			iterNode = nextNode;
		}

		head[i].Nodes = 0;
	}

	hts.Count_ = 0;
}

template<typename T>
bool ChHashTable<T>::IsSameNode(const char* Key, const unsigned& index)
{
	ChHTNode* checkNode = head[index].Nodes;
	++hts.Probes_;

	//while loop the list in that head node
	while (checkNode){
		++hts.Probes_;
		if (!std::strcmp(checkNode->Key, Key))
			return true;

		checkNode = checkNode->Next;
	}

	return false;
}

template<typename T>
void ChHashTable<T>::remove(const char *Key) throw(HashTableException){
	unsigned i = hts.HashFunc_(Key, hts.TableSize_);

	ChHTNode* iterNode = head[i].Nodes, *prevNode = head[i].Nodes;
	while (iterNode) {
		++hts.Probes_;
		if (!std::strcmp(iterNode->Key, Key)) {
			if (iterNode && prevNode == iterNode) {
				head[i].Nodes = iterNode->Next;
				DeleteNode(iterNode);
			}
			else {
				if (iterNode && prevNode){
					prevNode->Next = iterNode->Next;
					DeleteNode(iterNode);
				}
			}
			return;
		}

		prevNode = iterNode;
		iterNode = iterNode->Next;
	}
}

template<typename T>
const T& ChHashTable<T>::find(const char *Key) const throw(HashTableException)
{
	unsigned i = hts.HashFunc_(Key, hts.TableSize_);
	ChHTNode* iterNode = head[i].Nodes;

	while (iterNode)
	{
		++hts.Probes_;

		if (!std::strcmp(iterNode->Key, Key))
			return iterNode->Data;
		
		iterNode = iterNode->Next;
	}

	throw HashTableException(HashTableException::E_ITEM_NOT_FOUND,
		"Item Not Found");
}

template<typename T>
const typename ChHashTable<T>::ChHTHeadNode * ChHashTable<T>::GetTable(void) const
{
	return head;
}

template<typename T>
HTStats ChHashTable<T>::GetStats(void) const
{
	return hts;
}

template<typename T>
void ChHashTable<T>::DeleteNode(ChHTNode* deletePtr) {
	if (htc.FreeProc_)
		htc.FreeProc_(deletePtr->Data);

	if (oa)
		oa->Free(deletePtr);
	else
		delete deletePtr;
	
	--hts.Count_;
}