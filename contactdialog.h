#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include "contact.h"

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = nullptr, bool editMode = false);
    ~ContactDialog();

    void setContact(const Contact& contact);
    Contact getContact() const;

private slots:
    void onAddPhone();
    void onRemovePhone();
    void onEmailGenerated();
    void onAccept();
    void onNameChanged();

private:
    Contact currentContact;
    bool isEditMode;

    QLineEdit *nameEdit;
    QLineEdit *surnameEdit;
    QLineEdit *patronymicEdit;
    QLineEdit *emailEdit;
    QLineEdit *birthDateEdit;
    QLineEdit *phoneEdit;

    QListWidget *phoneListWidget;

    QPushButton *addPhoneButton;
    QPushButton *removePhoneButton;
    QPushButton *generateEmailButton;

    QDialogButtonBox *buttonBox;

    void setupUI();
    void setupValidators();
    void updateEmailFromName();
    bool validateForAccept();
    void validateInput();
};

#endif // CONTACTDIALOG_H
