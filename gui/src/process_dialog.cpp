#include <QComboBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "process_dialog.h"
#include "main_window.h"

ProcessDialog::ProcessDialog(QWidget *parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Initializing ProcessDialog");

    QLabel *idLabel = new QLabel("ID:");
    idEdit = new QLineEdit(this);
    idEdit->setValidator(new QIntValidator(11, 10000, this));
    layout->addWidget(idLabel);
    layout->addWidget(idEdit);

    QLabel *nameLabel = new QLabel("Name:");
    nameEdit = new QLineEdit(this);
    layout->addWidget(nameLabel);
    layout->addWidget(nameEdit);

    QLabel *executableFileLabel = new QLabel("Executable File:");
    executionFile = new QLineEdit(this);
    executionFile->setReadOnly(true);
    layout->addWidget(executableFileLabel);
    layout->addWidget(executionFile);

    QPushButton *selectExecutableFileButton = new QPushButton("Select File", this);
    layout->addWidget(selectExecutableFileButton);

    QLabel *qemuPlatformLabel = new QLabel("QEMU Platform:");
    qemuPlatformCombo = new QComboBox(this);
    qemuPlatformCombo->addItems({"x86_64", "arm", "aarch64"});
    qemuPlatformCombo->setEnabled(false);

    // Adding widgets to layout
    layout->addWidget(qemuPlatformLabel);
    layout->addWidget(qemuPlatformCombo);

    // Currently, hardware selection is not needed, so the widgets are hidden.
    // If you need to use this in the future, remove the following lines:
    qemuPlatformCombo->setVisible(false);
    qemuPlatformLabel->setVisible(false);

    QRegExp regex("[a-zA-Z0-9]*");  // Allows only English letters and numbers
    QRegExpValidator *validator = new QRegExpValidator(regex, this);

    nameEdit->setValidator(validator);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this,
            &ProcessDialog::validateAndAccept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    connect(selectExecutableFileButton, &QPushButton::clicked, this, &ProcessDialog::selectExecutableFile); // Changed the connection for file selection

    setLayout(layout);
}

void ProcessDialog::selectExecutableFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Executable File", "", 
                                                    "Executable Files (*.sh *.cmake);;Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        executionFile->setText(fileName);  // Update the renamed QLineEdit
    }
}


int ProcessDialog::getId() const
{
    return idEdit->text().toInt();
}

QString ProcessDialog::getName() const
{
    return nameEdit->text();
}

QString ProcessDialog::getExecutionFile() const
{
    return executionFile->text();
}

QString ProcessDialog::getQEMUPlatform() const
{
    return qemuPlatformCombo->currentText();
}

bool ProcessDialog::isValid() const
{
    if (!idEdit || !nameEdit || !executionFile || !qemuPlatformCombo) {
        return false;
    }
    return !idEdit->text().isEmpty() && !nameEdit->text().isEmpty() &&
           !executionFile->text().isEmpty() &&
           !qemuPlatformCombo->currentText().isEmpty();
}

bool ProcessDialog::validateAndAccept()
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Validating ProcessDialog inputs");

    if (isValid()) {
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::INFO, "Validation successful, accepting dialog");
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::DEBUG,
            "Entered values: ID = " + idEdit->text().toStdString() +
                ", Name = " + nameEdit->text().toStdString() +
                ", Executable File = " + executionFile->text().toStdString() +
                ", QEMU Platform = " +
                qemuPlatformCombo->currentText().toStdString());
        accept();
        return true;
    }
    else {
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::ERROR,
            "Validation failed, missing or incorrect input");
        QMessageBox::warning(this, "Input Error",
                             "Please fill in all fields correctly.");
        return false;
    }
}
void ProcessDialog::setId(int id)
{
    idEdit->setText(QString::number(id));
}

void ProcessDialog::setName(const QString &name)
{
    nameEdit->setText(name);
}

void ProcessDialog::setExecutionFile(const QString &executableFile)
{
    executionFile->setText(executableFile);
}

void ProcessDialog::setQEMUPlatform(const QString &qemuPlatform)
{
    int index = qemuPlatformCombo->findText(qemuPlatform);
    if (index != -1) {
        qemuPlatformCombo->setCurrentIndex(index);
    }
}