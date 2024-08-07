#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>

class ProcessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget *parent = nullptr);

    int getId() const;
    QString getName() const;
    QString getCMakeProject() const;
    QString getQEMUPlatform() const;
    bool isValid() const;
    void setId(int id);
    void setName(const QString &name);
    void setCMakeProject(const QString &cmakeProject);
    void setQEMUPlatform(const QString &qemuPlatform);

private slots:
    void validateAndAccept();

private:
    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QLineEdit *cmakeProjectEdit;
    // QLineEdit *qemuPlatformEdit;
    QComboBox *qemuPlatformCombo;
};

#endif // PROCESSDIALOG_H
