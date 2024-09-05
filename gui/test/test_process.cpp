#include <QtTest/QtTest>
#include "../src/process.h"

class ProcessTests : public QObject {
    Q_OBJECT

private slots:
    void testProcessConstructor() {
        Process process(1, "Test Process", "TestProject", "TestPlatform");
        QCOMPARE(process.getId(), 1);
        QCOMPARE(process.getName(), QString("Test Process"));
        QCOMPARE(process.getCMakeProject(), QString("TestProject"));
        QCOMPARE(process.getQEMUPlatform(), QString("TestPlatform"));
    }

    void testDefaultConstructor() {
        Process process;
        QCOMPARE(process.getId(), -1);
        QCOMPARE(process.getName(), QString(""));
        QCOMPARE(process.getCMakeProject(), QString(""));
        QCOMPARE(process.getQEMUPlatform(), QString(""));
    }

    void testSetters() {
        Process process;
        process.setId(2);
        process.setName("New Process");
        process.setCMakeProject("NewProject");
        process.setQEMUPlatform("NewPlatform");

        QCOMPARE(process.getId(), 2);
        QCOMPARE(process.getName(), QString("New Process"));
        QCOMPARE(process.getCMakeProject(), QString("NewProject"));
        QCOMPARE(process.getQEMUPlatform(), QString("NewPlatform"));
    }
};

QTEST_MAIN(ProcessTests)
#include "test_process.moc"
