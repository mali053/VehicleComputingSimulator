#include <gtest/gtest.h>
#include "full_condition.h"
#include "global_properties.h"
#include "test_helper.h"

#pragma region Same Subtrees Condition Test

// Check using the same subtree in the same tree
TEST(InSameTree, IdenticalSubtrees)
{
    int num = 800;
    FieldValue fv = num;
    testCondition(
        "|([8]&(|(=(Speed,500),<(Status,true)),=(Msg,aaa)),[8]&(|(=("
        "Speed,500),<(Status,true)),>(Msg,aaa)))",
        8, "Speed", fv, FieldType::SIGNED_INT);
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
    num = 500;
    fv = num;
    instanceGP.sensors[8]->updateTrueRoots("Speed", fv, FieldType::SIGNED_INT);
    string str = "aaa";
    fv = str;
    instanceGP.sensors[8]->updateTrueRoots("Msg", fv, FieldType::CHAR_ARRAY);
    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check using the same subtree in the different trees
TEST(InDifferentTrees, IdenticalSubtrees)
{
    int num = 800;
    FieldValue fv = num;
    // Creates a `FullCondition` objects with the given parameters
    testCondition(
        "|([8]&(|(=(Speed,500),<(Status,true)),=(Msg,aaa)),[8]&(|(=("
        "Speed,500),<(Status,true)),>(Msg,aaa)))",
        2, "Speed", fv, FieldType::SIGNED_INT);

    GlobalProperties &instanceGP = GlobalProperties::getInstance();
    vector<pair<int, string>> vec = {{2, "the condition is true"}};
    FullCondition *cond =
        new FullCondition("[8]|(=(Speed,500),<(Status,true))", vec);
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
    num = 500;
    fv = num;
    instanceGP.sensors[8]->updateTrueRoots("Speed", fv, FieldType::SIGNED_INT);
    string str = "aaa";
    fv = str;
    instanceGP.sensors[8]->updateTrueRoots("Msg", fv, FieldType::CHAR_ARRAY);
    EXPECT_EQ(instanceGP.trueConditions.size(), 2);
}

#pragma endregion

#pragma region Same Operators Condition Test

// Check building And operator into And operator
TEST(AndInAndCondition, SameOperatorsConditionTest)
{
    int num = 500;
    FieldValue fv = num;
    // Creates a `FullCondition` object with the given parameters
    testCondition("&([8]&(=(Status,true),=(Speed,500)),[8]!=(Speed,800))", 8,
                  "Speed", fv, FieldType::SIGNED_INT);

    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition))
        EXPECT_EQ(on->conditions.size(), 3);
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");

    // Check correct update
    bool status = true;
    fv = status;
    instanceGP.sensors[8]->updateTrueRoots("Status", fv,
                                           FieldType::BOOLEAN);
    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check building Or operator into Or operator
TEST(OrInOrCondition, SameOperatorsConditionTest)
{
    int num = 500;
    FieldValue fv = num;
    // Creates a `FullCondition` object with the given parameters
    testCondition("|([8]=(Speed,800),[8]|(=(Status,true),=(Speed,500)))", 8,
                  "Speed", fv, FieldType::SIGNED_INT);

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
    int num = 500;
    FieldValue fv = num;
    // Creates a `FullCondition` object with the given parameters
    testCondition(
        "|([8]&(=(Speed,800),!=(Msg,aaa)),[8]|(=(Speed,500),|(=(Status,true),=(Msg,goog))))",
        8, "Speed", fv, FieldType::SIGNED_INT);

    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Check correct building
    if (OperatorNode *on = dynamic_cast<OperatorNode *>(
            instanceGP.conditions[0]->root->firstCondition))
        EXPECT_EQ(on->conditions.size(), 4);
    else
        EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition",
                  "OperatorNode*");

    // Check correct update
    EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

#pragma endregion