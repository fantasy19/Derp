//---------------------------------------------------------------------------
#ifndef AVLTREE_H
#define AVLTREE_H
//---------------------------------------------------------------------------
#include <stack>
#include "BSTree.h"

template <typename T>
class AVLTree : public BSTree<T>
{
  public:
    AVLTree(ObjectAllocator *OA = 0, bool ShareOA = false);
    virtual ~AVLTree();
    virtual void insert(const T& value) throw(BSTException);
    virtual void remove(const T& value);

      // Returns true if efficiency implemented
    static bool ImplementedBalanceFactor(void);
    
  private:
    // private stuff
};

#include "AVLTree.h"

#endif
//---------------------------------------------------------------------------
