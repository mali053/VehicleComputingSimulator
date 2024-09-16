#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QMap>

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
    void setCMakeProject(const QString &cmakeProject);
    
    enum KeyPermission {
        VERIFY,
        SIGN,
        ENCRYPT,
        DECRYPT,
        EXPORTABLE
    };

    QMap<KeyPermission, bool> getKeyPermissions() const;
    void setKeyPermissions(const QMap<KeyPermission, bool> &permissions);

private slots:
    bool validateAndAccept();
    void selectExecutableFile();

private:
    void setupPermissionCheckboxes();

    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QLineEdit *executionFile;
    QComboBox *qemuPlatformCombo;
    QMap<KeyPermission, QCheckBox*> permissionCheckboxes;
    QLineEdit *cmakeProjectEdit;            
    QPushButton *selectExecutableFileButton; 

    friend class ProcessDialogTests;
};

#endif  // PROCESSDIALOG_H