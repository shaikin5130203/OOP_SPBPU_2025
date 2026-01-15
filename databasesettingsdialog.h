#ifndef DATABASESETTINGSDIALOG_H
#define DATABASESETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>

class DatabaseSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseSettingsDialog(QWidget *parent = nullptr);
    ~DatabaseSettingsDialog();

    QString host() const;
    int port() const;
    QString databaseName() const;
    QString userName() const;
    QString password() const;

    void setHost(const QString& host);
    void setPort(int port);
    void setDatabaseName(const QString& databaseName);
    void setUserName(const QString& userName);
    void setPassword(const QString& password);

private slots:
    void onTestConnection();
    void onSave();
    void onCancel();

private:
    // UI элементы
    QLineEdit *hostEdit;
    QSpinBox *portSpinBox;
    QLineEdit *databaseEdit;
    QLineEdit *userEdit;
    QLineEdit *passwordEdit;
    QPushButton *testButton;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QDialogButtonBox *buttonBox;

    void setupUI();
};

#endif // DATABASESETTINGSDIALOG_H
