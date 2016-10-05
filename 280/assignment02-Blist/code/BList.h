////////////////////////////////////////////////////////////////////////////////
#ifndef BLIST_H
#define BLIST_H
////////////////////////////////////////////////////////////////////////////////
#include <string>     // error strings
#include <stdexcept>  // std::exception

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
      BNode() : next(0), prev(0), count(0) {}
    };

    BList() :head_(0), tail_(0), bls() {};                 // default constructor                        
    BList(const BList &rhs){ // copy constructor
        BNode * tmp = rhs.head_, * frontTracker = 0;

        while (tmp) {
            BNode * makeNode = new BNode();
            makeNode->count = tmp->count;
            for (int i = 0; i < count; ++i)
                makeNode->values[i] = tmp->values[i];

            if (head) {
                frontTracker->next = makeNode;
                frontTracker->next->prev = frontTracker;
                frontTracker = frontTracker->next;
            }
            else {
                head_ = makeNode;
                head_->prev = 0;
                frontTracker = head;
            }
            

            if (!tmp->next) {
                tail_ = makeNode;
                tail_->next = 0;
            }

            tmp = tmp->next;
            
        }



    }
    ~BList();                // destructor                         

    BList& operator=(const BList &rhs) throw(BListException) {

    }

      // arrays will be unsorted
    void push_back(const T& value) throw(BListException){}
    void push_front(const T& value) throw(BListException) {
        
        if (head && head_->count < Size) 
            head_->values[head_->count++] = value;
        
        return;

        BNode * tmp = new BNode();
        tmp->values[0] = value;
        tmp->next = head_;
        tmp->prev = 0;

        if (tmp->next) {
            head->prev = tmp;
            head = tmp;
        }
        else {  // no head = no tail
            head_ = tmp;
            tail_ = head_;
        }
          
    };

      // arrays will be sorted
    void insert(const T& value) throw(BListException) {}

    void remove(int index) throw(BListException) {}
    void remove_by_value(const T& value) {}

    int find(const T& value) const {      // returns index, -1 if not found
    
        BNode * tmp = head_;
        while (tmp) {
            for (int i = 0; i < tmp->count; ++i) {
                if (tmp->values[i] == value)
                    return i;
            }
            tmp = tmp->next;
        }
    }

    T& operator[](int index) throw(BListException);             // for l-values
    const T& operator[](int index) const throw(BListException); // for r-values

    unsigned size(void) const {// total number of items (not nodes)
    
        BNode * tmp = head_;
        unsigned tmpNum = 0;
        while (tmp) {
            tmpNum += tmp->count;
            tmp = tmp->next;
        }
        return tmpNum;
    }

    void clear(void);          // delete all nodes 

    static unsigned nodesize(void); // so the allocator knows the size

      // For debugging
    const BNode *GetHead() const;
    BListStats GetStats() const { return bls; };

  private:
    BNode *head_; // points to the first node
    BNode *tail_; // points to the last node

    // Other private methods you may need
    BListStats bls;
};

#include "BList.cpp"

#endif // BLIST_H