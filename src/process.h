#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

class Process
{
public:
    Process(int id, const QString &name, const QString &cmakeProject, const QString &qemuPlatform);
    Process();

    int getId() const;
    QString getName() const;
    QString getCMakeProject() const;
    QString getQEMUPlatform() const;

private:
    int id;
    QString name;
    QString cmakeProject;
    QString qemuPlatform;
};

#endif // PROCESS_H
