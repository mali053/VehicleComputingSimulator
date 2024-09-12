#include <QtTest/QtTest>
#include "../src/main_window.h"

class TestMainWindow : public QObject {
    Q_OBJECT

private slots:

    void testCreateNewProcess();
    void testAddProcessSquare();
    void testIsUniqueId();
    void testStartProcesses();
    void testEndProcesses();
    void testStopProcesses();
    void testDeleteSquare();
    void testShowTimerInput();
    void addId(int id);
};

void TestMainWindow::testCreateNewProcess()
{
    MainWindow window;
    int newProcessId = 6; // Ensure this is greater than 5 and unique
    QString processName = "NewProcess";
    QString cmakeProject = "../src/dummy_program3";
    QString qemuPlatform = "QEMUPlatform";
    // Create a new Process object with the provided data
    Process *newProcess =
        new Process(newProcessId, processName, cmakeProject, qemuPlatform);
    // Simulate adding the process square to the main window
    window.addProcessSquare(newProcess);
    window.addId(newProcessId);
    // Verify that the process was added correctly
    Process *retrievedProcess = window.getProcessById(newProcessId);
    QVERIFY(retrievedProcess != nullptr);
    QCOMPARE(retrievedProcess->getName(), processName);
    QCOMPARE(retrievedProcess->getCMakeProject(), cmakeProject);
    QCOMPARE(retrievedProcess->getQEMUPlatform(), qemuPlatform);
    // Clean up
    delete newProcess;
}

void TestMainWindow::testAddProcessSquare()
{
    MainWindow window;
    Process *newProcess =
        new Process(5, "Test Process", "../src/dummy_program1", "QEMUPlatform");
    window.addProcessSquare(newProcess);
    QCOMPARE(window.squares.size(), 5);  // Check if square is added
}

void TestMainWindow::testIsUniqueId()
{
    MainWindow window;
    window.addId(5);
    QCOMPARE(window.isUniqueId(5), false);  // Check if the ID is unique
    QCOMPARE(window.isUniqueId(10), true);  // Check if a different ID is unique
}

void TestMainWindow::testStartProcesses()
{
    MainWindow window;
    window.startProcesses();
    QVERIFY(
        !window.runningProcesses.isEmpty());  // Ensure processes are started
}

void TestMainWindow::testEndProcesses()
{
    MainWindow window;
    window.startProcesses();
    window.endProcesses();
    QVERIFY(window.runningProcesses.isEmpty());  // Ensure processes are stopped
}

void TestMainWindow::testStopProcesses()
{
    MainWindow window;
    QProcess* testProcess = new QProcess();
    int testId = 6;

    window.runningProcesses.append(QPair<QProcess*, int>(testProcess, testId));

    window.stopProcess(testId);

    QCOMPARE(window.runningProcesses.size(), 0); // Ensure the process is removed from the list
    QCOMPARE(testProcess->state(), QProcess::NotRunning); // Verify that the process is not running
}

void TestMainWindow::testDeleteSquare()
{
    MainWindow window;
    Process *process =
        new Process(5, "Test Process", "../src/dummy_program1", "QEMUPlatform");
    window.addProcessSquare(process);

    window.deleteSquare(5);

    // Ensure that only the initial 4 squares remain
    QCOMPARE(window.squares.size(), 4);
    QVERIFY(!window.squarePositions.contains(5));
    QVERIFY(!window.usedIds.contains(5));
}

void TestMainWindow::addId(int id)
{
    MainWindow window;
    window.addId(77);
    QVERIFY(window.usedIds.contains(77));
}

void TestMainWindow::testShowTimerInput()
{
    MainWindow mainWindow;

    // Initially, the time input and label should be hidden
    QVERIFY(!mainWindow.timeInput->isVisible());
    QVERIFY(!mainWindow.timeLabel->isVisible());

    mainWindow.show();
    // Trigger the showTimerInput function
    mainWindow.showTimerInput();

    // Now, the time input and label should be visible
    QVERIFY(mainWindow.timeInput->isVisible());
    QVERIFY(mainWindow.timeLabel->isVisible());
}

QTEST_MAIN(TestMainWindow)
#include "test_main_window.moc"