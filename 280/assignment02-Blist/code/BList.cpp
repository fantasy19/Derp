template <typename T, unsigned Size>
unsigned BList<T, Size>::nodesize(void)
{
  return sizeof(BNode);
}

template <typename T, unsigned Size>
const typename BList<T, Size>::BNode* BList<T, Size>::GetHead() const
{
  return head_; 
}

template <typename T, unsigned Size>
BList<T, Size>::BList():head_(0), tail_(0), bls(nodesize(), 0, Size, 0), insert_(false){}

template <typename T, unsigned Size>
BList<T, Size>::BList(const BList &rhs){
    BNode * tmp = rhs.head_, *frontTracker = 0;
    bls = rhs.bls;
    head_ = 0;
    tail_ = 0;
    while (tmp) {
        BNode * makeNode = 0;
        try{
            makeNode = new BNode();
        }
        catch (BListException &){
            throw(BListException(BListException::E_NO_MEMORY, "no memory"));
        }
        makeNode->count = tmp->count;
        for (unsigned i = 0; i < makeNode->count; ++i)
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
            tail_ = frontTracker;
            frontTracker->next = 0;
        }

        tmp = tmp->next;

    }
}

template <typename T, unsigned Size>
BList<T, Size>::~BList(){ clear(); }

template <typename T, unsigned Size>
BList<T, Size>& BList<T, Size>::operator=(const BList &rhs) throw(BListException){

    clear();
    BNode * tmp = rhs.head_, *frontTracker = 0;
    bls = rhs.bls;

    while (tmp) {
        BNode * makeNode = 0;
        try{
            makeNode = new BNode();
        }
        catch (BListException &){
            throw(BListException(BListException::E_NO_MEMORY, "no memory"));
        }
        makeNode->count = tmp->count;
        for (unsigned i = 0; i < makeNode->count; ++i)
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
    return *this;
    
}

template <typename T, unsigned Size>
void BList<T, Size>::push_back(const T& value) throw(BListException) {

    if (tail_) {
        if (tail_->count < Size)  // got space
            tail_->values[tail_->count++] = value;
        else {

            try{
                tail_->next = new BNode();
            }
            catch (BListException &){
                throw(BListException(BListException::E_NO_MEMORY, "no memory"));
            }
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
        try{
            head_ = new BNode();
        }
        catch (BListException &){
            throw(BListException(BListException::E_NO_MEMORY, "no memory"));
        }
        head_->prev =
            head_->next = 0;
        head_->values[0] = value;
        head_->count = 1;
        tail_ = head_;
        ++bls.ItemCount;
        ++bls.NodeCount;
    }

}

template <typename T, unsigned Size>
void BList<T, Size>::push_front(const T& value) throw(BListException) {

    if (head_ && head_->count < Size) { // got space
        for (int i = head_->count - 1; i != -1; --i)
            head_->values[i + 1] = head_->values[i];

        head_->values[0] = value;
        ++head_->count;
        ++bls.ItemCount;
        return;
    }

    // no space or empty list
    if (!head_) {
        try{
            head_ = new BNode();
        }
        catch (BListException &){
            throw(BListException(BListException::E_NO_MEMORY, "no memory"));
        }
        head_->prev =
            head_->next = 0;
        head_->values[0] = value;
        head_->count = 1;
        tail_ = head_;
        ++bls.NodeCount;
    }
    else {
        BNode * tmp = 0;
        try{
            tmp = new BNode();
        }
        catch (BListException &){
            throw(BListException(BListException::E_NO_MEMORY, "no memory"));
        }
        tmp->values[0] = value;
        tmp->count = 1;
        tmp->next = head_;
        head_->prev = tmp;
        head_ = tmp;
        ++bls.NodeCount;
    }

    ++bls.ItemCount;
}

template <typename T, unsigned Size>
void BList<T, Size>::insert(const T& value) throw(BListException) {
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

        unsigned i = 0, j = tmp->count;

        for (; i < tmp->count; ++i) {
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

        BNode* tmp2 = 0;
        try{
            tmp2 = new BNode();
        }
        catch (BListException &){
            throw(BListException(BListException::E_NO_MEMORY, "no memory"));
        }
        unsigned one = 1;
        if (Size != one) {
            unsigned i = 0;

            for (; i < tmp->count; ++i) {
                if (!compare(tmp->values[i], value))
                    break;
            }

            if (i == tmp->count && tmp->next && tmp->next->count < Size) {
                delete tmp2;
                for (int i = tmp->next->count; i > 0; --i)
                    tmp->next->values[i] = tmp->next->values[i - 1];

                tmp->next->values[0] = value;
                ++tmp->next->count;
                ++bls.ItemCount;
                return;
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

template <typename T, unsigned Size>
void BList<T, Size>::remove(int index) throw(BListException) {
    try{
        this->operator[](index);
    }
    catch (BListException &){
        throw BListException(BListException::E_BAD_INDEX, "Out of range");
    }
    BNode * tmp = head_;
    bool match = false;
    T num = this->operator[](index);
    int j = 0;
    while (tmp) {
        for (unsigned i = 0; i < tmp->count; ++i) {
            if (tmp->values[i] == num) {
                j = i;
                match = true;
                break;
            }

        }
        if (match)
            break;
        tmp = tmp->next;
    }

    if (tmp) {
        if (tmp->count == 1) {
            BNode * me = tmp;
            if (tmp->prev)
                tmp->prev->next = tmp->next;

            if (tmp->next)
                tmp->next->prev = tmp->prev;

            if (tmp == head_)
                head_ = head_->next;

            if (tmp == tail_)
                tail_ = tail_->prev;
            delete me;
            --bls.NodeCount;
        }
        else {
            for (unsigned i = j; i < tmp->count - 1; ++i) {
                tmp->values[i] = tmp->values[i + 1];
            }
            --tmp->count;
        }
        --bls.ItemCount;
    }

}

template <typename T, unsigned Size>
void BList<T, Size>::remove_by_value(const T& value) {
    remove(find(value));
}

template <typename T, unsigned Size>
int BList<T, Size>::find(const T& value) const {      // returns index, -1 if not found
    if (insert_){
        //  std::cout << value << " is " << this->operator[](found) << std::endl;
        return Bsearch(value);
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

template <typename T, unsigned Size>
T& BList<T, Size>::operator[](int index) throw(BListException) {
    if (index == -1 || static_cast<unsigned>(index) > bls.ItemCount)
        throw BListException(BListException::E_BAD_INDEX, "Out of range");

    BNode * tmp = head_;
    T * num = 0;
    while (tmp && index != -1) {
        for (unsigned i = 0; i < tmp->count && index != -1; ++i) {
            num = &tmp->values[i];
            --index;
        }
        tmp = tmp->next;
    }

    return *num;
}

template <typename T, unsigned Size>
const T& BList<T, Size>::operator[](int index) const throw(BListException) {
    if (index == -1 || static_cast<unsigned>(index) > bls.ItemCount)
        throw BListException(BListException::E_BAD_INDEX, "Out of range");

    BNode * tmp = head_;
    T * num = 0;
    while (tmp && index != -1) {
        for (unsigned i = 0; i < tmp->count && index != -1; ++i) {
            num = &tmp->values[i];
            --index;
        }
        tmp = tmp->next;
    }
    return *num;
}

template <typename T, unsigned Size>
unsigned BList<T, Size>::size(void) const {
    return bls.ItemCount;
}

template <typename T, unsigned Size>
void BList<T, Size>::clear(void) {
    BNode * tmp = head_;
    while (tmp) {
        BNode * me = tmp;
        tmp = tmp->next;
        delete me;
    }
    head_ = 0;
    tail_ = 0;
    bls.ItemCount = 0;
    bls.NodeCount = 0;
}

template <typename T, unsigned Size>
BListStats BList<T, Size>::GetStats() const { return bls; }

template <typename T, unsigned Size>
void BList<T, Size>::sort(unsigned count, T * arr1, T * arr2, T input, bool left) {

    T  oneArray[Size + 1];
    int j = -1;

    for (unsigned i = 0; i < Size + 1; ++i) {

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
        for (unsigned i = 0; i < Size / 2 + 1; ++i)
            arr2[i] = oneArray[i];

        for (unsigned i = Size / 2 + 1, l = 0; i < Size + 1; ++i, ++l)
            arr1[l] = oneArray[i];
    }
    else {
        for (unsigned i = 0; i < Size / 2; ++i)
            arr2[i] = oneArray[i];

        for (unsigned i = Size / 2, m = 0; i < Size + 1; ++i, ++m)
            arr1[m] = oneArray[i];
    }

}

template <typename T, unsigned Size>
int BList<T, Size>::Bsearch(T input) const{
    if (bls.ItemCount <= 1)
        return 1;

    int left = 0, right = bls.ItemCount - 1;
    while (right >= left) {
        int middle = left + (right - left) / 2;
        if (input == this->operator[](middle)) {
            return middle;
        }

        if (input < this->operator[](middle))
            right = middle - 1;
        else
            left = middle + 1;

    }
    return -1;
}

template <typename T, unsigned Size>
bool BList<T, Size>::compare(T t, T t2) { return (t < t2); }