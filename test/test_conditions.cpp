#include <gtest/gtest.h>
#include "sensor.h"
#include "full_condition.h"
#include "global_properties.h"

GlobalProperties& g_instanceGP = GlobalProperties::getInstance();

// בדיקה של יצירת תנאי והוספתו ל-GlobalProperties
TEST(ConditionTest, TestConditionCreation) {
    map<int, string> actions = { {5, "slow down"}, {8, "ccc"} };
    FullCondition cond("[5]|(=(code,500),<(status,\"high\"))", actions);
    g_instanceGP.conditions.insert({ cond.id, &cond });

    ASSERT_NE(g_instanceGP.conditions.find(cond.id), g_instanceGP.conditions.end());
}

// בדיקה של עדכון חיישן
TEST(SensorTest, TestSensorUpdate) {
    Sensor sensor(5);
    sensor.updateTrueRoots("status", "\"high\"");
    // בדוק שהסטטוס עודכן כראוי
    // צריך להוסיף את הבדיקה המדויקת המתאימה לקוד שלך
}

// בדיקה של שליחת פעולות לחיישנים
// TEST(ActionTest, TestSendActions) {
//     map<int, string> actions = { {5, "slow down"}, {8, "ccc"} };
//     g_instanceGP.sensors[5] = new Sensor(5);
//     g_instanceGP.sensors[8] = new Sensor(8);

//    sendToActions(actions);

//     // צריך להוסיף בדיקה לוודא שהפעולות אכן בוצעו
//     // לדוגמה, בדיקה אם sensor->doAction בוצע
// }

int main1(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
