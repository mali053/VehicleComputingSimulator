#include <QtTest/QtTest>
#include "../src/process_dialog.h"

class ProcessDialogTests : public QObject {
    Q_OBJECT

private slots:
    void testGetId() {
        ProcessDialog dialog;
        dialog.setId(123);
        QCOMPARE(dialog.getId(), 123);
    }

    void testGetName() {
        ProcessDialog dialog;
        dialog.setName("Test Process");
        QCOMPARE(dialog.getName(), "Test Process");
    }

    void testGetCMakeProject() {
        ProcessDialog dialog;
        dialog.setCMakeProject("TestProject");
        QCOMPARE(dialog.getCMakeProject(), "TestProject");
    }

    void testGetQEMUPlatform() {
        ProcessDialog dialog;
        dialog.setQEMUPlatform("arm");
        QCOMPARE(dialog.getQEMUPlatform(), "arm");
    }

    void testIsValid() {
        ProcessDialog dialog;
        dialog.setId(123);
        dialog.setName("Test Process");
        dialog.setCMakeProject("TestProject");
        dialog.setQEMUPlatform("x86");
        QVERIFY(dialog.isValid());
    }

    void testValidateAndAccept() {
        ProcessDialog dialog;
        dialog.setId(123);
        dialog.setName("Test Process");
        dialog.setCMakeProject("TestProject");
        dialog.setQEMUPlatform("x86");
        QVERIFY(dialog.validateAndAccept() == true); // Assuming true indicates validation success
    }
};

// הוסף את המאקרו QTEST_MAIN כדי ליצור את פונקציית main
QTEST_MAIN(ProcessDialogTests)

// הקובץ שכולל את Q_OBJECT צריך להיות מחובר למאקרו
#include "test_process_dialog.moc"
