#include <QtTest/QtTest>
#include "process.h"

class ProcessTests : public QObject {
    Q_OBJECT

private slots:
    void testProcessConstructor()
    {
        Process process(1, "Test Process", "TestProject", "TestPlatform");
        QCOMPARE(process.getId(), 1);
        QCOMPARE(process.getName(), QString("Test Process"));
        QCOMPARE(process.getExecutionFile(), QString("TestProject"));
        QCOMPARE(process.getQEMUPlatform(), QString("TestPlatform"));
    }

    void testDefaultConstructor()
    {
        Process process;
        QCOMPARE(process.getId(), -1);
        QCOMPARE(process.getName(), QString(""));
        QCOMPARE(process.getExecutionFile(), QString(""));
        QCOMPARE(process.getQEMUPlatform(), QString(""));
    }

    void testSetters()
    {
        Process process;
        process.setId(2);
        process.setName("New Process");
        process.setExecutionFile("NewProject");
        process.setQEMUPlatform("NewPlatform");

        QCOMPARE(process.getId(), 2);
        QCOMPARE(process.getName(), QString("New Process"));
        QCOMPARE(process.getExecutionFile(), QString("NewProject"));
        QCOMPARE(process.getQEMUPlatform(), QString("NewPlatform"));
    }
};

QTEST_MAIN(ProcessTests)
#include "test_process.moc"