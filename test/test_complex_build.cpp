#include <gtest/gtest.h>
#include "full_condition.h"
#include "global_properties.h"
#include "test_helper.h"

#pragma region Same Subtrees Condition Test

// Check using the same subtree in the same tree
TEST(InSameTree, IdenticalSubtrees)
{
    testCondition(
        "|([5]&(|(=(code,500),<(status,\"high\")),=(msg,\"aaa\")),[5]&(|(=("
        "code,500),<(status,\"high\")),>(msg,\"aaa\")))",
        8, "code", "800");
    GlobalProperties &instanceGP = GlobalProperties::getInstance();
    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition)) {
        EXPECT_EQ(
            ((OperatorNode *)on->conditions[1])->conditions[0]->parents.size(),
            2);
        EXPECT_EQ(
            ((OperatorNode *)on->conditions[0])->conditions[0]->parents.size(),
            2);
    }
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");
    // Check correct update
    instanceGP.sensors[5]->updateTrueRoots("code", "500");
    instanceGP.sensors[5]->updateTrueRoots("msg", "aaa");
    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check using the same subtree in the different trees
TEST(InDifferentTrees, IdenticalSubtrees)
{
    // Creates a `FullCondition` objects with the given parameters
    testCondition(
        "|([5]&(|(=(code,500),<(status,\"high\")),=(msg,\"aaa\")),[5]&(|(=("
        "code,500),<(status,\"high\")),>(msg,\"aaa\")))",
        8, "code", "800");

    GlobalProperties &instanceGP = GlobalProperties::getInstance();
    map<int, string> map = {{8, "the condition is true"}};
    FullCondition *cond =
        new FullCondition("[5]|(=(code,500),<(status,\"high\"))", map);
    instanceGP.conditions[cond->id] = cond;

    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition)) {
        EXPECT_EQ(
            ((OperatorNode *)on->conditions[1])->conditions[0]->parents.size(),
            3);
        EXPECT_EQ(
            ((OperatorNode *)on->conditions[0])->conditions[0]->parents.size(),
            3);
        EXPECT_EQ(
            ((OperatorNode *)instanceGP.conditions[1]->root->firstCondition)
                ->parents.size(),
            3);
    }
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");

    // Check correct update
    instanceGP.sensors[5]->updateTrueRoots("code", "500");
    instanceGP.sensors[5]->updateTrueRoots("msg", "aaa");
    EXPECT_EQ(instanceGP.trueConditions.size(), 2);
}

#pragma endregion

#pragma region Same Operators Condition Test

// Check building And operator into And operator
TEST(AndInAndCondition, SameOperatorsConditionTest)
{
    // Creates a `FullCondition` object with the given parameters
    testCondition("&([5]&(=(status,\"high\"),=(code,500)),[8]=(code,800))", 8,
                  "code", "800");

    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition))
        EXPECT_EQ(on->conditions.size(), 3);
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");

    // Check correct update
    instanceGP.sensors[5]->updateTrueRoots("status", "\"high\"");
    instanceGP.sensors[5]->updateTrueRoots("code", "500");

    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check building Or operator into Or operator
TEST(OrInOrCondition, SameOperatorsConditionTest)
{
    // Creates a `FullCondition` object with the given parameters
    testCondition("|([8]=(code,800),[5]|(=(status,\"high\"),=(code,500)))", 5,
                  "code", "500");

    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition))
        EXPECT_EQ(on->conditions.size(), 3);
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");

    // Check correct update
    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check building Complex Or operator into Or operator
TEST(ComplexOrInOrCondition, SameOperatorsConditionTest)
{
    // Creates a `FullCondition` object with the given parameters
    testCondition(
        "|([8]&(=(code,800),!=(msg,\"aaa\")),[5]|(=(code,500),|(=(status,"
        "\"high\"),=(msg,\"goog\"))))",
        8, "code", "800");

    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition))
        EXPECT_EQ(on->conditions.size(), 4);
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");

    // Check correct update
    instanceGP.sensors[8]->updateTrueRoots("msg", "\"good\"");

    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

#pragma endregion