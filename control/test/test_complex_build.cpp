#include <gtest/gtest.h>
#include "full_condition.h"
#include "global_properties.h"
#include "test_helper.h"

#pragma region Same Subtrees Condition Test

// Check using the same subtree in the same tree
TEST(InSameTree, IdenticalSubtrees)
{
    int *num = new int(800);
    void *vptr = &num;
    testCondition(
        "|([1]&(|(=(code,500),<(status,\"high\")),=(msg,\"aaa\")),[1]&(|(=("
        "code,500),<(status,\"high\")),>(msg,\"aaa\")))",
        2, "code", vptr, FieldType::SIGNED_INT);
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
    num = new int(500);
    vptr = &num;
    instanceGP.sensors[1]->updateTrueRoots("code", vptr, FieldType::SIGNED_INT);
    char *str = "aaa";
    vptr = &str;
    instanceGP.sensors[1]->updateTrueRoots("msg", vptr, FieldType::CHAR_ARRAY);
    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check using the same subtree in the different trees
TEST(InDifferentTrees, IdenticalSubtrees)
{
    int *num = new int(800);
    void *vptr = &num;
    // Creates a `FullCondition` objects with the given parameters
    testCondition(
        "|([1]&(|(=(code,500),<(status,\"high\")),=(msg,\"aaa\")),[1]&(|(=("
        "code,500),<(status,\"high\")),>(msg,\"aaa\")))",
        2, "code", vptr, FieldType::SIGNED_INT);

    GlobalProperties &instanceGP = GlobalProperties::getInstance();
    vector<pair<int, string>> vec = {{2, "the condition is true"}};
    FullCondition *cond =
        new FullCondition("[1]|(=(code,500),<(status,\"high\"))", vec);
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
    num = new int(500);
    vptr = &num;
    instanceGP.sensors[1]->updateTrueRoots("code", vptr, FieldType::SIGNED_INT);
    char *str = "aaa";
    vptr = &str;
    instanceGP.sensors[1]->updateTrueRoots("msg", vptr, FieldType::CHAR_ARRAY);
    EXPECT_EQ(instanceGP.trueConditions.size(), 2);
}

#pragma endregion

#pragma region Same Operators Condition Test

// Check building And operator into And operator
TEST(AndInAndCondition, SameOperatorsConditionTest)
{
    int *num = new int(800);
    void *vptr = &num;
    // Creates a `FullCondition` object with the given parameters
    testCondition("&([1]&(=(status,\"high\"),=(code,500)),[2]=(code,800))", 2,
                  "code", vptr, FieldType::SIGNED_INT);

    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition))
        EXPECT_EQ(on->conditions.size(), 3);
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");

    // Check correct update
    char *str = "\"high\"";
    vptr = &str;
    num = new int(500);
    instanceGP.sensors[1]->updateTrueRoots("status", vptr,
                                           FieldType::CHAR_ARRAY);
    vptr = &num;
    instanceGP.sensors[1]->updateTrueRoots("code", vptr, FieldType::SIGNED_INT);

    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check building Or operator into Or operator
TEST(OrInOrCondition, SameOperatorsConditionTest)
{
    int *num = new int(500);
    void *vptr = &num;
    // Creates a `FullCondition` object with the given parameters
    testCondition("|([2]=(code,800),[1]|(=(status,\"high\"),=(code,500)))", 1,
                  "code", vptr, FieldType::SIGNED_INT);

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
    int *num = new int(800);
    void *vptr = &num;
    // Creates a `FullCondition` object with the given parameters
    testCondition(
        "|([2]&(=(code,800),!=(msg,\"aaa\")),[1]|(=(code,500),|(=(status,"
        "\"high\"),=(msg,\"goog\"))))",
        2, "code", vptr, FieldType::SIGNED_INT);

    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition))
        EXPECT_EQ(on->conditions.size(), 4);
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");

    // Check correct update
    char *str = "\"good\"";
    vptr = &str;
    instanceGP.sensors[2]->updateTrueRoots("msg", vptr, FieldType::CHAR_ARRAY);

    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

#pragma endregion