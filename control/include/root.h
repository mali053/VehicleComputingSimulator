#ifndef __ROOT_H__
#define __ROOT_H__

#include "node.h"
#include "global_properties.h"

// Root node - the first node of the tree
class Root : public Node {
public:
    // Tree id
    int conditionId;
    // Pointer to the first node of the tree
    Condition *firstCondition;

    // C-tor that gets an id and initializes the conditionId
    Root(int id, Condition *firstCondition)
        : conditionId(id), firstCondition(firstCondition){};

    // Function that updates the status of all nodes that should change
    void updateTree() override;
};

#endif  // _ROOT_H_