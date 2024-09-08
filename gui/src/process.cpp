#include "process.h"
#include "main_window.h"

Process::Process(int id, const QString &name, const QString &cmakeProject,
                 const QString &qemuPlatform)
    : id(id), name(name), cmakeProject(cmakeProject), qemuPlatform(qemuPlatform)
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Process created with ID: " + std::to_string(id));
}

// Copy constructor
Process::Process(const Process &other)
    : id(other.id),
      name(other.name),
      cmakeProject(other.cmakeProject),
      qemuPlatform(other.qemuPlatform)
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Process copied with ID: " + std::to_string(other.id));
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
