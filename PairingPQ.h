#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

using namespace std;

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;
    
public:
    // Each node within the pairing heap
    class Node {
    public:
        // TODO: After you add add one extra pointer (see below), be sure to
        // initialize it here.
        explicit Node(const TYPE &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, previous{ nullptr }
        {}
        
        // Description: Allows access to the element at that Node's position.
        // There are two versions, getElt() and a dereference operator, use
        // whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }
        
        // The following line allows you to access any private data members of this
        // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
        // statement in PairingPQ's add_node() function).
        friend PairingPQ;
        
    private:
        TYPE elt;
        Node *child;
        Node *sibling;
        Node *previous;
        // TODO: Add one extra pointer (parent or previous) as desired.
    }; // Node
    
    
    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp } {
        // TODO: Implement this function.
    } // PairingPQ()
    
    
    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp } {
        // TODO: Implement this function.
        // cout << "    >>>>trace: Using vector int the contructor. data size: " << data.size() << endl;
        // Iterate through the input data, and add them as nodes
        for (InputIterator i = start; i != end; i++) {
            addNode(*i);
        }
    } // PairingPQ()
    
    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
    BaseClass{ other.compare } {
        // TODO: Implement this function.
        
        // cout << "    >>>>trace: in Copy constructor! other size = " << other.size() << ", other.root is: " << other.root->getElt() << endl;
        
        Node* current = other.root;
        
        // TODO: The following code is exactly the same as in the Copy assignment
        // Can we combine them?
        deque<Node*> temp;
        temp.push_back(current);
        
        while (!temp.empty()) {
            current = temp.front();
            
            // Add the child and sibling to the temp stack, if any
            if (current->child != NULL) {
                temp.push_back(current->child);
            }
            if (current->sibling != NULL) {
                temp.push_back(current->sibling);
            }
            // Add the current node to heap
            addNode(current->getElt());
            
            // remove the current node from the temp stack
            temp.pop_front();
        }
        
        // cout << "    >>>>trace: Done with Copy contructor! pq_size: " << pq_size << ", other size: " << other.size() << endl;
        
    } // PairingPQ()
    
    
    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ& operator=(const PairingPQ& rhs) {
        // TODO: Implement this function.
        // FINISH?
        PairingPQ temp(rhs);
        std::swap(this->pq_size, temp.pq_size);
        std::swap(this->root, temp.root);
        return *this;
    } // operator=()
    
    
    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        
        if (root == NULL) return;
        
        deque<Node*> temp;
        
        temp.push_back(root);
        
        Node* current = root;
        
        while (!temp.empty()) {
            current = temp.front();
            
            // Add the child and sibling to the temp stack, if any
            if (current->child != NULL) {
                temp.push_back(current->child);
            }
            if (current->sibling != NULL) {
                temp.push_back(current->sibling);
            }
            
            // remove the current node from the temp stack
            temp.pop_front();
            
            // Delete the node
            // cout << "    >>>>trace: deleting: " << current->getElt() << endl;
            delete current;
        }
        
    } // ~PairingPQ()
    
    
    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        // cout << "    >>>>trace: updatePriorities(), root elt: " << root->getElt() << endl;
        
        // If there is only one node in the heap, there is no need to update the heap
        if (pq_size < 2) {
            return;
        }
        
        deque<Node*> temp;
        temp.push_back(root);
        
        Node* current = root;
        Node* newNode = nullptr;
        
        while (!temp.empty()) {
            current = temp.front();
            
            // Add the child and sibling to the temp stack, if any
            if (current->child != NULL) {
                temp.push_back(current->child);
            }
            if (current->sibling != NULL) {
                temp.push_back(current->sibling);
            }
            // Isolate the current node
            current->child = NULL;
            current->sibling = NULL;
            current->previous = NULL;
            
            // meld the current node with the newNode
            newNode = meld(current, newNode);
            
            // remove the current node from the temp stack
            temp.pop_front();
        }
        
        // Make the root node to point to the newNode
        root = newNode;
        
        // cout << "    >>>>trace: done calling updatePriorities(). new root val: " << root->getElt() << endl;
    } // updatePriorities()
    
    // Description: Add a new element to the priority_queue. This is almost done,
    //              in that you should implement push functionality in the addNode()
    //              function, and this function should call addNode().
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
        // TODO: Implement this function.
        
        addNode(val);
        
    } // push()
    
    
    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority_queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority_queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        deque<Node*> temp;
        
        // cout << "pop() is called! pq_size: " << pq_size << endl;
        
        if (root == NULL) {
            return;
        }
        if (root->child == NULL) {
            pq_size = 0;
            delete root;
            root = NULL;
        }
        else {
            Node *tn = root->child;
            
            // Get all the second level sibings and hold them in a temp queue
            while (tn != NULL) {
                tn->previous = NULL;
                temp.push_back(tn);
                tn = tn->sibling;
            }
            
            // cout << endl << "Done pushing the siblings!  temp size is: " << temp.size() << endl << endl;
            
            // Performing the pairing on the temp queue by melding two nodes at
            // a time, from left to right (make sure not from the right!)
            while (temp.size() > 1) {
                
                // cout << "    >>>>trace: pairing two siblings..." << endl;
                
                Node* first = temp.front();
                temp.pop_front();
                Node* second = temp.front();
                temp.pop_front();
                Node* melded = meld(first, second);
                temp.push_back(melded);
            }
            
            // The new root is the only remaining element in the queue!
            // delete root;
            // delete tn;
            
            root = temp.front();
            
            // Finally decrease the pq_size by 1
            --pq_size;
            
            // cout << "    >>>>trace: after pop(), the pq looks like: " << endl;
            // printPQ();
        }
        
    } // pop()
    
    
    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
        // TODO: Implement this function
        // TODO: handle no root case!
        return root->getElt();
        
    } // top()
    
    
    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual size_t size() const {
        // TODO: Implement this function
        // Done!
        return pq_size;
    } // size()
    
    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        // Done!
        return (pq_size == 0);
        
    } // empty()
    
    
    // Description: Updates the priority of an element already in the priority_queue by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain priority_queue invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE & new_value) {
        // TODO: Implement this function
        // TODO: since the new value is more extreme, can we only update partial tree?
        
        // If the node is the root node, or new_value is less than the value in node,
        // do nothing
        if (this->compare(new_value, node->getElt()) || root == node) {
            // cout << "    >>>>trace: No need to update element!" << endl;
            return;
        }
        
        // Reset the node value with new value
        node->elt = new_value;
        
        // Remove the node from the tree as a standalone max tree
        // If the node is a leftmost node, remove itself as a child from the parent
        if (node == node->previous->child) {
            node->previous->child = NULL;
        }
        // Always cut the previous link
        node->previous = NULL;
        
        // meld the sub tree with root node and make the root to be the returned node
        root = meld(node, root);
        
        // cout << "    >>>>trace: done updateElt: new root val: " << root->getElt() << endl;
    } // updateElt()
    
    
    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE & val) {
        // TODO: Implement this function
        Node* newNode = new Node(val);
        
        // If there is no root yet, the new node is the root
        if (pq_size == 0) {
            root = newNode;
        }
        else {
            if (this->compare(root->getElt(), val)) {
                newNode->child = root;
                newNode->sibling = NULL;
                root->previous = newNode;
                root = newNode;
            }
            else {
                newNode->previous = root;
                newNode->sibling = root->child;
                root->child = newNode;
            }
        }
        
        // Inrease the queue size count
        pq_size++;
        
        return newNode;
    } // addNode()
    
    
private:
    // TODO: Add any additional member functions or data you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
    Node* root;
    size_t pq_size = 0;
    
    // meld from to 'to'. from can not be NULL. 'to' could be NULL
    //
    // PairingPQ* meld_pq(PairingPQ* &from, PairingPQ* &to) {
    //     if (from == NULL) {
    //         return *to;
    //     }
    //     if (to == NULL) {
    //         return *from;
    //     }
    //
    //     // If 'to' is more extreme, make 'from' as child of 'to'
    //     if (this->compare(from->top(), to->top())) {
    //         from->previous = to;
    //         from->sibling = to->child;
    //         to->child = from;
    //
    //         return *to;
    //     }
    //     else {
    //         to->provious = from;
    //         to->sibling = from->child;
    //         from->child = to;
    //
    //         return *from;
    //     }
    // }
    
    Node* meld(Node* &from, Node* &to) {
        // TODO: should we not allow from to be NULL?
        if (from == NULL) {
            return to;
        }
        if (to == NULL) {
            return from;
        }
        
        // cout << "    >>>>trace: meld is called: from val: " << from->getElt() << ", to val: " << to->getElt() << endl;
        // If 'to' is more extreme, make 'from' as child of 'to'
        if (this->compare(from->getElt(), to->getElt())) {
            from->previous = to;
            from->sibling = to->child;
            to->child = from;
            to->sibling = NULL;
            
            return to;
        }
        else {
            to->previous = from;
            to->sibling = from->child;
            from->child = to;
            from->sibling = NULL;
            
            return from;
        }
    }
    
    // A private method to print the queue out for testing
    void printPQ() {
        deque<Node*> temp;
        
        temp.push_back(root);
        
        Node* current = root;
        
        while (!temp.empty()) {
            current = temp.front();
            
            // Add the child and sibling to the temp stack, if any
            if (current->child != NULL) {
                temp.push_back(current->child);
            }
            if (current->sibling != NULL) {
                temp.push_back(current->sibling);
            }
            
            if (current->previous != NULL && current == current->previous->child) {
                cout << "\n" << current->getElt() << " ";
            }
            else {
                cout << current->getElt() << " ";
            }
            // remove the current node from the temp stack
            temp.pop_front();
            
        }
        
        
    }
};


#endif // PAIRINGPQ_H
