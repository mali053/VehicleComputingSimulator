#include "process.h"

Process::Process(int id, const QString &name, const QString &cmakeProject, const QString &qemuPlatform)
    : id(id), name(name), cmakeProject(cmakeProject), qemuPlatform(qemuPlatform)
{
}

Process::Process()
    : id(-1), name(""), cmakeProject(""), qemuPlatform("") 
{
}

int Process::getId() const
{
    return id;
}

QString Process::getName() const
{
    return name;
}

QString Process::getCMakeProject() const
{
    return cmakeProject;
}

QString Process::getQEMUPlatform() const
{
    return qemuPlatform;
}
