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
				tail_->values[++tail_->count] = value;
			else {
				tail_->next = new BNode();
				tail_->next->count = 1;
				tail_->next->prev = tail_;
				tail_->next->next = 0;
			}
		}
		return;

		if (!head_) { // no tail means no head
			head_ = new BNode();
			head_->prev =
				head_->next = 0;
			head_->values[0] = value;
			head_->count = 1;
		}

	}

	void push_front(const T& value) throw(BListException) {

		if (head_ && head_->count < Size) { // got space
			for (int i = head_->count;i != -1;--i)
				head_->values[i + 1] = head_->values[i];

			head_->values[0] = value;


			head_->values[head_->count++] = value;
		}

		return;

		// no space or empty list
		if (!head_) {
			head_ = new BNode();
			head_->prev =
				head_->next = 0;
			head_->values[0] = value;
			head_->count = 1;
		}
		else {
			BNode * tmp = new BNode();
			tmp->values[0] = value;
			tmp->count = 1;
			tmp->next = head_;
			head_ = tmp;
		}
	};

	// arrays will be sorted
	void insert(const T& value) throw(BListException) {

		BNode * tmp = head_, *tmp2 = 0;

		if (!tmp)
			push_front(value);

		return;

		while (tmp) { // slot first
			for (unsigned i = 0; i < tmp->count;++i) {
				if (tmp->values[i] > value) {
					if (tmp->count < Size) {
						tmp->values[count++] = value;
						tmp2 = tmp;
						tmp = tail_;
						break;
					}
					else {
						if (tmp == head_) {
							push_front(value);
							tmp2 = head_;
						}
						else
							if (tmp == tail_) {
								push_back(value);
								tmp2 = tail_;
							}

							else {
								BNode * tmp3 = tmp->next;
								tmp->next = new BNode();
								tmp2 = tmp->next;
								tmp->next->next = tmp3;
								tmp->next->prev = tmp;
								tmp->next->count = 1;
								tmp->next->value[0] = value;
							}
					}


				}
			}
		}
		//then arrange
		if (tmp2 != head_ && tmp2 != tail_) {

		}
		else {
			if (tmp2 == head_) {
				// sort 1st and 2nd node
				bool insert = false;
				int i = 0;// count starts at 1 by now
				while (tmp2->count != Size / 2) {

					if (insert) {
						if (value > tmp2->next->values[i]) {
							tmp2->values[i] = tmp2->next->values[i++];
							++tmp2->count;
						}
						else {
							tmp2->values[count] = value;
							++i;
							insert = true;
						}
					}
					else {
						tmp2->values[i] = tmp2->next->values[i++];
						++tmp2->count;
					}
				
				}
				i = tmp2->next->count - tmp2->count;
				int j = 0;
				while (j != i) {
					tmp2->next->values[j] = tmp2->next->values[j + 1];
					--tmp2->next->count;
					++j;
				}
			}

			}

			if (tmp2 == tail_) {
				// sort tail and 2nd last node
				int tmpNum = (tmp2->prev->count / 2);
				for (int i = 0; i < tmpNum;++i) {
					push_back(tmp2->prev->values[i]);
					--tmp2->prev->count;
					++tmp2->count;
				}
				// then sort

			}
		}


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

	T& operator[](int index) throw(BListException) { T tmp; return tmp; }             // for l-values
	const T& operator[](int index) const throw(BListException) { T tmp; return tmp; }; // for r-values

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
};

#include "BList.cpp"

#endif // BLIST_H