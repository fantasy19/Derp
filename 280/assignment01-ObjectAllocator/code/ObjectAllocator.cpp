#include "ObjectAllocator.h"
#include <string.h>
#include <iostream>



ObjectAllocator::ObjectAllocator(size_t ObjectSize, const OAConfig& config) throw(OAException) :
oac(config) {
  std::cout <<ObjectSize << std::endl;
  oas.ObjectSize_ = ObjectSize;
  oas.PagesInUse_ = 0;
  if (oac.Alignment_ != 0){
    oac.InterAlignSize_ = countInterAlign(ObjectSize,true);
    oac.LeftAlignSize_ = countInterAlign(sizeof(GenericObject), false);
  }

  oas.PageSize_ =  (oac.ObjectsPerPage_ - 1) * oac.InterAlignSize_ +
              (oac.ObjectsPerPage_ - 1) * oac.HBlockInfo_.size_ +
              (oac.ObjectsPerPage_ - 1) * 2 * oac.PadBytes_ +
              oac.ObjectsPerPage_ * ObjectSize +
              oac.PadBytes_;

  perObj = ObjectSize + 2 * oac.PadBytes_ + oac.HBlockInfo_.size_ + oac.InterAlignSize_;
  headSize = sizeof(GenericObject) + oac.PadBytes_ + oac.HBlockInfo_.size_ + oac.LeftAlignSize_;

  oas.PageSize_ += headSize;
  MakePage();

}

ObjectAllocator::~ObjectAllocator() throw(){
  while(PageListHead){
    char * tmp = reinterpret_cast<char*>(PageListHead);
    PageListHead = PageListHead->Next;
    delete[] tmp;
  }
}


void * ObjectAllocator::Allocate(const char *label) throw(OAException){

  GenericObject * tmp = FreeListHead;

  if (oac.UseCPPMemManager_){
    char * charTmp = reinterpret_cast<char*>(tmp);
    charTmp = new char[oas.ObjectSize_];
  }

  if (!tmp && !oac.UseCPPMemManager_){
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

  memset(tmp, ALLOCATED_PATTERN, oas.ObjectSize_);
  SetHeaderInfo(reinterpret_cast<char*>(tmp) + oas.ObjectSize_ + oac.PadBytes_ + oac.InterAlignSize_,
                label);
  return reinterpret_cast<void*>(tmp);
}

unsigned ObjectAllocator::countInterAlign(size_t size_, bool interAlignment){
  return size_ + ((interAlignment)?2:1) * oac.PadBytes_ + oac.HBlockInfo_.size_ +
          (static_cast<unsigned>(size_) % oac.Alignment_);
}

void ObjectAllocator::MakePage(){
  char * tmphead;
  try{
    tmphead = new char[oas.PageSize_];
  }
  catch(std::bad_alloc){
    throw OAException(OAException::E_NO_MEMORY," Insufficient Memory");
  }

  if (tmphead){
    ++oas.PagesInUse_;
    memset (tmphead,0,oas.PageSize_);
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

    SetHeadMem();

    FreeListHead = reinterpret_cast<GenericObject*>(tmphead + headSize);
    FreeListHead->Next = 0;

    for (size_t i = 0; i < oac.ObjectsPerPage_ - 1 ; ++i){
      GenericObject * tmpFree = FreeListHead;
      GenericObject * tmpNextFree = FreeListHead->Next;

      FreeListHead = reinterpret_cast<GenericObject*>(
                                reinterpret_cast<char*>(FreeListHead) + perObj
                              );
      FreeListHead->Next = tmpFree;
      FreeListHead->Next->Next = tmpNextFree;

      if (i != oac.ObjectsPerPage_ - 2)
        SetBlockMem(reinterpret_cast<char*>(FreeListHead) + sizeof(GenericObject));
      else
        memset(reinterpret_cast<char*>(FreeListHead) + perObj + sizeof(GenericObject),
              PAD_PATTERN,
              oac.PadBytes_);
    }

    oas.FreeObjects_ = oac.ObjectsPerPage_;
}
}

void ObjectAllocator::SetHeadMem(){
  char * tmp = reinterpret_cast<char*>(PageListHead) + sizeof(GenericObject);
  memset( tmp, ALIGN_PATTERN , oac.LeftAlignSize_);
  memset( tmp + oac.LeftAlignSize_ + oac.HBlockInfo_.size_, PAD_PATTERN , oac.PadBytes_);
}

void ObjectAllocator::SetBlockMem(char * tmp){
  memset(tmp, UNALLOCATED_PATTERN, oas.ObjectSize_);
  memset(tmp + oas.ObjectSize_, PAD_PATTERN , oac.PadBytes_);
  memset(tmp + oas.ObjectSize_ + oac.PadBytes_, ALIGN_PATTERN, oac.InterAlignSize_);
  memset(tmp + oas.ObjectSize_ + oac.PadBytes_ + oac.InterAlignSize_ + oac.HBlockInfo_.size_,
        PAD_PATTERN , oac.PadBytes_);

}

void ObjectAllocator::SetHeaderInfo(char * tmp, const char * label){


  if (oac.HBlockInfo_.type_ != OAConfig::hbNone){
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

void ObjectAllocator::Free(void *Object) throw(OAException){}
unsigned ObjectAllocator::DumpMemoryInUse(DUMPCALLBACK fn) const{return 0;}
unsigned ObjectAllocator::ValidatePages(VALIDATECALLBACK fn) const{return 0;}
unsigned ObjectAllocator::FreeEmptyPages(void){return 0;}
bool ObjectAllocator::ImplementedExtraCredit(void){return false;}

void ObjectAllocator::SetDebugState(bool State){ oac.DebugOn_ = State;}
const void * ObjectAllocator::GetFreeList(void) const{ return FreeListHead; }
const void * ObjectAllocator::GetPageList(void) const{ return PageListHead; }
OAConfig ObjectAllocator::GetConfig(void) const{ return oac;}
OAStats ObjectAllocator::GetStats(void) const{ return oas;}
