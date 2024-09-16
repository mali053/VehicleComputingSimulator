#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDebug>
#include <vector>
#include <QSet>
#include <QMap>
#include <QWidget>
#include <QProcess>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMap>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsRotation>
#include <QTransform>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QVector>
#include <memory>
#include "process.h"
#include "draggable_square.h"
#include "frames.h"
#include "log_handler.h"
#include "process.h"
#include "process_dialog.h"
#include "simulation_state_manager.h"
#include "../../logger/logger.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateTimer();
    void endProcesses();
    void stopProcess(int deleteId);
    void showTimerInput();
    void timerTimeout();
    void openImageDialog();
    void createProcessConfigFile(int id, const QString &processPath);
    void disableButtonsExceptEnd();  // Disable all buttons except the "End" button
    void enableAllButtons();         // Re-enable all buttons
    void showLoadingIndicator();     // Show loading animation (spinner)
    void hideLoadingIndicator();     // Hide loading animation (spinner)
  
    QLineEdit *getTimeInput() const
    {
        return timeInput;
    }
    QPushButton *getStartButton() const
    {
        return runButton;
    }
    QTimer *getTimer() const
    {
        return timer;
    }
    QTextEdit *getLogOutput() const
    {
        return logOutput;
    }
    QString getCurrentImagePath() const
    {
        return currentImagePath;
    }
    static logger guiLogger;

private slots:
    void showSimulation();
    void loadSimulation();

public slots:
    void createNewProcess();
    void editSquare(int id);
    void deleteSquare(int id);

private:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    friend class TestMainWindow;
    friend class DraggableSquareTest;
    friend class UserInteractionTests;
    void addProcessSquare(Process *&process);
    bool isUniqueId(int id);
    void addId(int id);
    void addProcessSquare(Process *process, QPoint position, int width,
                          int height, const QString &color);
    void compileProjects();
    void runProjects();
    QString getExecutableName(const QString &buildDirPath);
    Process *getProcessById(int id);
    void rotateImage();     // Function to handle rotation
    void openSecondProject();  // Function that handles launching the second project
    QPushButton *openSecondProjectButton;  // Button to open the second project
    QVBoxLayout *toolboxLayout;
    QWidget *workspace;
    QVector<DraggableSquare *> squares;
    QMap<int, QPoint> squarePositions;
    QSet<int> usedIds;
    QPushButton *compileButton;
    QPushButton *runButton;
    QPushButton *endButton;
    QPushButton *timerButton;
    QLineEdit *timeInput;
    QLabel *timeLabel;
    QTextEdit *logOutput;
    QTimer *timer;
    QLabel *imageLabel;
    QVector<QPair<QProcess *, int>> runningProcesses;
    QLabel *loadingLabel;
    QPixmap loadingPixmap;  // Store the original pixmap
    QTimer *rotationTimer;  // Timer for rotation
    int rotationAngle;      // Store the current rotation angle
    QString currentImagePath;
    SimulationStateManager *stateManager;
    LogHandler logHandler;
};

#endif  // MAIN_WINDOW_H