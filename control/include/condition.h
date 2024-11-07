
#ifndef __CONDITION_H__
#define __CONDITION_H__

#include <vector>
#include <iostream>
using namespace std;

class Node;

// status: stores the boolean result of evaluating this subtree.
// It represents whether the subtree rooted at this node is currently "true" or "false."
class Condition {
public:
    // status: contains what is the boolean value of this subtree
    bool status = false;
    // parents: stores pointers to all parent nodes that reference this node as a child.
    // This helps in optimizing the tree by allowing multiple parents to share the same subtree,
    // preventing duplication of identical subtrees across different parts of the tree.
    vector<Node *> parents;

    virtual ~Condition() = default;
};

#endif  // _CONDITION_H_
