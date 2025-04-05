#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    int getHeight(AVLNode<Key, Value>* node) const;
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void balance(AVLNode<Key, Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // Check if key already exists
    AVLNode<Key, Value>* existing = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
    if (existing != NULL) {
        existing->setValue(new_item.second);
        return;
    }

    // BST insert
    if (this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;
    while (curr != NULL) {
        parent = curr;
        if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        }
        else {
            curr = curr->getRight();
        }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }

    // Balance from the parent of the new node up to root
    curr = parent;
    while (curr != NULL) {
        balance(curr);
        curr = curr->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == NULL) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();

    // Case 1: No children
    if (node->getLeft() == NULL && node->getRight() == NULL) {
        if (parent == NULL) {
            this->root_ = NULL;
        }
        else if (parent->getLeft() == node) {
            parent->setLeft(NULL);
        }
        else {
            parent->setRight(NULL);
        }
        delete node;
    }
    // Case 2: One child
    else if (node->getLeft() == NULL || node->getRight() == NULL) {
        AVLNode<Key, Value>* child = (node->getLeft() != NULL) ? node->getLeft() : node->getRight();
        if (parent == NULL) {
            this->root_ = child;
        }
        else if (parent->getLeft() == node) {
            parent->setLeft(child);
        }
        else {
            parent->setRight(child);
        }
        child->setParent(parent);
        delete node;
    }
    // Case 3: Two children
    else {
      ///error
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, pred);

        AVLNode<Key, Value>* child = (node->getLeft() !=NULL)? node->getLeft() : node->getRight();
        parent = node->getParent();


        if(child !=NULL){
          child->setParent(parent);
        }
        if (parent==NULL){
          this->root_=child;
        }
        else{
          if (parent->getLeft()==node){
            parent->setLeft(child);
          }
          else{
            parent->setRight(child);
          }
        }
        delete pred;
        
      ///error
    }

    // Balance from the parent up to root
    AVLNode<Key, Value>* curr = parent;
    while (curr != NULL) {
        balance(curr);
        curr = curr->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

//Helpers below
template<class Key, class Value>
int AVLTree<Key, Value>::getHeight(AVLNode<Key, Value>* node) const
{
    if (node == NULL) {
        return 0;
    }
    int leftHeight = getHeight(node->getLeft());
    int rightHeight = getHeight(node->getRight());
    return 1 + std::max(leftHeight, rightHeight);
}

//rotators to abstract rotation code from above insert and delete functions
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightChild = node->getRight();
    if (rightChild == NULL) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != NULL) {
        rightChild->getLeft()->setParent(node);
    }
    rightChild->setLeft(node);
    rightChild->setParent(parent);
    node->setParent(rightChild);

    if (parent == NULL) {
        this->root_ = rightChild;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(rightChild);
    }
    else {
        parent->setRight(rightChild);
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* leftChild = node->getLeft();
    if (leftChild == NULL) {
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent();
    node->setLeft(leftChild->getRight());
    if (leftChild->getRight() != NULL) {
        leftChild->getRight()->setParent(node);
    }
    leftChild->setRight(node);
    leftChild->setParent(parent);
    node->setParent(leftChild);

    if (parent == NULL) {
        this->root_ = leftChild;
    }
    else if (parent->getLeft() == node) {
        parent->setLeft(leftChild);
    }
    else {
        parent->setRight(leftChild);
    }
}
template<class Key, class Value>
void AVLTree<Key, Value>::balance(AVLNode<Key, Value>* node)
{
    int leftHeight = getHeight(node->getLeft());
    int rightHeight = getHeight(node->getRight());
    int balanceFactor = leftHeight - rightHeight;
    node->setBalance(balanceFactor);

    if (balanceFactor > 1) {
        AVLNode<Key, Value>* leftChild = node->getLeft();
        int leftBalance = leftChild->getBalance();
        if (leftBalance >= 0) {
            rotateRight(node);
            node->setBalance(getHeight(node->getLeft()) - getHeight(node->getRight()));
            node->getParent()->setBalance(getHeight(node->getParent()->getLeft()) - getHeight(node->getParent()->getRight()));
        }
        else {
            rotateLeft(leftChild);
            rotateRight(node);
            node->setBalance(getHeight(node->getLeft()) - getHeight(node->getRight()));
            node->getParent()->setBalance(getHeight(node->getParent()->getLeft()) - getHeight(node->getParent()->getRight()));
            leftChild->setBalance(getHeight(leftChild->getLeft()) - getHeight(leftChild->getRight()));
        }
    }
    else if (balanceFactor < -1) {
        AVLNode<Key, Value>* rightChild = node->getRight();
        int rightBalance = rightChild->getBalance();
        if (rightBalance <= 0) {
            rotateLeft(node);
            node->setBalance(getHeight(node->getLeft()) - getHeight(node->getRight()));
            node->getParent()->setBalance(getHeight(node->getParent()->getLeft()) - getHeight(node->getParent()->getRight()));
        }
        else {
            rotateRight(rightChild);
            rotateLeft(node);
            node->setBalance(getHeight(node->getLeft()) - getHeight(node->getRight()));
            node->getParent()->setBalance(getHeight(node->getParent()->getLeft()) - getHeight(node->getParent()->getRight()));
            rightChild->setBalance(getHeight(rightChild->getLeft()) - getHeight(rightChild->getRight()));
        }
    }
}
#endif
