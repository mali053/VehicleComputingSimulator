#ifndef PROCESS_H
#define PROCESS_H

#include <QString>

class Process {
public:
    Process(int id, const QString &name, const QString &executionFile,
            const QString &qemuPlatform);
    Process();
    Process(const Process &other);  // Copy constructor

    int getId() const;
    QString getName() const;
    QString getExecutionFile() const;
    QString getQEMUPlatform() const;
    void setId(int newId)
    {
        id = newId;
    }
    void setName(const QString &newName)
    {
        name = newName;
    }
    void setExecutionFile(const QString &newExecutionFile)
    {
        executionFile = newExecutionFile;
    }
    void setQEMUPlatform(const QString &newQEMUPlatform)
    {
        qemuPlatform = newQEMUPlatform;
    }

private:
    int id;
    QString name;
    QString executionFile;
    QString qemuPlatform;
};

#endif  // PROCESS_H