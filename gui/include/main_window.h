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
#include "simulation_data_manager.h"
#include "../logger/logger.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void startProcesses();
    void endProcesses();
    void showTimerInput();
    void timerTimeout();
    void openImageDialog();
    void createProcessConfigFile(int id, const QString &processPath);
    QLineEdit *getTimeInput() const { return timeInput; }
    QPushButton *getStartButton() const { return startButton; }
    QTimer *getTimer() const { return timer; }
    QTextEdit *getLogOutput() const { return logOutput; }
    QString getCurrentImagePath() const { return currentImagePath; }
    static logger guiLogger;
   public slots:
    void createNewProcess();
    void editSquare(int id);
    void deleteSquare(int id);

   private:
    friend class TestMainWindow;
    friend class DraggableSquareTest;
    friend class UserInteractionTests;
    void addProcessSquare(Process *&process);
    bool isUniqueId(int id);
    void addId(int id);
    void addProcessSquare(Process *&process, int index,
                          const QString &color = "background-color: green;");
    void compileAndRunProjects();
    QString getExecutableName(const QString &buildDirPath);
    Process *getProcessById(int id);

    QVBoxLayout *toolboxLayout;
    QWidget *workspace;
    QVector<DraggableSquare *> squares;
    QMap<int, QPoint> squarePositions;
    QSet<int> usedIds;
    QPushButton *startButton;
    QPushButton *endButton;
    QPushButton *timerButton;
    QLineEdit *timeInput;
    QLabel *timeLabel;
    QTextEdit *logOutput;
    QTimer *timer;
    QLabel *imageLabel;
    QVector<QProcess *> runningProcesses;
    QString currentImagePath;
    SimulationDataManager *dataManager;
    LogHandler logHandler;
    Frames *frames;
};

#endif  // MAIN_WINDOW_H