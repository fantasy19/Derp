////////////////////////////////////////////////////////////////////////////////
#ifndef BLIST_H
#define BLIST_H
////////////////////////////////////////////////////////////////////////////////
#include <string>     // error strings
#include <stdexcept>  // std::exception
#include <iostream>
#ifdef _MSC_VER
#pragma warning( disable : 4290 ) // suppress warning: C++ Exception Specification ignored
#endif

class BListException : public std::exception
{
  private:
    int m_ErrCode;
    std::string m_Description;

  public:
    BListException(int ErrCode, const std::string& Description) :
    m_ErrCode(ErrCode), m_Description(Description) {};

    virtual int code(void) const {
      return m_ErrCode;
    }

    virtual const char *what(void) const throw() {
      return m_Description.c_str();
    }

    virtual ~BListException() throw() {
    }

    enum BLIST_EXCEPTION {E_NO_MEMORY, E_BAD_INDEX, E_DATA_ERROR};
};

struct BListStats
{

  BListStats() : NodeSize(0), NodeCount(0), ArraySize(0), ItemCount(0)  {};
  BListStats(size_t nsize, size_t ncount, size_t asize, unsigned count) :
     NodeSize(nsize), NodeCount(ncount), ArraySize(asize), ItemCount(count)  {};

  size_t NodeSize;    // Size of a node
  size_t NodeCount;   // Number of nodes in the list
  size_t ArraySize;   // Max number of items in each node
  unsigned ItemCount; // Number of items in the entire list
};

template <typename T, unsigned Size = 1>
class BList
{
public:
	struct BNode
	{
		BNode *next;
		BNode *prev;
		unsigned count; // number of items currently in the node
        T values[Size];
        BNode() : next(0), prev(0), count(0) {
            for (unsigned i = 0; i < Size; ++i)
                values[i] = static_cast<T>(0);
        }
	};

	BList();                 // default constructor
  BList(const BList &rhs); // copy constructor

  ~BList();            // destructor
        

  BList& operator=(const BList &rhs)  throw(BListException);

	// arrays will be unsorted
  void push_back(const T& value) throw(BListException);

  void push_front(const T& value) throw(BListException);


	// arrays will be sorted
  void insert(const T& value) throw(BListException);

  void remove(int index) throw(BListException);
  void remove_by_value(const T& value);

  int find(const T& value) const;     // returns index, -1 if not found
       

  T& operator[](int index) throw(BListException);// for l-values
	const T& operator[](int index) const throw(BListException) ; // for r-values



  unsigned size(void) const;// total number of items (not nodes)

	void clear(void);          // delete all nodes

    static unsigned nodesize(void); // so the allocator knows the size

      // For debugging
    const BNode *GetHead() const;
    BListStats GetStats() const ;

  private:
    BNode *head_; // points to the first node
    BNode *tail_; // points to the last node

    // Other private methods you may need
    BListStats bls;
    bool insert_;
    bool compare(T t, T t2);

  void sort(unsigned count, T * arr1, T * arr2, T input, bool left);

  int Bsearch(T input) const;
};


#include "BList.cpp"

#endif // BLIST_H
