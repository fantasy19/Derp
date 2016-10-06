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
		BNode() : next(0), prev(0), count(0) {}
	};

	BList() :head_(0), tail_(0), bls(nodesize(),0,Size,0) {};                 // default constructor                        
	BList(const BList &rhs) { // copy constructor
		
		BNode * tmp = rhs.head_, *frontTracker = 0;
		bls = rhs.bls;

		while (tmp) {
			BNode * makeNode = new BNode();
			makeNode->count = tmp->count;
			for (int i = 0; i < makeNode->count; ++i)
				makeNode->values[i] = tmp->values[i];

			if (head_) {
				frontTracker->next = makeNode;
				frontTracker->next->prev = frontTracker;
				frontTracker = frontTracker->next;
			}
			else {
				head_ = makeNode;
				head_->prev = 0;
				frontTracker = head_;
			}


			if (!tmp->next) {
				tail_ = makeNode;
				tail_->next = 0;
			}

			tmp = tmp->next;

		}


	}
	~BList() {                // destructor                         
		while (head_) {
			BNode * tmpN = head_;
			head_ = head_->next;
			delete tmpN;
		}
	}

	BList& operator=(const BList &rhs) throw(BListException) {

		while (head_) {
			BNode * tmpN = head_;
			head_ = head_->next;
			delete tmpN;
		}

		return BList(rhs);
	}

	// arrays will be unsorted
	void push_back(const T& value) throw(BListException) {

		if (tail_) {
			if (tail_->count < Size)  // got space
				tail_->values[tail_->count++] = value;
			else {
				tail_->next = new BNode();
				tail_->next->count = 1;
				tail_->next->prev = tail_;
				tail_->next->next = 0;
			//	std::cout << value << std::endl;
				tail_->next->values[0] = value;
			
				tail_ = tail_->next;
				++bls.NodeCount;
			}
			++bls.ItemCount;
			return;
		}
		

		if (!head_) { // no tail means no head
			head_ = new BNode();
			head_->prev =
			head_->next = 0;
			head_->values[0] = value;
			head_->count = 1;
			tail_ = head_;
			++bls.ItemCount;
			++bls.NodeCount;
		}

	}

	void push_front(const T& value) throw(BListException) {
		
		if (head_ && head_->count < Size) { // got space
			for (int i = head_->count; i != -1; --i)
				head_->values[i + 1] = head_->values[i];

			head_->values[0] = value;
			++head_->count;
			++bls.ItemCount;
			return;
		}

		// no space or empty list
		if (!head_) {
			head_ = new BNode();
			head_->prev =
			head_->next = 0;
			head_->values[0] = value;
			head_->count = 1;
			tail_ = head_;
			++bls.NodeCount;
		}
		else {
			BNode * tmp = new BNode();
			tmp->values[0] = value;
			tmp->count = 1;
			tmp->next = head_;
			head_ = tmp;
			++bls.NodeCount;
		}

		++bls.ItemCount;
	};


	// arrays will be sorted
	void insert(const T& value) throw(BListException) {
		
		BNode * tmp = head_;

		if (!head_) {
			push_front(value);
			return;
		}


		
		while (tmp->next) { // find fitting note
			if (compare(tmp->values[0], value) && !compare(tmp->next->values[0], value))
				break;

			tmp = tmp->next;
		}
		
		if (tmp->count < Size) { // got space
			T num; 
			int i = 0, j = Size - 1;

			for (; i < Size;++i) {
				if (i != 0)	
					num = tmp->values[i - 1];
				if (!compare(tmp->values[i], value)) {
					tmp->values[i - 1] = value;
					break;
				}
			}

			for (;j != (i - 1);--j) 
				tmp->values[j] = tmp->values[j - 1];
				
			tmp->values[j] = num;
		}
		else { // full at this point of time
			++bls.NodeCount;
			int i = 0;

			for (; i < Size;++i) {
				if (!compare(tmp->values[i], value)) {
					--i;
					break;
				}
			}

			if (i < (Size / 2)) {
				BNode * tmp2 = new BNode();
				tmp2->next = tmp;
				tmp2->prev = tmp->prev;
				for (int k = 0; k < i; ++k) {
					tmp2->values[k] = tmp2->next->values[k];
					//tmp2->next->values[k] = tmp2->next->values[k + 1];
				}

				tmp2->next->count -= i+1;
				tmp2->count += i+1;
				tmp2->values[i] = value;
				tmp->prev = tmp2;
				if (tmp->prev)
					tmp2->prev->next = tmp2;
			}
			else {
				BNode * tmp3 = new BNode();
				tmp3->prev = tmp;
				tmp3->next = tmp->next;
				tmp3->values[0] = value;
				for (int k = i, l = 1; k < Size; ++k, ++l) {
					tmp3->values[l] = tmp3->prev->values[k];
					//tmp3->prev->values[k] = 0;
				}
				
				tmp3->prev->count -= i + 1;
				tmp3->count += i + 1;
				if (tmp->next)
					tmp3->next = tmp->next->next;
				tmp->next = tmp3;
			}


		}

		++bls.ItemCount;

		
	}


	

    void remove(int index) throw(BListException) {}
    void remove_by_value(const T& value) {}

    int find(const T& value) const {      // returns index, -1 if not found
    
        BNode * tmp = head_;
        while (tmp) {
            for (unsigned i = 0; i < tmp->count; ++i) {
                if (tmp->values[i] == value)
                    return i;
            }
            tmp = tmp->next;
        }

		return -1;
    }

	T& operator[](int index) throw(BListException) { 
		return *const_cast<T*>(&this->operator[](index));
	}
	// for l-values
	const T& operator[](int index) const throw(BListException) { 
		
		BNode * tmp = head_;
		T num;
		while (tmp && index) {
			for (int i = 0; i < tmp->count; ++i) {
				num = tmp->values[i];
				--index;
			}
			tmp = tmp->next;
		}
		return num;
	}; // for r-values

    unsigned size(void) const {// total number of items (not nodes)
    
        BNode * tmp = head_;
        unsigned tmpNum = 0;
        while (tmp) {
            tmpNum += tmp->count;
            tmp = tmp->next;
        }
        return tmpNum;
    }

	void clear(void) {};          // delete all nodes 

    static unsigned nodesize(void); // so the allocator knows the size

      // For debugging
    const BNode *GetHead() const;
    BListStats GetStats() const { return bls; };

  private:
    BNode *head_; // points to the first node
    BNode *tail_; // points to the last node

    // Other private methods you may need
    BListStats bls;

	bool compare(T t, T t2) { return (t < t2); }
};


#include "BList.cpp"

#endif // BLIST_H