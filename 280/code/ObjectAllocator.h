/************************************************************************/
/*!
  \file ObjectAllocator.h
  \author Goh Zi He
  \par email: zihe.goh\@digipen.edu
  \par DigiPen login: zihe.goh
  \par Course: CS280
  \par Assignment #1
  \date 1/22/2016
  \brief
    This file contains the declaration of the following functions for
    the ObjectAllocator, OAException, OAConfig, OAStats class:

    Public Operations include:
    ObjectAllocator           (ObjectAllocator for managing memory)
      Constructor             - Construct an ObjectAllocator with the size of the allocator and the OAConfig info.
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
      Constructor             - Construct an OAException with the exception type and message.
      Allocate                - Allocate an object inside the object allocator.
      Code                    - Get the error code.
      What                    - What is the exceptions?
      
    OAConfig                  (Stores the config for the ObjectAllocator)
      Constructor             - Construct an OAConfig with the the different parameters for the ObjectAllocator.
      
    OAStats                   (Stores the stats for the ObjectAllocator)
      Constructor             - Construct an OAStats with the the different parameters for the ObjectAllocator.
    
    Hours spent on this assignment: 40
    Specific portions that gave you the most trouble: 
*/
/************************************************************************/
//---------------------------------------------------------------------------
#ifndef OBJECTALLOCATORH
#define OBJECTALLOCATORH
//---------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning( disable : 4290 ) // suppress warning: C++ Exception Specification ignored
#endif

#include <string>
#include <iostream>

// If the client doesn't specify these:
static const int DEFAULT_OBJECTS_PER_PAGE = 4;  
static const int DEFAULT_MAX_PAGES = 3;

/******************************************************************************/
/*!
  \class OAException
  \brief  
    Custom exceptions for ObjectAllocator.
*/
/******************************************************************************/
class OAException
{
  public:
      // Possible exception codes
    enum OA_EXCEPTION 
    {
      E_NO_MEMORY,      // out of physical memory (operator new fails)
      E_NO_PAGES,       // out of logical memory (max pages has been reached)
      E_BAD_BOUNDARY,   // block address is on a page, but not on any block-boundary
      E_MULTIPLE_FREE,  // block has already been freed
      E_CORRUPTED_BLOCK // block has been corrupted (pad bytes have been overwritten)
    };

    OAException(OA_EXCEPTION ErrCode, const std::string& Message) : error_code_(ErrCode), message_(Message) {};

    virtual ~OAException() {
    }

    OA_EXCEPTION code(void) const { 
      return error_code_; 
    }

    virtual const char *what(void) const {
      return message_.c_str();
    }
  private:  
    OA_EXCEPTION error_code_;
    std::string message_;
};

/******************************************************************************/
/*!
  \struct OAConfig
  \brief  
    Configurations for ObjectAllocator.
*/
/******************************************************************************/
struct OAConfig
{
  static const size_t BASIC_HEADER_SIZE = sizeof(unsigned) + 1; // allocation number + flags
  static const size_t EXTERNAL_HEADER_SIZE = sizeof(void*);     // just a pointer

  enum HBLOCK_TYPE{hbNone, hbBasic, hbExtended, hbExternal};
  struct HeaderBlockInfo
  {
    HBLOCK_TYPE type_;
    size_t size_;
    size_t additional_;
    HeaderBlockInfo(HBLOCK_TYPE type = hbNone, unsigned additional = 0) : type_(type), size_(0), additional_(additional)
    {
      if (type_ == hbBasic)
        size_ = BASIC_HEADER_SIZE;
      else if (type_ == hbExtended) // alloc # + use counter + flag byte + user-defined
        size_ = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(char) + additional_;
      else if (type_ == hbExternal)
        size_ = EXTERNAL_HEADER_SIZE;
    };
  };

  OAConfig(bool UseCPPMemManager = false,
           unsigned ObjectsPerPage = DEFAULT_OBJECTS_PER_PAGE, 
           unsigned MaxPages = DEFAULT_MAX_PAGES, 
           bool DebugOn = false, 
           unsigned PadBytes = 0,
           const HeaderBlockInfo &HBInfo = HeaderBlockInfo(),
           unsigned Alignment = 0) : UseCPPMemManager_(UseCPPMemManager),
                                     ObjectsPerPage_(ObjectsPerPage), 
                                     MaxPages_(MaxPages), 
                                     DebugOn_(DebugOn), 
                                     PadBytes_(PadBytes),
                                     HBlockInfo_(HBInfo),
                                     Alignment_(Alignment)
  {
    HBlockInfo_ = HBInfo;
    LeftAlignSize_ = 0;  
    InterAlignSize_ = 0;
  }

  bool UseCPPMemManager_;   // by-pass the functionality of the OA and use new/delete
  unsigned ObjectsPerPage_; // number of objects on each page
  unsigned MaxPages_;       // maximum number of pages the OA can allocate (0=unlimited)
  bool DebugOn_;            // enable/disable debugging code (signatures, checks, etc.)
  unsigned PadBytes_;          // size of the left/right padding for each block
  HeaderBlockInfo HBlockInfo_; // size of the header for each block (0=no headers)
  unsigned Alignment_;      // address alignment of each block

  unsigned LeftAlignSize_;  // number of alignment bytes required to align first block
  unsigned InterAlignSize_; // number of alignment bytes required between remaining blocks
};

/******************************************************************************/
/*!
  \struct OAStats
  \brief  
    Stats for ObjectAllocator.
*/
/******************************************************************************/
struct OAStats
{
  OAStats(void) : ObjectSize_(0), PageSize_(0), FreeObjects_(0), ObjectsInUse_(0), PagesInUse_(0),
                  MostObjects_(0), Allocations_(0), Deallocations_(0) {};

  size_t ObjectSize_;      // size of each object
  size_t PageSize_;        // size of a page including all headers, padding, etc.
  unsigned FreeObjects_;   // number of objects on the free list
  unsigned ObjectsInUse_;  // number of objects in use by client
  unsigned PagesInUse_;    // number of pages allocated
  unsigned MostObjects_;   // most objects in use by client at one time
  unsigned Allocations_;   // total requests to allocate memory
  unsigned Deallocations_; // total requests to free memory
};

/******************************************************************************/
/*!
  \struct GenericObject
  \brief  
    This allows us to easily treat raw objects as nodes in a linked list.
*/
/******************************************************************************/
struct GenericObject
{
  GenericObject *Next;
};

/******************************************************************************/
/*!
  \struct BasicHeader
  \brief  
    This allows us to easily treat raw objects as nodes in a linked list.
*/
/******************************************************************************/
struct BasicHeader
{
	unsigned allocationNum;
	char inUse;
};

/******************************************************************************/
/*!
  \struct MemBlockInfo
  \brief  
    This is the data structure for external header block.
*/
/******************************************************************************/
struct MemBlockInfo
{
  bool in_use;        // Is the block free or in use?
  unsigned alloc_num; // The allocation number (count) of this block
  char *label;        // A dynamically allocated NUL-terminated string
};

/******************************************************************************/
/*!
  \class ObjectAllocator
  \brief  
    The main allocator class.
*/
/******************************************************************************/
class ObjectAllocator
{
  public:
      // Defined by the client (pointer to a block, size of block)
      typedef void (*DUMPCALLBACK)(const void *, size_t);
      typedef void (*VALIDATECALLBACK)(const void *, size_t);

      // Predefined values for memory signatures
      static const unsigned char UNALLOCATED_PATTERN = 0xAA;
      static const unsigned char ALLOCATED_PATTERN = 0xBB;
      static const unsigned char FREED_PATTERN = 0xCC;
      static const unsigned char PAD_PATTERN = 0xDD;
      static const unsigned char ALIGN_PATTERN = 0xEE;

      // Creates the ObjectManager per the specified values
      // Throws an exception if the construction fails. (Memory allocation problem)
      ObjectAllocator(size_t ObjectSize, const OAConfig& config) throw(OAException);

      // Destroys the ObjectManager (never throws)
      ~ObjectAllocator() throw();

      // Take an object from the free list and give it to the client (simulates new)
      // Throws an exception if the object can't be allocated. (Memory allocation problem)
      void *Allocate(const char *label = 0) throw(OAException);

      // Returns an object to the free list for the client (simulates delete)
      // Throws an exception if the the object can't be freed. (Invalid object)
      void Free(void *Object) throw(OAException);

      // Calls the callback fn for each block still in use
      unsigned DumpMemoryInUse(DUMPCALLBACK fn) const;

      // Calls the callback fn for each block that is potentially corrupted
      unsigned ValidatePages(VALIDATECALLBACK fn) const;

      // Frees all empty pages (extra credit)
      unsigned FreeEmptyPages(void);

      // Returns true if FreeEmptyPages and alignments are implemented
      static bool ImplementedExtraCredit(void);

      // Testing/Debugging/Statistic methods
      void SetDebugState(bool State);       // true=enable, false=disable
      const void *GetFreeList(void) const;  // returns a pointer to the internal free list
      const void *GetPageList(void) const;  // returns a pointer to the internal page list
      OAConfig GetConfig(void) const;       // returns the configuration parameters
      OAStats GetStats(void) const;         // returns the statistics for the allocator

  private:

      // Make private to prevent copy construction and assignment
      ObjectAllocator(const ObjectAllocator &oa);
      ObjectAllocator &operator=(const ObjectAllocator &oa);
      
      // Other private fields and methods...
      // Other private fields and methods...
      void AllocatePage() throw(OAException);
      size_t ComputePageSize(OAConfig &config) const;
      bool IsPadSafe(unsigned char *ptr) const;
      void MemSet(void *ptr, int val, size_t sz);
      bool IsInFreeList(void *ptr) const;
      bool RemoveNode(GenericObject **head, GenericObject *ptr) const;
      // Other private fields and methods...

      const size_t ptrSize = sizeof(void *);

      int numPages;
      GenericObject *freeListPtr;
      GenericObject *pageListPtr;

      OAConfig conf;
      OAStats stats;

      const size_t objectSize;
      const size_t pageSize;
};

#endif
