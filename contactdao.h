#ifndef CONTACTDAO_H
#define CONTACTDAO_H

#include <QObject>
#include <QList>
#include <QMap>
#include "contact.h"
#include <QSqlQuery>

class ContactDAO : public QObject
{
    Q_OBJECT

public:
    static ContactDAO& instance();

    // Основные операции CRUD
    bool addContact(Contact& contact);
    bool updateContact(const Contact& contact);
    bool deleteContact(int contactId);
    bool deleteContact(const Contact& contact);

    // Получение контактов
    Contact getContactById(int id);
    QList<Contact> getAllContacts();
    QList<Contact> getContacts(int limit = 0, int offset = 0);

    // Поиск
    QList<Contact> searchByName(const QString& name);
    QList<Contact> searchBySurname(const QString& surname);
    QList<Contact> searchByPhone(const QString& phone);
    QList<Contact> searchByEmail(const QString& email);
    QList<Contact> searchByBirthDateRange(const QDate& from, const QDate& to);

    // Статистика
    int getContactCount();
    QMap<QString, int> getContactsByFirstLetter();
    QMap<int, int> getContactsByYear();

    // Проверка уникальности
    bool isEmailUnique(const QString& email, int excludeId = -1);
    bool isPhoneUnique(const QString& phone, int excludeId = -1);

    // Пакетные операции
    bool importContacts(const QList<Contact>& contacts);
    bool exportContacts(const QString& filename, const QList<Contact>& contacts);

    // История изменений (опционально)
    void addToHistory(int contactId, const QString& fieldName,
                      const QString& oldValue, const QString& newValue);

private:
    ContactDAO(QObject* parent = nullptr);
    ~ContactDAO();

    ContactDAO(const ContactDAO&) = delete;
    ContactDAO& operator=(const ContactDAO&) = delete;

    Contact contactFromQuery(const QSqlQuery& query);
    bool savePhoneNumbers(int contactId, const QStringList& phoneNumbers);
    QStringList getPhoneNumbers(int contactId);
    bool deletePhoneNumbers(int contactId);

    // Объявление метода searchByField
    QList<Contact> searchByField(const QString& fieldName, const QString& value);
};

#endif // CONTACTDAO_H
