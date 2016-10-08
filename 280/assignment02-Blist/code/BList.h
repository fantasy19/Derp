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

	BList() :head_(0), tail_(0), bls(nodesize(),0,Size,0), insert_(false) {};                 // default constructor                        
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
        insert_ = true;
		BNode * tmp = head_;

		if (!head_) {
			push_front(value);
			return;
		}
		
		while (tmp) { // find fitting note
            if (!compare(tmp->values[0], value)) {
                tmp = tmp->prev;
                break;
            }

			tmp = tmp->next;
		}
		
        if (value < head_->values[0])
            tmp = head_;

        if (!tmp)
            tmp = tail_;

        if (tmp->count < Size) { // got space

            int i = 0, j = tmp->count;

		    for (; i < tmp->count;++i) {
				if (!compare(tmp->values[i], value)) {
					
					break;
				}
		    }
            
            for (; j > i; --j)
                 tmp->values[j] = tmp->values[j - 1];

			tmp->values[i] = value;
            ++tmp->count;
		}
		else { // full at this point of time
			
            BNode* tmp2 = new BNode();

            if (Size != 1) {
                int i = 0;

                for (; i < tmp->count; ++i) {
                    if (!compare(tmp->values[i], value))
                        break;
                }

				tmp2->next = tmp;
				tmp2->prev = tmp->prev;
                
				if (!tmp2->prev)
					head_ = tmp2;

				sort(tmp->count, tmp->values, tmp2->values, value, (i <= tmp->count / 2));
				if (i <= tmp->count / 2) {
					tmp2->count = Size / 2 + 1;
					tmp->count = Size / 2;
				}
				else {
					tmp2->count = Size / 2;
					tmp->count = Size / 2 + 1;
				}

                if (tmp2 != head_)
                    tmp2->prev->next = tmp2;

                tmp->prev = tmp2;

                if (!tmp->next)
                    tail_ = tmp;
			}
			else{
                tmp2->count = 1;
                tmp2->next = tmp->next;
                if (tmp2->next)
                    tmp2->next->prev = tmp2;
                tmp2->prev = tmp;
                tmp->next = tmp2;
                
                BNode *tmpNode = head_;

                while (tmpNode) {
                    if (value < tmpNode->values[0]) {
                        if (tmpNode != head_) {
                            tmpNode = tmpNode->prev;
                            tmpNode->values[0] = value;
                        }
                        else {
                            tmpNode->next->values[0] = tmpNode->values[0];
                            tmpNode->values[0] = value;
                        }
                        break;
                    }
                    

                    if (!tmpNode->next)
                        tmpNode->values[0] = value;

                    tmpNode = tmpNode->next;
                }
                

                if (!tmp2->next)
                    tail_ = tmp2;
                
            }

            ++bls.NodeCount;
		}

		++bls.ItemCount;

	}

    void remove(int index) throw(BListException) {
        BNode * tmp = head_; 
        int j = 0;
        while (tmp) {
            bool break_ false;
            for (int i = 0; i < tmp->count; ++i) {
                if (tmp->values[i] == this->operator=[](index)) {
                    j = i;
                    break_ = true;
                    break;
                }
            }

            if (break_)
                break;

            tmp = tmp->next;
        }

        if (tmp) {
            if (tmp->count == 1) {

            }
            else {
                for (int i = j; i > tmp->count - 1; ++i) {
                    tmp->values[i] = tmp->values[i + 1];
                }
                --tmp->count;
            }
        }
    }
    void remove_by_value(const T& value) {}

    int find(const T& value) const {      // returns index, -1 if not found
        if (insert_){
            T num = value;
            int found = Bsearch(value);
          //  std::cout << value << " is " << this->operator[](found) << std::endl;
            return found;
        }
        else {
            int rtnVal = 0;
            BNode * tmp = head_;
            while (tmp) {
                for (unsigned i = 0; i < tmp->count; ++i) {
                    if (tmp->values[i] == value)
                        return rtnVal;

                    ++rtnVal;
                }
                tmp = tmp->next;
            }
        }
		return -1;
    }

    T& operator[](int index) throw(BListException) {
        BNode * tmp = head_;
        T num{};
        while (tmp && index != -1) {
            for (int i = 0;i < tmp->count && index != -1; ++i) {
                num = tmp->values[i];
                --index;
            }
            tmp = tmp->next;
        }

        return num;
    }
	
	// for l-values
	const T& operator[](int index) const throw(BListException) { 
		
        BNode * tmp = head_;
        T num{};
        while (tmp && index != -1) {
            for (int i = 0; i < tmp->count && index != -1; ++i) {
                num = tmp->values[i];
                --index;
            }
            tmp = tmp->next;
        }
        return num;
	}; // for r-values

    

    unsigned size(void) const {// total number of items (not nodes)
    
        return bls.ItemCount;
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
    bool insert_;
	bool compare(T t, T t2) { return (t < t2); }

	void sort(int count, T * arr1, T * arr2, T input, bool left) {
      
		T  oneArray[Size+1] ;
		int j = -1;

		for (int i = 0; i < Size + 1; ++i) {

			if (!compare(input, arr1[i]) && i < count)
				oneArray[i] = arr1[i];
			else {
				if (j == -1) j = i;
				if (i < count)
					oneArray[i + 1] = arr1[i];
			}
			
		}

		oneArray[j] = input;
		
		if (left) {
			for (int i = 0;i < Size/2+1;++i)
				arr2[i] = oneArray[i];

			for (int i = Size/2+1, l = 0;i < Size + 1;++i, ++l)
				arr1[l] = oneArray[i];
		}
		else {
			for (int i = 0;i < Size/2;++i)
				arr2[i] = oneArray[i];

			for (int i = Size / 2, m = 0;i < Size + 1;++i,++m) 
				arr1[m] = oneArray[i];
		}
		
        /*
        for (int i = 0; i < Size; ++i)
            std::cout << arr1[i] << " ";

    std::cout << std::endl;
    for (int i = 0; i < Size; ++i)
        std::cout << arr2[i] << " ";

    std::cout << std::endl;
        */
		
	}

    int Bsearch(T input) const{
        if (bls.ItemCount <= 1) 
            return 1;
        
        int left = 0, right = bls.ItemCount - 1;
        while (right >= left) {
            int middle = left + (right - left) / 2;
            if (input == this->operator[](middle)) {
               // std::cout << this->operator[](middle) << std::endl;
                return middle;
            }
            
            if (input < this->operator[](middle))
                right = middle - 1;
            else
                left = middle + 1;

        }
        return -1;
    }
};


#include "BList.cpp"

#endif // BLIST_H