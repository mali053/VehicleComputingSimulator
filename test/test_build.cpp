#include <gtest/gtest.h>
#include "global_properties.h"
#include "test_helper.h"
#include "full_condition.h"

// // Test with setup and teardown
// class EqualCondition : public ::testing::Test {
// protected:
//    void SetUp() override {
//       cout << "SetUp()" << endl;
//    }

//    void TearDown() override {
//       cout << "TearDown()" << endl;
//    }
// };




// Check build sensors
TEST(SensorsBuild, SensorsTest) {

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.sensors.size(), 2);
}

#pragma region Basic Condition Test

// Insert BasicCondition into instanceGP.conditions
TEST(EqualCondition, BasicConditionTest) {

   testCondition("[5]=(code,500)", 5, "code", "500");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.conditions.size(), 1);
}

// Check the update status
TEST(SimpleUpdateStatus, UpdateTest) {

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check the update status with BasicConditionNotEqual
TEST(NotEqualCondition, BasicConditionTest) {

   testCondition("[5]!=(code,500)", 5, "code", "800");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check the update status with BasicConditionLetter
TEST(LetterCondition, BasicConditionTest) {

   testCondition("[5]<(code,500)", 5, "code", "500");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.trueConditions.size(), 0);
}

// Check the update status with BasicConditionBigger
TEST(BiggerCondition, BasicConditionTest) {

   testCondition("[5]>(code,500)", 5, "code", "800");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

#pragma endregion

#pragma region Complex Condition Test

// Check building Or operator
TEST(OrCondition, ComplexConditionTest) {
   
   testCondition("[5]|(=(code,500),=(msg,aaa))", 5, "code", "500");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.conditions.size(), 1);
   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check building And operator
TEST(AndCondition, ComplexConditionTest) {

   testCondition("[5]&(=(code,500),=(msg,\"aaa\"))", 5, "code", "500");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.conditions.size(), 1);
   EXPECT_EQ(instanceGP.trueConditions.size(), 0);
}

// Check building And operator
TEST(OrAndCondition, ComplexConditionTest) {

   testCondition("|([5]&(=(status,\"high\"),=(code,500),=(msg,\"aaa\")),[8]=(code,800))", 8, "code", "800");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

#pragma endregion

#pragma region Same Operators Condition Test

// Check building And operator into And operator
TEST(AndInAndCondition, SameOperatorsConditionTest) {
   
   testCondition("&([5]&(=(status,\"high\"),=(code,500)),[8]=(code,800))", 8, "code", "800");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   // Check correct building
   if(OperatorNode* on = dynamic_cast<OperatorNode*>(instanceGP.conditions[0]->root->firstCondition))
      EXPECT_EQ(on->conditions.size(), 3);
   else
      EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition", "OperatorNode*");

   // Check correct update
   instanceGP.sensors[5]->updateTrueRoots("status", "\"high\"");
   instanceGP.sensors[5]->updateTrueRoots("code", "500");

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check building Or operator into Or operator
TEST(OrInOrCondition, SameOperatorsConditionTest) {
   
   testCondition("|([8]=(code,800),[5]|(=(status,\"high\"),=(code,500)))", 5, "code", "500");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   // Check correct building
   if(OperatorNode* on = dynamic_cast<OperatorNode*>(instanceGP.conditions[0]->root->firstCondition))
      EXPECT_EQ(on->conditions.size(), 3);
   else
      EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition", "OperatorNode*");
   
   // Check correct update
   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

// Check building Complex Or operator into Or operator
TEST(ComplexOrInOrCondition, SameOperatorsConditionTest) {
   
   testCondition("|([8]&(=(code,800),!=(msg,\"aaa\")),[5]|(=(code,500),|(=(status,\"high\"),=(msg,\"goog\"))))", 8, "code", "800");

   GlobalProperties& instanceGP = GlobalProperties::getInstance();

   // Check correct building
   if(OperatorNode* on = dynamic_cast<OperatorNode*>(instanceGP.conditions[0]->root->firstCondition))
      EXPECT_EQ(on->conditions.size(), 4);
   else
      EXPECT_EQ("instanceGP.conditions[0]->root->firstCondition", "OperatorNode*");

   // Check correct update
   instanceGP.sensors[8]->updateTrueRoots("msg", "\"good\"");

   EXPECT_EQ(instanceGP.trueConditions.size(), 1);
}

#pragma endregion



int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}