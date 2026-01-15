#ifndef CONTACT_H
#define CONTACT_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QVariant>

class Contact
{
public:
    Contact();
    Contact(const QString &name, const QString &surname,
            const QString &patronymic, const QString &email,
            const QDate &birthDate, const QStringList &phones = QStringList());

    // Геттеры
    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getSurname() const { return m_surname; }
    QString getPatronymic() const { return m_patronymic; }
    QString getEmail() const { return m_email; }
    QDate getBirthDate() const { return m_birthDate; }
    QString getBirthDateString() const { return m_birthDate.toString("dd.MM.yyyy"); }
    QStringList getPhones() const { return m_phones; }
    QString getFullName() const { return m_surname + " " + m_name + " " + m_patronymic; }

    // Сеттеры
    void setId(int id) { m_id = id; }
    bool setName(const QString &name);
    bool setSurname(const QString &surname);
    bool setPatronymic(const QString &patronymic);
    bool setEmail(const QString &email);
    bool setBirthDate(const QDate &date);
    bool setBirthDate(const QString &date);
    void setPhones(const QStringList &phones) { m_phones = phones; }

    // Добавление/удаление телефона
    bool addPhone(const QString &phone);
    void removePhone(int index);
    void clearPhones() { m_phones.clear(); }

    // Операторы сравнения для сортировки
    bool operator<(const Contact& other) const;
    bool operator>(const Contact& other) const;

    // Методы для разных типов сортировки
    static bool compareById(const Contact& a, const Contact& b, bool ascending);
    static bool compareBySurname(const Contact& a, const Contact& b, bool ascending);
    static bool compareByName(const Contact& a, const Contact& b, bool ascending);
    static bool compareByPatronymic(const Contact& a, const Contact& b, bool ascending);
    static bool compareByEmail(const Contact& a, const Contact& b, bool ascending);
    static bool compareByBirthDate(const Contact& a, const Contact& b, bool ascending);
    static bool compareByPhones(const Contact& a, const Contact& b, bool ascending);

private:
    int m_id;
    QString m_name;
    QString m_surname;
    QString m_patronymic;
    QString m_email;
    QDate m_birthDate;
    QStringList m_phones;
};

#endif // CONTACT_H
