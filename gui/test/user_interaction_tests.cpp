#include <QTest>
#include <QFileDialog>
#include "main_window.h"

class UserInteractionTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testOpenImageDialog();
    void testGetExecutableName();

private:
    MainWindow *mainWindow;
};

void UserInteractionTests::initTestCase()
{
    mainWindow = new MainWindow();
    mainWindow->resize(800, 600);
    mainWindow->show();
}

void UserInteractionTests::cleanupTestCase()
{
    delete mainWindow;
}

void UserInteractionTests::testOpenImageDialog()
{
    MainWindow window;
    window.openImageDialog();
    QVERIFY(
        !window.getCurrentImagePath().isEmpty());  // Ensure image path is set
}

void UserInteractionTests::testGetExecutableName()
{
    MainWindow window;
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QString buildDirPath = tempDir.path();
    QDir buildDir(buildDirPath);
    // Create an executable file for testing
    QString executableFilePath = buildDir.filePath("testExecutable");
    QFile executableFile(executableFilePath);
    QVERIFY(executableFile.open(QIODevice::WriteOnly));
    executableFile.write("dummy content");
    executableFile.close();
    QFile::setPermissions(executableFilePath, QFile::ExeUser);
    // Create a non-executable file for completeness
    QString nonExecutableFilePath = buildDir.filePath("nonExecutable.txt");
    QFile nonExecutableFile(nonExecutableFilePath);
    QVERIFY(nonExecutableFile.open(QIODevice::WriteOnly));
    nonExecutableFile.write("dummy content");
    nonExecutableFile.close();
    // Test case: The executable file should be detected
    QString executableName = window.getExecutableName(buildDirPath);
    QCOMPARE(executableName, QString("testExecutable"));
    // Test case: If the directory is empty
    QDir emptyDir(tempDir.path() + "/emptyDir");
    QVERIFY(emptyDir.mkpath("."));
    QString emptyDirExecutableName = window.getExecutableName(emptyDir.path());
    QCOMPARE(emptyDirExecutableName, QString());
    // Test case: If the directory contains only non-executable files
    QFile::remove(executableFilePath);
    QString noExecutableName = window.getExecutableName(buildDirPath);
    QCOMPARE(noExecutableName, QString());
    // Clean up
    QFile::remove(nonExecutableFilePath);
}

QTEST_MAIN(UserInteractionTests)
#include "user_interaction_tests.moc"