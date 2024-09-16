#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class ProcessDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget *parent = nullptr);

    int getId() const;
    QString getName() const;
    QString getExecutionFile() const;
    QString getQEMUPlatform() const;
    bool isValid() const;
    void setId(int id);
    void setName(const QString &name);
    void setExecutionFile(const QString &executableFile);
    void setQEMUPlatform(const QString &qemuPlatform);
    void selectExecutableFile();

private slots:
    bool validateAndAccept();
    friend class ProcessDialogTests;

private:
    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QLineEdit *executionFile;
    QComboBox *qemuPlatformCombo;
};

#endif  // PROCESSDIALOG_H