#include "process.h"

Process::Process(int id, const QString &name, const QString &cmakeProject,
                 const QString &qemuPlatform)
    : id(id), name(name), cmakeProject(cmakeProject), qemuPlatform(qemuPlatform)
{
}

// Copy constructor
Process::Process(const Process &other)
    : id(other.id),
      name(other.name),
      cmakeProject(other.cmakeProject),
      qemuPlatform(other.qemuPlatform)
{
}

Process::Process() : id(-1), name(""), cmakeProject(""), qemuPlatform("") {}

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
