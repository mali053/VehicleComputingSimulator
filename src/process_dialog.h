#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>

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

private slots:
    void validateAndAccept();

private:
    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QLineEdit *cmakeProjectEdit;
    QLineEdit *qemuPlatformEdit;
};

#endif // PROCESSDIALOG_H
