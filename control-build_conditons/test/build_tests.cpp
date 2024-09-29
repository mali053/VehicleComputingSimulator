#include <gtest/gtest.h>
#include <QApplication>
#include <QWidget>
#include <iostream>

#include "condition.h"
#include "main_window.h"
#include "actions.h"

using namespace std;

class QtTestEnvironment : public ::testing::Environment {
public:
    QApplication *app;

    virtual void SetUp() override {
        int argc = 0;
        char **argv = nullptr;
        app = new QApplication(argc, argv);
    }

    virtual void TearDown() override {
        Output::controlLogger.cleanUp();
        delete app;
    }
};

::testing::Environment* const qt_env = ::testing::AddGlobalTestEnvironment(new QtTestEnvironment);

TEST(basicCondition, buildCondition)
{
    MainWindow *window = new MainWindow();
    Condition *condition = new Condition(window);
    condition->sensors->setCurrentIndex(2);
    condition->operators->setCurrentIndex(1);      
    condition->sensorsFields->setCurrentIndex(1);      
    condition->textBox->setText("50");
    condition->submitHandler();

    EXPECT_EQ(condition->condition.toStdString() , "[3]=(Status,50)");
}

TEST(oneOperatorDifferentSensors, buildCondition)
{
    MainWindow *window = new MainWindow();
    Condition *condition = new Condition(window);
    condition->buttonClickHandler(condition->orBtn);
    
    condition->sensors->setCurrentIndex(2);
    condition->sensorsFields->setCurrentIndex(1); 
    condition->operators->setCurrentIndex(1);     
    condition->spinBox->setValue(50);
    condition->submitHandler();

    condition->sensors->setCurrentIndex(1);
    condition->operators->setCurrentIndex(2);      
    condition->sensorsFields->setCurrentIndex(2);      
    condition->spinBox->setValue(40);

    condition->submitHandler();

    EXPECT_EQ(condition->condition.toStdString() , "|([3]=(Status,50),[4]!=(Level,40))");
}

TEST(operatorInOperator, buildCondition)
{
    MainWindow *window = new MainWindow();
    Condition *condition = new Condition(window);
    condition->buttonClickHandler(condition->andBtn);
    condition->sensors->setCurrentIndex(4);

    condition->buttonClickHandler(condition->andBtn);
   
    condition->operators->setCurrentIndex(3);      
    condition->sensorsFields->setCurrentIndex(2);      
    condition->textBox->setText("20");
    condition->submitHandler();

    condition->operators->setCurrentIndex(1);      
    condition->sensorsFields->setCurrentIndex(1);      
    condition->textBox->setText("600");
    condition->submitHandler();

    condition->skipHandler();

    condition->sensors->setCurrentIndex(3);

    condition->operators->setCurrentIndex(2);      
    condition->sensorsFields->setCurrentIndex(2);      
    condition->textBox->setText("abc");
    condition->submitHandler();

    EXPECT_EQ(condition->condition.toStdString() , "&([2]&(>(Pressure,20),=(Temperature,600)),[8]!=(Msg,abc))");
}

TEST(differentOperatorsDifferentSensors, buildCondition)
{
    MainWindow *window = new MainWindow();
    Condition *condition = new Condition(window);;

    // Start with OR button logic
    condition->buttonClickHandler(condition->orBtn);  
    
    // First condition: AND logic with sensor index 2, field 'Code', value 500
    condition->sensors->setCurrentIndex(2);  
    condition->buttonClickHandler(condition->andBtn);  
    condition->operators->setCurrentIndex(1);  // Operator '='
    condition->sensorsFields->setCurrentIndex(2);  // Field 'Code'
    condition->textBox->setText("500");
    condition->submitHandler();  // Submit the first part of the condition

    // Second part: operator '!=', field 'Flags', value '0'
    condition->operators->setCurrentIndex(2);  // Operator '!='
    condition->sensorsFields->setCurrentIndex(3);  // Field 'Flags'
    condition->textBox->setText("0");
    condition->submitHandler();  // Submit the second part

    // Skip logic here (verify the behavior of skipHandler)
    condition->skipHandler();  

    // Third condition: sensor index 4, field 'Temperature', value '25'
    condition->sensors->setCurrentIndex(4);  
    condition->operators->setCurrentIndex(5);  // Operator '<='
    condition->sensorsFields->setCurrentIndex(1);  // Field 'Temperature'
    condition->textBox->setText("25");
    condition->submitHandler();  

    // Validate the final condition string
    EXPECT_EQ(condition->condition.toStdString(), "|([3]&(=(Code,500),!=(Flags,0)),[2]<=(Temperature,25))");
}

TEST(oneOperatorOneSensor, buildCondition)
{
    MainWindow *window = new MainWindow();
    Condition *condition = new Condition(window);
    // Set the first sensor and create the condition
    condition->sensors->setCurrentIndex(1);  // Select sensor with index 1
    condition->buttonClickHandler(condition->andBtn);  // Trigger AND button
    condition->operators->setCurrentIndex(1);  // Select operator '='
    condition->sensorsFields->setCurrentIndex(1);  // Set field 'Msg'
    condition->textBox->setText("aaa");  // Set value to 'aaa'
    condition->submitHandler();  // Submit the first part of the condition
    // Set the second sensor and create the condition
    condition->operators->setCurrentIndex(4);  // Select operator '<' 
    condition->sensorsFields->setCurrentIndex(2);  // Set field 'Distance'
    condition->textBox->setText("10"); 
    condition->submitHandler();  

    // Validate that the final condition matches the expected format
    EXPECT_EQ(condition->condition.toStdString(), "[4]&(=(MessageType,aaa),<(Level,10))");
}

TEST(oneAction,buildActions)
{
    MainWindow *window = new MainWindow();
    Actions *actions = new Actions(window , "if ()");
    actions->textBox->setText("do!!");
    actions->OKBtnHandler();
    actions->sensors->setCurrentIndex(3);
    actions->addBtnHandler();
    EXPECT_EQ(actions->messages[0].first,8);
    EXPECT_EQ(actions->messages[0].second,"do!!");
}

TEST(someActions,buildActions)
{
    MainWindow *window = new MainWindow();
    Actions *actions = new Actions(window , "if ()");
    actions->textBox->setText("do!!");
    actions->OKBtnHandler();
    actions->sensors->setCurrentIndex(3);
    actions->addBtnHandler();

    actions->sensors->setCurrentIndex(1);
    actions->textBox->setText("go!!!");
    actions->OKBtnHandler();

    actions->addBtnHandler();

    EXPECT_EQ(actions->messages[0].first,8);
    EXPECT_EQ(actions->messages[0].second,"do!!");
    EXPECT_EQ(actions->messages[1].first,4);
    EXPECT_EQ(actions->messages[1].second,"go!!!");
}

int main(int argc, char **argv)
{
    Input &input = Input::getInstance();
    input.setPathToSensors("../test/sensors.json");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
