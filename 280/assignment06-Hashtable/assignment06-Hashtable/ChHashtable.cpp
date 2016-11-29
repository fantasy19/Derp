#include "ChHashTable.h"

template<typename T>
ChHashTable<T>::ChHashTable(const HTConfig& Config,
	ObjectAllocator* allocator)
	: oa(0),
	htc(Config),
	hts(),
	head(0),
	loadFactor(0){

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
			newNode = (ChHTNode*)m_OA->Allocate();
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
		NewConfig.InitialTableSize_ = cpts;

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
		std::swap(head, NewHT.m_headnode);
		std::swap(hts, NewHT.m_stats);
		hts.Probes_ += NewHT.hts.Probes_;
		hts.Expansions_ += NewHT.m_stats.Expansions_;
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
				m_OA->Free(iterNode);
			else
				delete iterNode;

			iterNode = nextNode;
		}

		m_headnode[i].Nodes = 0;
	}

	m_stats.Count_ = 0;
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