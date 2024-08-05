#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <vector>
#include <QSet>
#include <QMap>
#include <QWidget>
#include <QProcess>
#include <QDir>
#include <QTextEdit>
#include <QLineEdit>
#include <QTimer>
#include <QLabel>
#include <QFileDialog>
#include <QPixmap>
#include <QVector>
#include <memory>

#include "process.h"
#include "draggable_square.h"
#include "process_dialog.h"
#include "simulation_data_manager.h"
#include "log_handler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void startProcesses();
    void endProcesses();
    void showTimerInput();
    void timerTimeout();
    void openImageDialog();
    
private slots:
    void createNewProcess();

public slots:
    void editSquare(int id);
    void deleteSquare(int id);

private:
    void addProcessSquare(const Process &process);
    bool isUniqueId(int id);
    void addId(int id);
    void addProcessSquare(const Process &process, int index, const QString &color = "background-color: green;");
    void compileBoxes();
    QString getExecutableName(const QString &buildDirPath);

    QVBoxLayout *toolboxLayout;
    QWidget *workspace;
    QVector<DraggableSquare*> squares;
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
    QVector<QProcess*> runningProcesses;
    QString currentImagePath;
    SimulationDataManager *dataManager;
    LogHandler logHandler;
};

#endif // MAIN_WINDOW_H
