#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>
#include <vector>
#include <QSet>
#include <QMap>
#include <QGridLayout>
#include <QWidget>
#include <QProcess>
#include <QDir>
#include <QTextEdit>
#include <QLineEdit>
#include <QTimer>
#include <QLabel>
#include <QFileDialog>
#include <QPixmap>
#include "process.h"
#include "draggable_square.h"
#include "process_dialog.h"

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
    void readProcess1Output();
    void readProcess2Output();
    void openImageDialog();
    
private slots:
    void createNewProcess();

private:
    void addProcessSquare(const Process &process);
    bool isUniqueId(int id);
    void addId(int id);
    void addProcessSquare(const Process& process, int index);
    void compileBoxes();
    QString getExecutableName(const QString &buildDirPath);
    QVBoxLayout *toolboxLayout;
    QWidget *workspace;
    std::vector<DraggableSquare*> squares;
    QMap<int, QPoint> squarePositions; 
    QSet<int> usedIds;
    QSize originalSize;
    QPushButton *startButton;
    QPushButton *endButton;
    QPushButton *timerButton;
    QLineEdit *timeInput;
    QLabel *timeLabel;
    QTextEdit *logOutput;
    QProcess *process1;
    QProcess *process2;
    QTimer *timer;
    QLabel *imageLabel;
};

#endif // MAIN_WINDOW_H
