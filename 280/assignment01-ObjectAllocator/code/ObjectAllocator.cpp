#include "ObjectAllocator.h"
#include <string.h>
#include <iostream>



ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config) throw(OAException) :
	oac(config), PageListHead(0), FreeListHead(0) {

	oas.ObjectSize_ = ObjectSize;
	oas.PagesInUse_ = 0;
	if (oac.Alignment_ != 0) {
		oac.InterAlignSize_ = countAlign(ObjectSize, true);
		oac.LeftAlignSize_ = countAlign(sizeof(GenericObject), false);
	}
	//std::cout << oac.PadBytes_ << " " << ObjectSize << " " << oac.HBlockInfo_.size_ << " " << oac.InterAlignSize_ << std::endl;

	perObj = ObjectSize + 2 * oac.PadBytes_ + oac.HBlockInfo_.size_ + oac.InterAlignSize_;

	oas.PageSize_ = ObjectSize + oac.PadBytes_ +
		(oac.ObjectsPerPage_ - 1) * perObj;
	headSize = sizeof(GenericObject) + oac.PadBytes_ + oac.HBlockInfo_.size_ + oac.LeftAlignSize_;
	//  std::cout << perObj << " " << headSize << std::endl;
	oas.PageSize_ += headSize;
	MakePage();

}

ObjectAllocator::~ObjectAllocator() throw() {
	while (PageListHead) {
		if (oac.HBlockInfo_.type_ == OAConfig::hbExternal) {
			while (FreeListHead) {
				MemBlockInfo ** tmpBlock = reinterpret_cast<MemBlockInfo**>(
					reinterpret_cast<char*>(FreeListHead) - oac.PadBytes_ - oac.HBlockInfo_.size_
					);
				if (*tmpBlock) {
					delete[](*tmpBlock)->label;
					delete *tmpBlock;
				}

				FreeListHead = FreeListHead->Next;
			}
		}
		char * tmp = reinterpret_cast<char*>(PageListHead);

		PageListHead = PageListHead->Next;
		delete[] tmp;

	}
}


void * ObjectAllocator::Allocate(const char *label) throw(OAException) {

	//GenericObject * tmp = FreeListHead;
	GenericObject * tmp = 0;

	if (oac.UseCPPMemManager_) {
		//char * charTmp = reinterpret_cast<char*>(tmp);
		++oas.ObjectsInUse_;
		++oas.Allocations_;
		--oas.FreeObjects_;

		if (oas.ObjectsInUse_ > oas.MostObjects_)
			oas.MostObjects_ = oas.ObjectsInUse_;

		return new char[oas.ObjectSize_];
	}

	if (!FreeListHead) MakePage();

	tmp = FreeListHead;
	FreeListHead = FreeListHead->Next;

	++oas.ObjectsInUse_;
	++oas.Allocations_;
	--oas.FreeObjects_;

	if (oas.ObjectsInUse_ > oas.MostObjects_)
		oas.MostObjects_ = oas.ObjectsInUse_;

	memset(reinterpret_cast<char*>(tmp), ALLOCATED_PATTERN, oas.ObjectSize_);

	if (oac.HBlockInfo_.type_ != OAConfig::hbNone && oac.HBlockInfo_.type_ != OAConfig::hbExternal) {

		if (oac.HBlockInfo_.type_ == OAConfig::hbExtended) {
			memset(reinterpret_cast<char*>(tmp) - oac.PadBytes_ - oac.HBlockInfo_.size_, 0, oac.HBlockInfo_.additional_);
			unsigned short * useCount = reinterpret_cast<unsigned short *>(reinterpret_cast<char*>(tmp) - oac.PadBytes_ - sizeof(bool) -
				sizeof(unsigned) - sizeof(unsigned short));
			++*useCount;
		}

		unsigned * allocNum = reinterpret_cast<unsigned*>(reinterpret_cast<char*>(tmp) - oac.PadBytes_ - sizeof(bool) - sizeof(unsigned));
		bool * inUse = reinterpret_cast<bool*>(reinterpret_cast<char*>(tmp) - oac.PadBytes_ - sizeof(bool));
		*allocNum = oas.Allocations_;
		*inUse = 1;

	}

	if (oac.HBlockInfo_.type_ == OAConfig::hbExternal){
		MemBlockInfo ** tmpBlock = reinterpret_cast<MemBlockInfo**>(reinterpret_cast<char*>(tmp) - oac.PadBytes_ - oac.HBlockInfo_.size_);
		*tmpBlock = new MemBlockInfo();
		(*tmpBlock)->in_use = 1;
		if (label) {
			(*tmpBlock)->label = new char[strlen(label) + 1];
			strcpy((*tmpBlock)->label, label);
		}
		else
			(*tmpBlock)->label = 0;

		(*tmpBlock)->alloc_num = oas.Allocations_;
	}
  
  return tmp;
}

unsigned ObjectAllocator::countAlign(size_t size_, bool interAlignment){

  size_t tmpSize = size_ + ((interAlignment)?2:1) * oac.PadBytes_ + oac.HBlockInfo_.size_;
  size_t tmpAlignSize = tmpSize;
  while(tmpAlignSize % oac.Alignment_) ++tmpAlignSize;
  return static_cast<unsigned>(tmpAlignSize - tmpSize);

}

void ObjectAllocator::MakePage() throw(OAException) {
  char * tmphead = 0;

  if (oas.PagesInUse_ >= oac.MaxPages_)
	  throw OAException(OAException::E_NO_PAGES," no more pages");
  
  try{
    tmphead = new char[oas.PageSize_];
  }
  catch(std::bad_alloc){
    throw OAException(OAException::E_NO_MEMORY," Insufficient Memory");
  }
  

  if (tmphead){
    
    ++oas.PagesInUse_;
    memset (tmphead,UNALLOCATED_PATTERN,oas.PageSize_);
	
	reinterpret_cast<GenericObject*>(tmphead)->Next = PageListHead;
	PageListHead = reinterpret_cast<GenericObject*>(tmphead);
    
    /*if (oac.DebugOn_)
      SetHeadMem();*/
  
	//tmphead += headSize;
	// HEAD MEM
	tmphead += sizeof(void*);
	
	memset(tmphead, ALIGN_PATTERN , oac.LeftAlignSize_);
	tmphead += (oac.LeftAlignSize_ + oac.HBlockInfo_.size_ + oac.PadBytes_);
	//

	GenericObject * prev = 0;
    for (size_t i = 0; i < oac.ObjectsPerPage_; ++i){
		reinterpret_cast<GenericObject*>(tmphead)->Next = prev;
		
		prev = reinterpret_cast<GenericObject*>(tmphead);
		SetBlockMem(reinterpret_cast<unsigned char *>(prev),i);
		SetHeaderInfo(reinterpret_cast<char *>(prev) - oac.PadBytes_  - oac.HBlockInfo_.size_);

		tmphead += (oas.ObjectSize_ + 2* oac.PadBytes_ + oac.InterAlignSize_ + oac.HBlockInfo_.size_);
    
	}
	FreeListHead = prev;
    oas.FreeObjects_ += oac.ObjectsPerPage_;
  }
}


void ObjectAllocator::SetBlockMem(unsigned char * tmp,size_t i){
  
  memset(tmp - oac.PadBytes_, PAD_PATTERN, oac.PadBytes_);
  memset(tmp + oas.ObjectSize_, PAD_PATTERN, oac.PadBytes_);
 // unsigned char * tmpp = tmp + oas.ObjectSize_ + oac.PadBytes_;
  if (i != oac.ObjectsPerPage_ -1)
	memset(tmp + oas.ObjectSize_ + oac.PadBytes_, ALIGN_PATTERN, oac.InterAlignSize_);
  /*
  memset(tmp + tmpObjsize, PAD_PATTERN , oac.PadBytes_);
  memset(tmp + tmpObjsize + oac.PadBytes_, ALIGN_PATTERN, oac.InterAlignSize_);
  memset(tmp + tmpObjsize + oac.PadBytes_ + oac.InterAlignSize_ + oac.HBlockInfo_.size_,
        PAD_PATTERN , oac.PadBytes_);
		*/
}

void ObjectAllocator::SetHeaderInfo(char * tmp){

	if (oac.HBlockInfo_.type_ != OAConfig::hbNone && oac.HBlockInfo_.type_ != OAConfig::hbExternal) {
		size_t offset = 0;
		if (oac.HBlockInfo_.type_ == OAConfig::hbExtended) {
			offset += (oac.HBlockInfo_.additional_ + sizeof(unsigned short));
			memset(tmp, 0, oac.HBlockInfo_.additional_);
			unsigned short * useCount = reinterpret_cast<unsigned short*>(tmp + oac.HBlockInfo_.additional_);
			*useCount = 0;;
		}

		unsigned * allocNum = reinterpret_cast<unsigned*>(tmp + offset);
		bool * inUse = reinterpret_cast<bool*>(tmp + offset + sizeof(unsigned));
		*allocNum = 0;
		*inUse = 0;
	}
	else 
		if (oac.HBlockInfo_.type_ == OAConfig::hbExternal)
			memset(tmp, 0, oac.HBlockInfo_.size_);
		
}

void ObjectAllocator::Free(void *Object) throw(OAException){

  unsigned char * tmp = reinterpret_cast<unsigned char*>(Object);

  if (!oac.UseCPPMemManager_){

	  if(static_cast<unsigned char>(*(tmp + sizeof(void*))) == FREED_PATTERN)
		  throw OAException(OAException::E_MULTIPLE_FREE, "freed already");
	  
	  GenericObject* tmpPage = PageListHead;
	  while (tmpPage){
		  if (tmp > reinterpret_cast<unsigned char*>(tmpPage) &&
		  tmp < reinterpret_cast<unsigned char*>(tmpPage) + oas.PageSize_){
			  unsigned char * blockStart = reinterpret_cast<unsigned char*>(tmpPage) + headSize;
			//  std::cout << (tmp - blockStart) % perObj << std::endl;
			  if ((tmp - blockStart) % perObj)
				 throw OAException(OAException::E_BAD_BOUNDARY, "bad Boundary"); 
		  }
		  
		  if (tmpPage->Next == 0 && reinterpret_cast<unsigned char*>(tmpPage) + oas.PageSize_ == tmp)
			  throw OAException(OAException::E_BAD_BOUNDARY, "bad Boundary"); 
		  tmpPage = tmpPage->Next;
	  }

	  for (unsigned i = 0; i < oac.PadBytes_; ++i) {

		  if (*(tmp - oac.PadBytes_ + i) != PAD_PATTERN ||
			  *(tmp + oas.ObjectSize_ + i) != PAD_PATTERN) 
			  throw OAException(OAException::E_CORRUPTED_BLOCK, "corrupted");
		  
	  }
		
	   memset(tmp , FREED_PATTERN, oas.ObjectSize_);
	   
	   if (oac.HBlockInfo_.type_ != OAConfig::hbNone && oac.HBlockInfo_.type_ != OAConfig::hbExternal) {

		   if (oac.HBlockInfo_.type_ == OAConfig::hbExtended) 
			   memset(reinterpret_cast<char*>(tmp) - oac.PadBytes_ - oac.HBlockInfo_.size_, 0, oac.HBlockInfo_.additional_);
		   

		   unsigned * allocNum = reinterpret_cast<unsigned*>(tmp - oac.PadBytes_ - sizeof(bool) - sizeof(unsigned));
		   bool * inUse = reinterpret_cast<bool*>(tmp - oac.PadBytes_ - sizeof(bool));
		   *allocNum = 0;
		   *inUse = 0;
	   }

	   if (oac.HBlockInfo_.type_ == OAConfig::hbExternal) {
		//   std::cout << oac.HBlockInfo_.size_ << " " << oac.EXTERNAL_HEADER_SIZE << std::endl;
		   MemBlockInfo ** tmpBlock = reinterpret_cast<MemBlockInfo **> (reinterpret_cast<char*>(tmp) - oac.PadBytes_ - oac.HBlockInfo_.size_);

		   delete[] (*tmpBlock)->label;
		   delete *tmpBlock;

		   memset(reinterpret_cast<char*>(tmp) - oac.PadBytes_ - oac.HBlockInfo_.size_, 0, oac.HBlockInfo_.size_);
	   }

	   reinterpret_cast<GenericObject*>(tmp)->Next = FreeListHead;
	   FreeListHead = reinterpret_cast<GenericObject*>(tmp);
	  


  }else{
    delete [] tmp;

  }
    --oas.ObjectsInUse_;
    ++oas.Deallocations_;
    ++oas.FreeObjects_;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const{

	GenericObject * tmpPage = PageListHead;
	unsigned corrupted = 0;
	unsigned char * PadLeft = 0, * PadRight = 0;

	if (!oac.PadBytes_)
		return 0;

	while (tmpPage) {

		PadLeft = reinterpret_cast<unsigned char*>(tmpPage) + headSize - oac.PadBytes_;
		PadRight = reinterpret_cast<unsigned char*>(tmpPage) + headSize + oas.ObjectSize_;

		for (unsigned i = 0; i < oac.ObjectsPerPage_; ++i) {
			for (unsigned j = 0; j < oac.PadBytes_; ++j) {
				if (*(PadLeft+j) != PAD_PATTERN || *(PadRight+j)!= PAD_PATTERN) {
					++corrupted;
					fn(reinterpret_cast<unsigned char*>(PadLeft + oac.PadBytes_), oas.ObjectSize_);
					break; // pages not blocks
				}
			}
			if (i < oac.ObjectsPerPage_ - 1) {
				PadLeft += perObj;
				PadRight += perObj;
			}
		}

		tmpPage = tmpPage->Next;
	}

	return corrupted;
}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const{

	GenericObject * tmpPage = PageListHead;
	
	while (tmpPage)
	{
		unsigned char * datastart = reinterpret_cast<unsigned char*>(tmpPage) + headSize;
		for (unsigned i = 0; i < oac.ObjectsPerPage_; ++i) {
			if (*datastart == ALLOCATED_PATTERN) {
				fn(datastart, oas.ObjectSize_);
			}

			if (i < oac.ObjectsPerPage_-1)
				datastart += perObj;

		}
		tmpPage = tmpPage->Next;
	}

	return oas.ObjectsInUse_;
}

unsigned ObjectAllocator::FreeEmptyPages(void){return 0;}
bool ObjectAllocator::ImplementedExtraCredit(void){return false;}

void ObjectAllocator::SetDebugState(bool State){ oac.DebugOn_ = State;}
const void * ObjectAllocator::GetFreeList(void) const{ return FreeListHead; }
const void * ObjectAllocator::GetPageList(void) const{ return PageListHead; }
OAConfig ObjectAllocator::GetConfig(void) const{ return oac;}
OAStats ObjectAllocator::GetStats(void) const{ return oas;}
