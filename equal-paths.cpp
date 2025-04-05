#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int findDepth(Node* node);


bool equalPaths(Node * root) {
    //Two special cases. Empty tree and single node tree are technically equal pathed
    if (!root) {
        return true;
    }
    if (!root->left && !root->right) {
        return true;
    }
    // Use helper function ensuring not equal to fail number (-1)
    return findDepth(root) != -1;
}

//helper function to get the depth at any given node, simplifying equalPaths code
int findDepth(Node* node) {
    if (!node) {     //if null depth is 0
        return 0;
    }
    
   
    if (!node->left && !node->right) {    //if no left or right exist it is leaf and is depth 1
        return 1;
    }
    
    int leftDepth = findDepth(node->left);
    int rightDepth = findDepth(node->right);
    
    //Stops checking down and send failure up recursion stack if fail located
    if (leftDepth == -1 || rightDepth == -1) {
        return -1;
    }
    // if there are differing depths return negative one to tell function to fail(return -1)
    if (node->left && node->right && leftDepth != rightDepth) {
        return -1;
    }
    
    return 1 + (node->left ? leftDepth : rightDepth);
}
