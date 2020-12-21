// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include "Eecs281PQ.h"

// A specialized version of the 'heap' ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
            this->compare = comp;
    } // BinaryPQ


    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function. FINISH
            this->compare = comp;
            //start from bottom work way to top
            //fix down each time new item is added
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automaticslly.
    virtual ~BinaryPQ() {
    } // ~BinaryPQ()


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
    } // updatePriorities()


    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
        data[++data.size()] = val;
        data.fixUp(data.size());
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        getElement(1) = getElement(data.size()--);
        fixDown(getElement(1));
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
        // TODO: Implement this function.
        return getElement(1);
    } // top()


    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function.  Might be very simple,
        // depending on your implementation.
        return data.size();
    } // size()


    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function.  Might be very simple,
        // depending on your implementation.
        return data.empty();
    } // empty()


private:
    // Note: This vector *must* be used your heap implementation.
    std::vector<TYPE> data;

    // TODO: Add any additional member functions or data you require here.
    // For instance, you might add fixUp() and fixDown().
    TYPE &getElement(std::size_t i) {
        return data[i-1];
    } //getElement()
    
    const TYPE &getElement(std::size_t i) const {
        return data[i-1];
    } //getElement()
    
    void fixUp(int index) { //would index > 1 still be ok since using getElement()?
        while (index > 1 && getElement(index/2) < getElement(index)) {
            std::swap(getElement(index), getElement(index/2));
            index /= 2;
        }
    } // fixUp()
    
    void fixDown(int index) {
        while (2*index <= data.size()) {
            // j = most extreme item, starts as left child
            int j = 2*index;
            // if right child exists and right child is more extreme than left, set j to right child
            if (j < data.size() && this->compare(getElement(j),getElement(j+1))) { //is it size or size-1
                ++j;
            }
            // if item is >= most extreme child, heap is correctly ordered
            if (!this->compare(getElement(index), getElement(j))) { //does !compare include >=
                break;
            }
            std::swap(getElement(index), getElement(j));
            index = j;
        }
    } // fixDown()

}; // BinaryPQ


#endif // BINARYPQ_H
