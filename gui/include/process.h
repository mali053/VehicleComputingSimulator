#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

class Process {
public:
    Process(int id, const QString &name, const QString &cmakeProject,
            const QString &qemuPlatform);
    Process();
    Process(const Process &other);  // Copy constructor

    int getId() const;
    QString getName() const;
    QString getCMakeProject() const;
    QString getQEMUPlatform() const;
    void setId(int newId)
    {
        id = newId;
    }
    void setName(const QString &newName)
    {
        name = newName;
    }
    void setCMakeProject(const QString &newCMakeProject)
    {
        cmakeProject = newCMakeProject;
    }
    void setQEMUPlatform(const QString &newQEMUPlatform)
    {
        qemuPlatform = newQEMUPlatform;
    }

private:
    int id;
    QString name;
    QString cmakeProject;
    QString qemuPlatform;
};

#endif  // PROCESS_H