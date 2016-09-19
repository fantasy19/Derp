#include "ObjectAllocator.h"
#include <string.h>
#include <iostream>



ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config) throw(OAException) :
oac(config), PageListHead(0), FreeListHead(0) {

  oas.ObjectSize_ = ObjectSize;
  oas.PagesInUse_ = 0;
  if (oac.Alignment_ != 0){
    oac.InterAlignSize_ = countAlign(ObjectSize,true);
    oac.LeftAlignSize_ = countAlign(sizeof(GenericObject), false);
  }
//std::cout << oac.PadBytes_ << " " << ObjectSize << " " << oac.HBlockInfo_.size_ << " " << oac.InterAlignSize_ << std::endl;

  perObj = ObjectSize + 2 * oac.PadBytes_ + oac.HBlockInfo_.size_ + oac.InterAlignSize_;

  oas.PageSize_ = ObjectSize + oac.PadBytes_+
                  (oac.ObjectsPerPage_ - 1) * perObj;
  headSize = sizeof(GenericObject) + oac.PadBytes_ + oac.HBlockInfo_.size_ + oac.LeftAlignSize_;
//  std::cout << perObj << " " << headSize << std::endl;
  oas.PageSize_ += headSize;
  MakePage();

}

ObjectAllocator::~ObjectAllocator() throw(){
  while(PageListHead){
    if (oac.HBlockInfo_.type_ ==  OAConfig::hbExternal){
      while(FreeListHead){
        delete reinterpret_cast<MemBlockInfo*>(
                reinterpret_cast<char*>(FreeListHead) + sizeof(GenericObject) +
                                        oas.ObjectSize_ +
                                        oac.InterAlignSize_+ oac.PadBytes_
               );
        FreeListHead = FreeListHead->Next;
      }
    }
    char * tmp = reinterpret_cast<char*>(PageListHead);
    PageListHead = PageListHead->Next;
    delete[] tmp;
  }
}


void * ObjectAllocator::Allocate(const char *label) throw(OAException){

  //GenericObject * tmp = FreeListHead;
  GenericObject * tmp = nullptr;

  if (oac.UseCPPMemManager_){
    //char * charTmp = reinterpret_cast<char*>(tmp);
    return new char[oas.ObjectSize_];
  }

  if(!FreeListHead) MakePage();
  
  tmp = FreeListHead;
  FreeListHead = FreeListHead->Next;
  
  ++oas.ObjectsInUse_;
  ++oas.Allocations_;
  --oas.FreeObjects_;

  if (oas.ObjectsInUse_ > oas.MostObjects_)
		  oas.MostObjects_ = oas.ObjectsInUse_;
	  
  memset(reinterpret_cast<char*>(tmp), ALLOCATED_PATTERN, perObj);
  
  return tmp;
/*  if (!tmp && !oac.UseCPPMemManager_){
	  
    if (oas.PagesInUse_ != oac.MaxPages_){ // limit of pages not hit yet
      MakePage();
      tmp = FreeListHead; // FreeListHead not nullptr by now

      FreeListHead = FreeListHead->Next;

      ++oas.ObjectsInUse_;
      ++oas.Allocations_;
      --oas.FreeObjects_;

      if (oas.ObjectsInUse_ > oas.MostObjects_)
		      oas.MostObjects_ = oas.ObjectsInUse_;

    }else
      throw OAException(OAException::E_NO_PAGES," no more pages");

  }else
    if (tmp && !oac.UseCPPMemManager_){
      FreeListHead = FreeListHead->Next;
      ++oas.ObjectsInUse_;
      ++oas.Allocations_;
      --oas.FreeObjects_;
    }

  memset(reinterpret_cast<char*>(tmp), ALLOCATED_PATTERN, perObj);
  SetHeaderInfo(reinterpret_cast<char*>(tmp) + oas.ObjectSize_ + oac.PadBytes_ + oac.InterAlignSize_,
                label);
  return reinterpret_cast<void*>(tmp);*/
}

unsigned ObjectAllocator::countAlign(size_t size_, bool interAlignment){

  size_t tmpSize = size_ + ((interAlignment)?2:1) * oac.PadBytes_ + oac.HBlockInfo_.size_;
  size_t tmpAlignSize = tmpSize;
  while(tmpAlignSize % oac.Alignment_) ++tmpAlignSize;
  return (tmpAlignSize - tmpSize);

}

void ObjectAllocator::MakePage() throw(OAException) {
  char * tmphead = nullptr;
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
    /*
    if (oas.PagesInUse_ == 1){
      PageListHead = reinterpret_cast<GenericObject*>(tmphead);
      PageListHead->Next = 0;
    }else{
      GenericObject * tmp = PageListHead;
      GenericObject * tmpNext = PageListHead->Next;

      PageListHead = reinterpret_cast<GenericObject*>(tmphead);
      PageListHead->Next = tmp;
      PageListHead->Next->Next = tmpNext;
    }
	*/
	
	reinterpret_cast<GenericObject*>(tmphead)->Next = PageListHead;
	PageListHead = reinterpret_cast<GenericObject*>(tmphead);
    
    /*if (oac.DebugOn_)
      SetHeadMem();*/
  
	//tmphead += headSize;
	tmphead += sizeof(void*);
	
	memset(tmphead, ALIGN_PATTERN , oac.LeftAlignSize_);
	tmphead += oac.LeftAlignSize_;
	
	GenericObject * prev = 0;
    for (size_t i = 0; i < oac.ObjectsPerPage_; ++i){
		reinterpret_cast<GenericObject*>(tmphead)->Next = prev;
		/*FreeListHead = reinterpret_cast<GenericObject*>(tmphead);
		FreeListHead->Next = prev;*/
		prev = reinterpret_cast<GenericObject*>(tmphead);
		tmphead += perObj;//oas.ObjectSize_;
		
		
      /*
	  GenericObject * tmpFree = FreeListHead;
      GenericObject * tmpNextFree = FreeListHead->Next;

      FreeListHead = reinterpret_cast<GenericObject*>(
                                reinterpret_cast<char*>(FreeListHead) + perObj
                              );
      FreeListHead->Next = tmpFree;
      FreeListHead->Next->Next = tmpNextFree;

      if (i != oac.ObjectsPerPage_ - 2){
        if (oac.DebugOn_)
          SetBlockMem(reinterpret_cast<char*>(FreeListHead) + sizeof(GenericObject));
      }else
        if (oac.DebugOn_)
          memset(reinterpret_cast<char*>(FreeListHead) + oas.ObjectSize_,
                PAD_PATTERN,
                oac.PadBytes_);*/
    
	}
	FreeListHead = prev;
    oas.FreeObjects_ += oac.ObjectsPerPage_;
}
}

void ObjectAllocator::SetHeadMem(){
  //char * tmp = reinterpret_cast<char*>(PageListHead) + sizeof(GenericObject);
  char * tmp = reinterpret_cast<char*>(PageListHead) + sizeof(void*);
  memset( tmp, ALIGN_PATTERN , oac.LeftAlignSize_);
  // set header here
  //memset( tmp + oac.LeftAlignSize_ + oac.HBlockInfo_.size_, PAD_PATTERN , oac.PadBytes_);
}

void ObjectAllocator::SetBlockMem(char * tmp){
  size_t tmpObjsize = oas.ObjectSize_ - sizeof(GenericObject);
  memset(tmp, UNALLOCATED_PATTERN, tmpObjsize);
  memset(tmp + tmpObjsize, PAD_PATTERN , oac.PadBytes_);
  memset(tmp + tmpObjsize + oac.PadBytes_, ALIGN_PATTERN, oac.InterAlignSize_);
  memset(tmp + tmpObjsize + oac.PadBytes_ + oac.InterAlignSize_ + oac.HBlockInfo_.size_,
        PAD_PATTERN , oac.PadBytes_);

}

void ObjectAllocator::SetHeaderInfo(char * tmp, const char * label){

  if (oac.HBlockInfo_.type_ != OAConfig::hbBasic){
    unsigned * allocNum = reinterpret_cast<unsigned*>(tmp);
    bool * inUse = reinterpret_cast<bool*>(tmp + sizeof(unsigned));
    *allocNum = oas.Allocations_;
    *inUse = 1;
  }

  if (oac.HBlockInfo_.type_ == OAConfig::hbExtended){
    memset(tmp,0,oac.HBlockInfo_.additional_);
    unsigned short * useCount = reinterpret_cast<unsigned short*>(tmp + oac.HBlockInfo_.additional_);
    unsigned * allocNum = reinterpret_cast<unsigned*>(tmp + oac.HBlockInfo_.additional_ +
                                                      sizeof(unsigned short));
    bool * inUse = reinterpret_cast<bool*>(tmp + oac.HBlockInfo_.additional_ +
                                          sizeof(unsigned short) + sizeof(unsigned));
    *allocNum = oas.Allocations_;
    *inUse = 1;
    ++*useCount;
  }

  if (oac.HBlockInfo_.type_ == OAConfig::hbExternal){
    MemBlockInfo * tmpblock = reinterpret_cast<MemBlockInfo*>(tmp);
    try{
      tmpblock = new MemBlockInfo();
      tmpblock->in_use = true;
      tmpblock->label = const_cast<char*>(label);
      tmpblock->alloc_num = oas.Allocations_;
    }catch (std::bad_alloc &){

    }

  }

}

void ObjectAllocator::Free(void *Object) throw(OAException){

  char * tmp = reinterpret_cast<char*>(Object);
  bool freed = true;
  bool corrupted = false;

  if (!oac.UseCPPMemManager_){

      for (int i = 0; i < oac.PadBytes_; ++i){ 
        if ( *(tmp + oas.ObjectSize_ + i) != PAD_PATTERN ||
              *(tmp + oas.ObjectSize_ + oac.PadBytes_ +
              oac.Alignment_ + oac.HBlockInfo_.size_ + i) != PAD_PATTERN)
          corrupted = true;
      }

//std::cout << "0" << std::endl;
      if (corrupted)
        throw OAException(OAException::E_CORRUPTED_BLOCK, "corrupted");
	  
	  if(static_cast<unsigned char>(*(tmp + sizeof(void*))) == FREED_PATTERN)
		  throw OAException(OAException::E_MULTIPLE_FREE, "freed already");
	  
	  GenericObject* tmpPage = PageListHead;
	  while (tmpPage){
		  if (tmp > reinterpret_cast<char*>(tmpPage) &&
		  tmp < reinterpret_cast<char*>(tmpPage) + oas.PageSize_){
			  char * blockStart = reinterpret_cast<char*>(tmpPage) + sizeof(void*) ;

			  if ((tmp - blockStart) % oas.ObjectSize_)
				 throw OAException(OAException::E_BAD_BOUNDARY, "bad Boundary"); 
		  }
		  
		  if (tmpPage->Next == 0 && reinterpret_cast<char*>(tmpPage) + oas.PageSize_ == tmp)
			  throw OAException(OAException::E_BAD_BOUNDARY, "bad Boundary"); 
		  tmpPage = tmpPage->Next;
	  }

		  
	  /*
	  for (int i = 0; i < oas.ObjectSize_; ++i){
        if ( *(tmp + i) != FREED_PATTERN)
          freed = false;
	  
      }
	  
	  if (freed)
		  throw OAException(OAException::E_MULTIPLE_FREE, "freed already");
	  */
	   memset(tmp , FREED_PATTERN, oas.ObjectSize_);
	   
	   reinterpret_cast<GenericObject*>(tmp)->Next = FreeListHead;
	   FreeListHead = reinterpret_cast<GenericObject*>(tmp);
	  
	  /*
      for (int i = 0; i < oas.ObjectSize_- sizeof(GenericObject); ++i){
        if ( *(tmp + sizeof(GenericObject) + i) != FREED_PATTERN)
          freed = false;
      }

      if (freed){
        
        char * headerFront = tmp + oas.ObjectSize_ + oac.PadBytes_ + oac.Alignment_;
        
        if (oac.HBlockInfo_.type_ != OAConfig::hbBasic){
          bool * inUse = reinterpret_cast<bool*>(headerFront + sizeof(unsigned));
          *inUse = false;
        }

        if (oac.HBlockInfo_.type_ == OAConfig::hbExtended){
          bool * inUse = reinterpret_cast<bool*>(headerFront + sizeof(unsigned short) + sizeof(unsigned));
          *inUse = false;
        }

        if (oac.HBlockInfo_.type_ == OAConfig::hbExternal){
          MemBlockInfo * tmpBlock = reinterpret_cast<MemBlockInfo*>(headerFront);
          tmpBlock->in_use = false;
        }
         
      }else
        throw OAException(OAException::E_MULTIPLE_FREE, "freed already");
      
      
      memset(tmp + sizeof(GenericObject), FREED_PATTERN, oas.ObjectSize_- sizeof(GenericObject));
      --oas.ObjectsInUse_;
      ++oas.Deallocations_;
      ++oas.FreeObjects_;
*/

  }else{
    delete [] tmp;

  }
    --oas.ObjectsInUse_;
    ++oas.Deallocations_;
    ++oas.FreeObjects_;
}

unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const{
  return 0;
}

unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const{return 0;}

unsigned ObjectAllocator::FreeEmptyPages(void){return 0;}
bool ObjectAllocator::ImplementedExtraCredit(void){return false;}

void ObjectAllocator::SetDebugState(bool State){ oac.DebugOn_ = State;}
const void * ObjectAllocator::GetFreeList(void) const{ return FreeListHead; }
const void * ObjectAllocator::GetPageList(void) const{ return PageListHead; }
OAConfig ObjectAllocator::GetConfig(void) const{ return oac;}
OAStats ObjectAllocator::GetStats(void) const{ return oas;}
