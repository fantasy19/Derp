#include "ChHashTable.h"

template<typename T>
ChHashTable<T>::ChHashTable(const HTConfig& Config,
	ObjectAllocator* allocator)
	: oa(0),
	htc(Config),
	hts(),
	head(0),
	loadFactor(0)
{
	hts.TableSize_ = htc.InitialTableSize_;
	hts.HashFunc_ = htc.HashFunc_;

	oa = allocator;
	head = new ChHTHeadNode[hts.TableSize_];
	head->Count = htc.InitialTableSize_;

	loadFactor = static_cast<double>(hts.TableSize_) * htc.MaxLoadFactor_;
}

template<typename T>
ChHashTable<T>::~ChHashTable()
{
	clear();
	delete[] head;
}