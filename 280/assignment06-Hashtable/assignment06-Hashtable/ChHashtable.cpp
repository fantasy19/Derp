/****************************************************************************/
/*!
\file   ChHashTable.cpp
\author Ang Cheng Yong
\par    email: a.chengyong\@digipen.edu
\par    DigiPen login: a.chengyong
\par    Course: CS280
\par    Programming Assignment #6
\date   3/12/2016
\brief
This file contains the implememtation needed for dealing with hashtable
*/
/****************************************************************************/

/***************************************************************************/
/*!
\fn template<typename T> ChHashTable<T>::ChHashTable(const HTConfig& Config, ObjectAllocator* allocator)
\brief
constructor for hashtable

\param Config
config to set the new hashtable

\param allocator
object allocator used for hashtable

\return
none
*/
/***************************************************************************/
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

/***************************************************************************/
/*!
\fn template<typename T> ChHashTable<T>::~ChHashTable()
\brief
destructor for hashtable

\return
none
*/
/***************************************************************************/

template<typename T>
ChHashTable<T>::~ChHashTable(){
	clear();
	delete[] head;
}

/***************************************************************************/
/*!
\fn template<typename T> void ChHashTable<T>::insert(const char *Key, const T& Data) throw(HashTableException)
\brief
insert a new unique value to hashtable and grow should the table run out of space

\param Key
the key to be used for the pair value

\param Data
value to be inserted to be identified by the key

\return
none
*/
/***************************************************************************/

template<typename T>
void ChHashTable<T>::insert(const char *Key, const T& Data) throw(HashTableException){

	ChHTNode* newNode = 0, * matchingNode = 0;
	bool match = false;

	if (hts.Count_ + 1 > loadFactor){
		try{
			Grow(); // expand table if necessary
		}
		catch (const HashTableException& e){
			throw e;
		}

		loadFactor = static_cast<double>(hts.TableSize_) * htc.MaxLoadFactor_;
	}

	// using pre-existing  hashing functions given by the driver
	unsigned i = hts.HashFunc_(Key, hts.TableSize_);

	matchingNode = head[i].Nodes;
	++hts.Probes_;

	// search for duplicate node
	while (matchingNode != NULL) {
		++hts.Probes_;
		if (!std::strcmp(matchingNode->Key, Key))
			match = true;

		matchingNode = matchingNode->Next;
	}

	if (match)
		throw HashTableException(HashTableException::E_DUPLICATE, "Duplicate key.");
	else { // a unique value found, insert it to table
		try {
			newNode = MakeNode(Key, Data);
			
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

/***************************************************************************/
/*!
\fn template<typename T> typename ChHashTable<T>::ChHTNode* ChHashTable<T>::MakeNode(const char *Key, const T& Data)
\brief
Make a new node if insertion is required

\param Key
the key to be used for the new node

\param Data
value to be inserted for the new node

\return
none
*/
/***************************************************************************/

template<typename T>
typename ChHashTable<T>::ChHTNode* ChHashTable<T>::MakeNode(const char *Key, const T& Data)
throw(HashTableException){
	ChHTNode* newNode = 0;
	try{

		if (!oa) // use objectAllocator where applicable
			newNode = new ChHTNode(Data);
		else{
			newNode = (ChHTNode*)oa->Allocate();
			newNode->Data = Data;
		}

		std::strcpy(newNode->Key, Key); // deep copy is required
		newNode->Next = 0;
	}
	catch (const HashTableException& e){
		throw HashTableException(e.code(), e.what());
	}

	return newNode;
}

/***************************************************************************/
/*!
\fn template<typename T> void ChHashTable<T>::Grow() throw(HashTableException)
\brief
Expands the hashtable

\return
none
*/
/***************************************************************************/

template<typename T>
void ChHashTable<T>::Grow() throw(HashTableException){

	double factor = std::ceil(hts.TableSize_ * htc.GrowthFactor_);

	try{
		HTConfig newHtc = htc;
		newHtc.InitialTableSize_ = GetClosestPrime(static_cast<unsigned>(factor));

		ChHashTable<T> NewHT(newHtc, oa);

		// insert happening for per row in table
		for (unsigned i = 0; i < hts.TableSize_; ++i){
			ChHTNode* currNode = head[i].Nodes;

			while (currNode){ // inserting existing nodes
				NewHT.insert(currNode->Key, currNode->Data);
				currNode = currNode->Next;
			}
		}

		clear();

		// adjust statistics
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

/***************************************************************************/
/*!
\fn template<typename T> void ChHashTable<T>::clear(void)
\brief
clear all the chains of the hashtable

\return
none
*/
/***************************************************************************/

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

/***************************************************************************/
/*!
\fn template<typename T> void ChHashTable<T>::remove(const char *Key) throw(HashTableException)
\brief
remove an element in the hashtable

\param Key
the key to be matched for removal

\return
none
*/
/***************************************************************************/

template<typename T>
void ChHashTable<T>::remove(const char *Key) throw(HashTableException){
	unsigned i = hts.HashFunc_(Key, hts.TableSize_);

	ChHTNode* currNode = head[i].Nodes, *prevNode = head[i].Nodes;

	while (currNode) {
		++hts.Probes_;
		if (!std::strcmp(currNode->Key, Key)) {
			// removes the element as though its in a linked list
			if (currNode && prevNode == currNode) { // should the matching element be the first one
				head[i].Nodes = currNode->Next;
				RemoveNode(currNode);
			}
			else { 
				if (currNode && prevNode){
					prevNode->Next = currNode->Next;
					RemoveNode(currNode);
				}
			}
			return;
		}

		prevNode = iterNode;
		iterNode = iterNode->Next;
	}
}

/***************************************************************************/
/*!
\fn template<typename T> const T& ChHashTable<T>::find(const char *Key) const throw(HashTableException)
\brief
find an element in the hashtable

\param Key
the key to be matched

\return
none
*/
/***************************************************************************/

template<typename T>
const T& ChHashTable<T>::find(const char *Key) const throw(HashTableException)
{
	unsigned i = hts.HashFunc_(Key, hts.TableSize_);
	ChHTNode* currNode = head[i].Nodes;

	while (currNode)
	{
		++hts.Probes_;

		if (!std::strcmp(currNode->Key, Key))
			return currNode->Data;
		
		currNode = currNode->Next;
	}

	throw HashTableException(HashTableException::E_ITEM_NOT_FOUND,
		"Item Not Found");
}

/***************************************************************************/
/*!
\fn template<typename T> const typename ChHashTable<T>::ChHTHeadNode * ChHashTable<T>::GetTable(void) const
\brief
get the table by getting its first chain

\return
first node in the first chain of hashtable
*/
/***************************************************************************/

template<typename T>
const typename ChHashTable<T>::ChHTHeadNode * ChHashTable<T>::GetTable(void) const
{
	return head;
}

/***************************************************************************/
/*!
\fn template<typename T> HTStats ChHashTable<T>::GetStats(void) const
\brief
get the statistics of hashtable

\return
stats of hashtable
*/
/***************************************************************************/

template<typename T>
HTStats ChHashTable<T>::GetStats(void) const
{
	return hts;
}

/***************************************************************************/
/*!
\fn template<typename T> void ChHashTable<T>::RemoveNode(ChHTNode* deletePtr)
\brief
check if removing requires driver function or object allocator

\param deletePtr
the node to be freed

\return
none
*/
/***************************************************************************/

template<typename T>
void ChHashTable<T>::RemoveNode(ChHTNode* deletePtr) {
	if (htc.FreeProc_)
		htc.FreeProc_(deletePtr->Data);

	if (oa)
		oa->Free(deletePtr);
	else
		delete deletePtr;
	
	--hts.Count_;
}