#ifndef CONTACTBOOK_H
#define CONTACTBOOK_H

#include "contact.h"
#include <QList>
#include <QString>
#include <QDate>
#include <QMap>

class ContactBook
{
public:
    ContactBook();

    // Основные операции
    bool addContact(Contact &contact);
    bool updateContact(const Contact &contact);
    bool deleteContact(int contactId);
    bool deleteContact(const Contact &contact);

    // Получение контактов
    QList<Contact> getAllContacts() const { return m_contacts; }
    Contact getContactById(int id) const;
    int getContactCount() const { return m_contacts.size(); }

    // Поиск
    QList<Contact> searchByName(const QString &name) const;
    QList<Contact> searchBySurname(const QString &surname) const;
    QList<Contact> searchByPhone(const QString &phone) const;
    QList<Contact> searchByEmail(const QString &email) const;

    // Сортировка
    void sortById(bool ascending = true);
    void sortBySurname(bool ascending = true);
    void sortByName(bool ascending = true);
    void sortByPatronymic(bool ascending = true);
    void sortByEmail(bool ascending = true);
    void sortByBirthDate(bool ascending = true);
    void sortByPhones(bool ascending = true);

    // Общая функция сортировки
    void sortByColumn(int column, bool ascending = true);

    // Статистика
    QMap<QString, int> getContactsByFirstLetter() const;
    QMap<int, int> getContactsByYear() const;

    // Сохранение/загрузка (для совместимости)
    bool saveToFile(const QString &filename);
    bool loadFromFile(const QString &filename);

    // Генерация следующего ID
    int getNextId() const;

    // Проверка уникальности
    bool isEmailUnique(const QString &email, int excludeId = -1) const;
    bool isPhoneUnique(const QString &phone, int excludeId = -1) const;

private:
    QList<Contact> m_contacts;

    // Вспомогательные методы
    int findContactIndexById(int id) const;
};

#endif // CONTACTBOOK_H
