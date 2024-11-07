#ifndef COMPILER_H
#define COMPILER_H

#include <QThread>
#include <QFile>
#include <QProcess>
#include <QDir>
#include <QStringList>
#include <QString>

class Compiler : public QThread {
    Q_OBJECT
public:
    Compiler(QString cmakePath, bool *compileSuccessful,
             QObject *parent = nullptr);

protected:
    void run() override;

signals:
    void logMessage(const QString &message);

private:
    QString cmakePath;
    bool *compileSuccessful;
};

#endif  // COMPILER_H