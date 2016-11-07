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
	  void RotateLeft(BinTree node);
	  void RotateRight(BinTree node);
	  void insert_node(BinTree node, T value, std::stack<BinTree> & nodes);
	  void BalanceAVLTree(std::stack<BinTree> & nodes);
	  int height(BinTree node);
	  ObjectAllocator * oa;
	  bool share;
};

#include "AVLTree.cpp"

#endif
//---------------------------------------------------------------------------
