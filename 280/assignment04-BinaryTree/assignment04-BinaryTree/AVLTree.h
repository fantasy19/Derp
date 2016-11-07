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
	  void RotateLeft(typename BSTree<T>::BinTree &node);
	  void RotateRight(typename BSTree<T>::BinTree &node);
	  void insert_begin(typename BSTree<T>::BinTree &tree, const T& value);
	  void insert_node(typename BSTree<T>::BinTree & node, T value, std::stack<typename BSTree<T>::BinTree*> & nodes);
	  void BalanceAVLTree(std::stack<typename BSTree<T>::BinTree*> & nodes);
	  unsigned int node_count(typename BSTree<T>::BinTree& tree) const;
	  void remove_begin(typename BSTree<T>::BinTree &tree, const T& value);
	  void remove_node(typename BSTree<T>::BinTree &tree, const T& value,
		  std::stack<typename BSTree<T>::BinTree*>& nodes);
	  ObjectAllocator * oa;
	  bool share;
};

#include "AVLTree.cpp"

#endif
//---------------------------------------------------------------------------
