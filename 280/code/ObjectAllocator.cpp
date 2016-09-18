/************************************************************************/
/*!
  \file ObjectAllocator.cpp
  \author Goh Zi He
  \par email: zihe.goh\@digipen.edu
  \par DigiPen login: zihe.goh
  \par Course: CS280
  \par Assignment #1
  \date 1/22/2016
  \brief
    This file contains the implementation of the following functions for
    the ObjectAllocator, OAException, OAConfig, OAStats class:

  Public Operations include:
    ObjectAllocator           (ObjectAllocator for managing memory)
    Constructor             - Construct an ObjectAllocator with the size of
    the allocator and the OAConfig info.
    Destructor              - Destruct ObjectAllocator and release memory.
    Allocate                - Allocate an object inside the object allocator.
    Free                    - Free the object inside the object allocator.
    DumpMemoryInUse         - Dump the memory that is in use.
    ValidatePages           - Check if paddings are right.
    FreeEmptyPages          - Free all the empty pages.
    ImplementedExtraCredit  - Returns true if extra credit is implemented.
    SetDebugState           - Set the debug state for the ObjectAllocator
    GetFreeList             - Get the pointer to the free list.
    GetPageList             - Get the pointer to the page list.
    GetConfig               - Get the config settings of the allocator.
    GetStats                - Get the stats info of the allocator.

  OAException               (Exceptions for ObjectAllocator)
    Constructor             - Construct an OAException with the exception type
    and message.
    Allocate                - Allocate an object inside the object allocator.
    Code                    - Get the error code.
    What                    - What is the exceptions?

  OAConfig                  (Stores the config for the ObjectAllocator)
    Constructor             - Construct an OAConfig with the the different
    parameters for the ObjectAllocator.

  OAStats                   (Stores the stats for the ObjectAllocator)
    Constructor             - Construct an OAStats with the the different
    parameters for the ObjectAllocator.

  Hours spent on this assignment: 40
  Specific portions that gave you the most trouble:
*/
/************************************************************************/
#include "ObjectAllocator.h"  /* ObjectAllocator, OAConfig, OAException, OAStats */
#include <cstring>				    /* memset */

/******************************************************************************/
/*!
  ObjectAllocator constructor.

  \param ObjectSize
    Set the object size for the object allocator.

  \param config
    Set the config for the ObjectAllocator.
*/
/******************************************************************************/
ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig &config) throw(OAException)
	: numPages(0), freeListPtr(nullptr), pageListPtr(nullptr), conf(config),
	objectSize(ObjectSize), pageSize(ComputePageSize(conf))
{
	// Reset the stats
	stats.PagesInUse_ = 0;
	stats.ObjectSize_ = objectSize;
	stats.PageSize_ = pageSize;
	stats.ObjectsInUse_ = 0;

	// Allocate a page
	AllocatePage();
}

/******************************************************************************/
/*!
  ObjectAllocator destructor.
*/
/******************************************************************************/
ObjectAllocator::~ObjectAllocator() throw()
{
	// Free all pages here
	GenericObject *page = pageListPtr;
	while (page)
	{
		// If external header block, free it
		if (conf.HBlockInfo_.type_ == conf.hbExternal)
		{
			char *start = reinterpret_cast<char *>(page) + sizeof(void *) +
				conf.LeftAlignSize_;

			// Go through the page and free the headers
			for (size_t i = 0; i < conf.ObjectsPerPage_; ++i)
			{
				// Get the pointer to the header
				MemBlockInfo **header = reinterpret_cast<MemBlockInfo **>(
					start +
					(i * (objectSize + conf.HBlockInfo_.size_ + (conf.PadBytes_ << 1) +
						  conf.InterAlignSize_)));

				// delete the headers and the labels
				if (*header)
				{
					if ((*header)->label)
						delete[](*header)->label;
					delete *header;
				}
			}
		}

		// Delete the page
		GenericObject *toBeDelete = page;
		page = page->Next;
		delete[] toBeDelete;
	}
}

/******************************************************************************/
/*!
  Allocate memory and returns a pointer.

  \param label
    The label to set the external header.

  \return
    A pointer from the page within the allocator is returned if there is enough space.
*/
/******************************************************************************/
void *ObjectAllocator::Allocate(const char *label) throw(OAException)
{
	if (!stats.FreeObjects_ && !conf.UseCPPMemManager_)
	{
		// Try to allocate more pages
		AllocatePage();
	}

	// Increment the allocation stats
	++stats.Allocations_;
	++stats.MostObjects_;
	++stats.ObjectsInUse_;
	--stats.FreeObjects_;

	void *free = nullptr;

	// Only use new when UseCPPMemManager_ is true
	if (conf.UseCPPMemManager_)
	{
		free = new char[objectSize];
	}
	else
	{
		// Otherwise get the pointer from the freeList
		free = freeListPtr;
		freeListPtr = freeListPtr->Next;
	}

	// Set the allocated pattern for the page
	MemSet(free, ALLOCATED_PATTERN, objectSize);

	// Set the header info for the pages
	if (conf.HBlockInfo_.type_ == conf.hbBasic)
	{
		BasicHeader *header = reinterpret_cast<BasicHeader *>(
			reinterpret_cast<char *>(free) - conf.PadBytes_ - conf.BASIC_HEADER_SIZE);

		header->allocationNum = stats.Allocations_;
		header->inUse = 1;
	}
	else if (conf.HBlockInfo_.type_ == conf.hbExtended)
	{
		char *header = reinterpret_cast<char *>(free) - conf.PadBytes_ - conf.HBlockInfo_.size_;

		// Set 0 for flags for now
		memset(header, 0, conf.HBlockInfo_.additional_);

		// Increment the use counter
		++*(reinterpret_cast<unsigned short *>(header + conf.HBlockInfo_.additional_));

		// Set the allocation #
		*(reinterpret_cast<unsigned *>(header + conf.HBlockInfo_.additional_ + sizeof(unsigned short))) = stats.Allocations_;

		// Set the use flags
		*(reinterpret_cast<char *>(header + conf.HBlockInfo_.additional_ + sizeof(unsigned short) + sizeof(unsigned))) = 1;
	}
	else if (conf.HBlockInfo_.type_ == conf.hbExternal)
	{
		MemBlockInfo *memBlockInfo = nullptr;
		try
		{
			// Allocate new MemBlockInfo for external
			memBlockInfo = new MemBlockInfo();
			memBlockInfo->alloc_num = stats.Allocations_;
			memBlockInfo->in_use = 1;

			// Allocate memory for label
			if (label)
			{
				size_t len = strlen(label) + 1;
				memBlockInfo->label = new char[len];
				strcpy(memBlockInfo->label, label);
			}
			else
			{
				memBlockInfo->label = nullptr;
			}
		}
		catch (std::bad_alloc)
		{
			delete memBlockInfo;
		}

		// Set the header to point to the created MemBlockInfo
		MemBlockInfo **ptr = reinterpret_cast<MemBlockInfo **>(reinterpret_cast<char *>(free) - conf.PadBytes_ - conf.EXTERNAL_HEADER_SIZE);
		*ptr = memBlockInfo;
	}

	return free;
}

/******************************************************************************/
/*!
  Free the pointer

  \param Object
    The object to free.
*/
/******************************************************************************/
void ObjectAllocator::Free(void *Object) throw(OAException)
{
	// Check if the pointer is already in the free list
	if (conf.DebugOn_ && IsInFreeList(Object))
		throw OAException(OAException::OA_EXCEPTION::E_MULTIPLE_FREE, "Free multiple times");

	// Update the stats
	++stats.Deallocations_;
	--stats.ObjectsInUse_;
	++stats.FreeObjects_;

	// Use allocator
	if (!conf.UseCPPMemManager_)
	{
		GenericObject *page = pageListPtr;
		while (page)
		{
			// Is on this page
			if (Object >= reinterpret_cast<char *>(page) && reinterpret_cast<char *>(Object) + objectSize <= reinterpret_cast<char *>(page) + stats.PageSize_)
			{
				// If it is within the bounds
				size_t left = (reinterpret_cast<char *>(Object) - reinterpret_cast<char *>(page)) - (ptrSize + conf.LeftAlignSize_ + conf.HBlockInfo_.size_ + conf.PadBytes_);
				size_t perBlockSize = objectSize + (conf.PadBytes_ << 1) + conf.InterAlignSize_ + conf.HBlockInfo_.size_;
				
        // If debug is off, bypass boundary checks
        if (!(left % perBlockSize) || !conf.DebugOn_)
				{
					// Deallocate
					GenericObject *current = reinterpret_cast<GenericObject *>(Object);
					MemSet(Object, FREED_PATTERN, objectSize);

					// Set the free list ptr
					current->Next = freeListPtr;
					freeListPtr = current;

					// Set the header info
					if (conf.HBlockInfo_.type_ == conf.hbBasic)
					{
						BasicHeader *header = reinterpret_cast<BasicHeader *>(
							reinterpret_cast<char *>(Object) - conf.PadBytes_ - conf.BASIC_HEADER_SIZE);

						header->allocationNum = 0;
						header->inUse = 0;
					}
					else if (conf.HBlockInfo_.type_ == conf.hbExtended)
					{
						char *header = reinterpret_cast<char *>(Object) - conf.PadBytes_ - conf.HBlockInfo_.size_;
            
            // Reset Header flags
            *(reinterpret_cast<char *>(header)) = 0;
						*(reinterpret_cast<unsigned *>(header + conf.HBlockInfo_.additional_ + sizeof(unsigned short))) = 0;
						*(reinterpret_cast<char *>(header + conf.HBlockInfo_.additional_ + sizeof(unsigned short) + sizeof(unsigned))) = 0;
					}
					else if (conf.HBlockInfo_.type_ == conf.hbExternal)
					{
						MemBlockInfo **header = reinterpret_cast<MemBlockInfo **>(
							reinterpret_cast<char *>(Object) - conf.PadBytes_ - conf.EXTERNAL_HEADER_SIZE);

						// Delete the external label and the header info
						delete[](*header)->label;
						delete *header;

						*header = nullptr;
					}

					// Only check for paddings when debug on is off
					if (conf.DebugOn_)
					{
						unsigned char *leftPad = reinterpret_cast<unsigned char *>(Object) - conf.PadBytes_;
						unsigned char *rightPad = reinterpret_cast<unsigned char *>(Object) + objectSize;

						if (!IsPadSafe(leftPad) || !IsPadSafe(rightPad))
						{
							throw OAException(OAException::OA_EXCEPTION::E_CORRUPTED_BLOCK, "Corrupted bound");
						}
					}
				}
				else
				{
					throw OAException(OAException::OA_EXCEPTION::E_BAD_BOUNDARY, "Bad bound");
				}
				return;
			}
			page = page->Next;
		}
    
    throw OAException(OAException::OA_EXCEPTION::E_NO_PAGES, "Out of page");
	}
	else
	{
		delete[] reinterpret_cast<char *>(Object);
		return;
	}
}

/******************************************************************************/
/*!
  Call the callback fn whenever the object is not in the free list.

  \param fn
    The call back function.

  \return
    Return the number of allocated memory.
*/
/******************************************************************************/
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const
{
	unsigned numAllocated = 0;
	// Iterate through the pages
	if (fn && conf.DebugOn_)
	{
		GenericObject *page = pageListPtr;
		while (page)
		{
			// Iterate through the pages and check if it is in the free list
			char *start = reinterpret_cast<char *>(page) + ptrSize + conf.LeftAlignSize_ + conf.HBlockInfo_.size_ + conf.PadBytes_;
			for (size_t i = 0; i < conf.ObjectsPerPage_; ++i)
			{
				char *obj = start + (i * (objectSize + conf.HBlockInfo_.size_ + (conf.PadBytes_ << 1) + conf.InterAlignSize_));
				if (!IsInFreeList(obj))
				{
					// Call the function
					++numAllocated;
					fn(obj, objectSize);
				}
			}
			page = page->Next;
		}
	}
	return numAllocated;
}

/******************************************************************************/
/*!
  Call the callback fn whenever the object has bad paddings.

  \param fn
    The call back function.

  \return
    Return the number of corrupted memory.
*/
/******************************************************************************/
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const
{
	unsigned numCorruption = 0;
	// Iterate through the pages when paddings is valid
	if (fn && conf.DebugOn_ && conf.PadBytes_ > 0)
	{
		GenericObject *page = pageListPtr;
		while (page)
		{
			// Point to the start of the page after the pointer size
			unsigned char *start = reinterpret_cast<unsigned char *>(page) + sizeof(void *) + conf.LeftAlignSize_ + conf.HBlockInfo_.size_;
			for (size_t i = 0; i < conf.ObjectsPerPage_; ++i)
			{
				unsigned char *leftPad = start + (i * (objectSize + conf.HBlockInfo_.size_ + (conf.PadBytes_ << 1) + conf.InterAlignSize_));
				unsigned char *rightPad = leftPad + conf.PadBytes_ + objectSize;

				// Check if the padding is correct
				if (!IsPadSafe(leftPad) || !IsPadSafe(rightPad))
				{
					++numCorruption;
					fn(leftPad + conf.PadBytes_, objectSize);
				}
			}
			page = page->Next;
		}
	}
	return numCorruption;
}

/******************************************************************************/
/*!
  Free the empty pages.

  \return
    Return the number of freed pages.
*/
/******************************************************************************/
unsigned ObjectAllocator::FreeEmptyPages()
{
	unsigned pagesFreed = 0;

	// Iterate through the pages
	GenericObject *currentPage = pageListPtr;
	while (currentPage)
	{
		// Check to see if any blocks in the page is in the free list
		bool freePage = true;
		{
			char *start = reinterpret_cast<char *>(currentPage) + ptrSize + conf.LeftAlignSize_ + conf.HBlockInfo_.size_ + conf.PadBytes_;
			for (size_t i = 0; i < conf.ObjectsPerPage_; ++i)
			{
				char *obj = start + (i * (objectSize + conf.HBlockInfo_.size_ + (conf.PadBytes_ << 1) + conf.InterAlignSize_));
				if (!IsInFreeList(obj))
				{
					freePage = false;
					break;
				}
			}
		}

		// Can free the page
		if (freePage)
		{
			// Point the freeList to the prevPage or nextPage
			char *start = reinterpret_cast<char *>(currentPage) + sizeof(void *) + conf.LeftAlignSize_ + conf.HBlockInfo_.size_ + conf.PadBytes_;

			// Free the external headers and remove the nodes from the free list
			for (size_t i = 0; i < conf.ObjectsPerPage_; ++i)
			{
				if (conf.HBlockInfo_.type_ == conf.hbExternal)
				{
					MemBlockInfo **header = reinterpret_cast<MemBlockInfo **>(start + (i * (objectSize + conf.HBlockInfo_.size_ + (conf.PadBytes_ << 1) + conf.InterAlignSize_)));
					if (*header)
					{
						if ((*header)->label)
							delete[](*header)->label;
						delete *header;
					}
				}

				GenericObject *obj = reinterpret_cast<GenericObject *>(start + (i * (objectSize + conf.HBlockInfo_.size_ + (conf.PadBytes_ << 1) + conf.InterAlignSize_)));
				if (RemoveNode(&freeListPtr, obj))
				{
					--stats.FreeObjects_;
				}
			}

			// Keep the old page
			GenericObject *old = currentPage;
			currentPage = currentPage->Next;

			// Reassign page list head if head is removed
			if (old == pageListPtr)
				pageListPtr = currentPage;

			// Remove the page
			RemoveNode(&pageListPtr, old);
			--stats.PagesInUse_;
			++pagesFreed;

			delete[] old;
		}
		else
		{
			currentPage = currentPage->Next;
		}
	}
	return pagesFreed;
}

/******************************************************************************/
/*!
  Return true if extra credit is implemented.

  \return
    True if extra credit is implemented.
*/
/******************************************************************************/
bool ObjectAllocator::ImplementedExtraCredit()
{
	return true;
}

/******************************************************************************/
/*!
  Set the debug state for the object allocator.

  \param State
    The debug state of the object allocator.
*/
/******************************************************************************/
void ObjectAllocator::SetDebugState(bool State)
{
	conf.DebugOn_ = State;
}

/******************************************************************************/
/*!
  Get a pointer to the free list.

  \return
    The pointer to the free list.
*/
/******************************************************************************/
const void *ObjectAllocator::GetFreeList() const
{
	return freeListPtr;
}

/******************************************************************************/
/*!
  Get a pointer to the page list.

  \return
    The pointer to the page list.
*/
/******************************************************************************/
const void *ObjectAllocator::GetPageList() const
{
	return pageListPtr;
}

/******************************************************************************/
/*!
  Get the config of the object allocator.

  \return
    The configuration of the object allocator.
*/
/******************************************************************************/
OAConfig ObjectAllocator::GetConfig() const
{
	return conf;
}

/******************************************************************************/
/*!
  Get the stats of the object allocator.

  \return
    The stats of the object allocator.
*/
/******************************************************************************/
OAStats ObjectAllocator::GetStats() const
{
	return stats;
}

/******************************************************************************/
/*!
  Helper fn to allocate a new page
*/
/******************************************************************************/
void ObjectAllocator::AllocatePage() throw(OAException)
{
	// Exceed max page to allocate
	if (stats.PagesInUse_ == conf.MaxPages_)
		throw OAException(OAException::OA_EXCEPTION::E_NO_PAGES, "No pages left");

	++numPages;

	// Allocate more page here
	char *page = nullptr;
	try
	{
		page = new char[pageSize];
	}
	catch (std::bad_alloc)
	{
		throw OAException(OAException::OA_EXCEPTION::E_NO_MEMORY, "No memory");
	}

	// Set the pattern for unallocated and alignment
	MemSet(page, UNALLOCATED_PATTERN, pageSize);
	MemSet(page + ptrSize, ALIGN_PATTERN, conf.LeftAlignSize_);

	// Set up the page list
	if (!pageListPtr)
	{
		pageListPtr = reinterpret_cast<GenericObject *>(page);
		pageListPtr->Next = nullptr;
	}
	else
	{
		GenericObject *currentPage = reinterpret_cast<GenericObject *>(page);
		currentPage->Next = pageListPtr;
		pageListPtr = currentPage;
	}

	// Set up pointer to point to just before headers
	GenericObject *ptr = nullptr;
	char *start = page + ptrSize + conf.LeftAlignSize_;
	const size_t blockSize = objectSize + (conf.PadBytes_ << 1) + conf.HBlockInfo_.size_ + conf.InterAlignSize_;

	for (size_t i = 0; i < conf.ObjectsPerPage_; ++i)
	{
		// Point to before header
		char *tmp = start + (i * blockSize);

		// Set up headers
		if (conf.HBlockInfo_.type_ == conf.hbBasic)
		{
			BasicHeader *header = reinterpret_cast<BasicHeader *>(tmp);
			header->allocationNum = 0;
			header->inUse = 0;
		}
		else if (conf.HBlockInfo_.type_ == conf.hbExtended)
		{
			memset(tmp, 0, conf.HBlockInfo_.size_);
			*(reinterpret_cast<unsigned short *>(tmp + conf.HBlockInfo_.additional_)) = 0;
			*(reinterpret_cast<unsigned *>(tmp + conf.HBlockInfo_.additional_ + sizeof(unsigned short))) = 0;
			*(reinterpret_cast<char *>(tmp + conf.HBlockInfo_.additional_ + sizeof(unsigned short) + sizeof(unsigned))) = 0;
		}
		else if (conf.HBlockInfo_.type_ == conf.hbExternal)
		{
			MemSet(tmp, 0, conf.EXTERNAL_HEADER_SIZE);
		}

		// Padding
		MemSet(tmp + conf.HBlockInfo_.size_, PAD_PATTERN, conf.PadBytes_);

		// Set up free list
		GenericObject *data = reinterpret_cast<GenericObject *>(tmp + conf.HBlockInfo_.size_ + conf.PadBytes_);
		data->Next = ptr;
		ptr = data;
		freeListPtr = data;

		// Padding
		MemSet(tmp + conf.HBlockInfo_.size_ + conf.PadBytes_ + objectSize, PAD_PATTERN, conf.PadBytes_);

		// Don't memset inter align once we reach the last object
		if (i == conf.ObjectsPerPage_ - 1)
			continue;

		MemSet(tmp + conf.HBlockInfo_.size_ + (conf.PadBytes_ << 1) + objectSize, ALIGN_PATTERN, conf.InterAlignSize_);
	}

	++stats.PagesInUse_;
	stats.FreeObjects_ = conf.ObjectsPerPage_;
}

/******************************************************************************/
/*!
  Return true if pad is not corrupted.

  \param ptr
    The ptr to the padding.

  \return
    True if pad is correct.
*/
/******************************************************************************/
bool ObjectAllocator::IsPadSafe(unsigned char *ptr) const
{
	for (size_t i = 0; i < conf.PadBytes_; ++i)
	{
		if (*(ptr + i) != PAD_PATTERN)
			return false;
	}
	return true;
}

/******************************************************************************/
/*!
  Helper fn for memset for assigning signatures. Will not memset if debug is off.

  \param ptr
    The ptr to the data.

  \param val
    The val to memset.

  \param sz
    The size of the data to write.
*/
/******************************************************************************/
void ObjectAllocator::MemSet(void *ptr, int val, size_t sz)
{
	if (conf.DebugOn_)
		memset(ptr, val, sz);
}

/******************************************************************************/
/*!
  Helper fn to check if an object is inside the free list.

  \param ptr
    The ptr to the data.

  \return
    Return true if the object is in the free list.
*/
/******************************************************************************/
bool ObjectAllocator::IsInFreeList(void *ptr) const
{
	// Traverse free list
	GenericObject *object = freeListPtr;
	while (object)
	{
		// Compare ptr with the object in the free list
		if (reinterpret_cast<GenericObject *>(ptr) == object)
			return true;
		object = object->Next;
	}
	return false;
}

/******************************************************************************/
/*!
  Remove the node from a linked list

  \param head
    The pointer to the head node.

  \param ptr
    The pointer to be removed.

  \return
    True if the node is found and removed
*/
/******************************************************************************/
bool ObjectAllocator::RemoveNode(GenericObject **head, GenericObject *ptr) const
{
	// Hold the current node for traversiong
	GenericObject *current = *head;

	// Hold the prev node
	GenericObject *prev = nullptr;

	// While node is valid
	while (current)
	{
		// Check if it is the pointer
		if (current == ptr)
		{
			// If the head is the same as ptr, we are removing the head, set head to the next ptr
			if (*head == ptr)
				*head = (*head)->Next;

			// If the prev is valid, link it to the next node.
			if (prev)
				prev->Next = current->Next;

			return true;
		}

		prev = current;
		current = current->Next;
	}
	return false;
}

/******************************************************************************/
/*!
  Helper fn to compute the page size.

  \param config
    Set the position of the chess piece.

  \return
    The size of the page
*/
/******************************************************************************/
size_t ObjectAllocator::ComputePageSize(OAConfig &config) const
{
	// Set some constants
	const size_t PAD_SZ = config.PadBytes_;
	const size_t HEADER_SZ = config.HBlockInfo_.size_;
	const unsigned NUM_OBJ = config.ObjectsPerPage_;
	const unsigned ALIGNMENT = config.Alignment_;

	// Need to check if this number % objectSize
	unsigned start = static_cast<unsigned>(ptrSize + PAD_SZ + HEADER_SZ);
	unsigned end = static_cast<unsigned>(objectSize + (PAD_SZ << 1) + HEADER_SZ);

	// Compute the left and inter align sizes
	const unsigned LEFT_ALIGN_SZ = (ALIGNMENT == 0) ? 0 : ((ALIGNMENT - (start % ALIGNMENT)) == ALIGNMENT) ? 0 : (ALIGNMENT - (start % ALIGNMENT));
	const unsigned INTER_ALIGN_SZ = (ALIGNMENT == 0) ? 0 : ((ALIGNMENT - (end % ALIGNMENT)) == ALIGNMENT) ? 0 : (ALIGNMENT - (end % ALIGNMENT));

	// Set the align sizes inside the config
	config.LeftAlignSize_ = LEFT_ALIGN_SZ;
	config.InterAlignSize_ = INTER_ALIGN_SZ;

	return (
		ptrSize +
		LEFT_ALIGN_SZ +
		((NUM_OBJ - 1) * INTER_ALIGN_SZ) +
		((HEADER_SZ + (PAD_SZ << 1) + objectSize) * NUM_OBJ));
}
