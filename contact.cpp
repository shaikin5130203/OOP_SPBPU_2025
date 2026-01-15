#include "contact.h"
#include "checker.h"
#include <QMessageBox>

Contact::Contact()
    : m_id(-1) // -1 означает, что контакт еще не сохранен в БД
{}

Contact::Contact(const QString &name, const QString &surname,
                 const QString &patronymic, const QString &email,
                 const QDate &birthDate, const QStringList &phones)
    : m_id(-1), m_name(name), m_surname(surname), m_patronymic(patronymic),
      m_email(email), m_birthDate(birthDate), m_phones(phones)
{}

bool Contact::setName(const QString &name)
{
    if (!Checker::validateName(name)) {
        return false;
    }
    m_name = Checker::formatName(name);
    return true;
}

bool Contact::setSurname(const QString &surname)
{
    if (!Checker::validateSurname(surname)) {
        return false;
    }
    m_surname = Checker::formatSurname(surname);
    return true;
}

bool Contact::setPatronymic(const QString &patronymic)
{
    if (!Checker::validatePatronymic(patronymic)) {
        return false;
    }
    m_patronymic = Checker::formatPatronymic(patronymic);
    return true;
}

bool Contact::setEmail(const QString &email)
{
    if (!Checker::validateEmail(email)) {
        return false;
    }
    m_email = Checker::formatEmail(email);
    return true;
}

bool Contact::setBirthDate(const QDate &date)
{
    if (!date.isValid() || date > QDate::currentDate()) {
        return false;
    }
    m_birthDate = date;
    return true;
}

bool Contact::setBirthDate(const QString &date)
{
    if (!Checker::validateDate(date)) {
        return false;
    }

    QStringList parts = date.split('.');
    if (parts.size() != 3) return false;

    int day = parts[0].toInt();
    int month = parts[1].toInt();
    int year = parts[2].toInt();

    QDate d(year, month, day);
    if (!d.isValid()) return false;

    m_birthDate = d;
    return true;
}

bool Contact::addPhone(const QString &phone)
{
    if (!Checker::validatePhone(phone)) {
        return false;
    }

    QString formattedPhone = Checker::formatPhone(phone);
    if (m_phones.contains(formattedPhone)) {
        return false;
    }

    m_phones.append(formattedPhone);
    return true;
}

void Contact::removePhone(int index)
{
    if (index >= 0 && index < m_phones.size()) {
        m_phones.removeAt(index);
    }
}

bool Contact::operator<(const Contact& other) const
{
    return m_id < other.m_id;
}

bool Contact::operator>(const Contact& other) const
{
    return m_id > other.m_id;
}

bool Contact::compareById(const Contact& a, const Contact& b, bool ascending)
{
    if (ascending) {
        return a.m_id < b.m_id;
    } else {
        return a.m_id > b.m_id;
    }
}

bool Contact::compareBySurname(const Contact& a, const Contact& b, bool ascending)
{
    int cmp = QString::compare(a.m_surname, b.m_surname, Qt::CaseInsensitive);
    if (cmp == 0) {
        // Если фамилии одинаковые, сортируем по имени
        cmp = QString::compare(a.m_name, b.m_name, Qt::CaseInsensitive);
        if (cmp == 0) {
            // Если имена одинаковые, сортируем по ID
            return Contact::compareById(a, b, ascending);
        }
    }

    if (ascending) {
        return cmp < 0;
    } else {
        return cmp > 0;
    }
}

bool Contact::compareByName(const Contact& a, const Contact& b, bool ascending)
{
    int cmp = QString::compare(a.m_name, b.m_name, Qt::CaseInsensitive);
    if (cmp == 0) {
        // Если имена одинаковые, сортируем по фамилии
        cmp = QString::compare(a.m_surname, b.m_surname, Qt::CaseInsensitive);
        if (cmp == 0) {
            return Contact::compareById(a, b, ascending);
        }
    }

    if (ascending) {
        return cmp < 0;
    } else {
        return cmp > 0;
    }
}

bool Contact::compareByPatronymic(const Contact& a, const Contact& b, bool ascending)
{
    int cmp = QString::compare(a.m_patronymic, b.m_patronymic, Qt::CaseInsensitive);
    if (cmp == 0) {
        return Contact::compareBySurname(a, b, ascending);
    }

    if (ascending) {
        return cmp < 0;
    } else {
        return cmp > 0;
    }
}

bool Contact::compareByEmail(const Contact& a, const Contact& b, bool ascending)
{
    int cmp = QString::compare(a.m_email, b.m_email, Qt::CaseInsensitive);
    if (cmp == 0) {
        return Contact::compareById(a, b, ascending);
    }

    if (ascending) {
        return cmp < 0;
    } else {
        return cmp > 0;
    }
}

bool Contact::compareByBirthDate(const Contact& a, const Contact& b, bool ascending)
{
    if (a.m_birthDate == b.m_birthDate) {
        return Contact::compareBySurname(a, b, ascending);
    }

    if (ascending) {
        return a.m_birthDate < b.m_birthDate;
    } else {
        return a.m_birthDate > b.m_birthDate;
    }
}

bool Contact::compareByPhones(const Contact& a, const Contact& b, bool ascending)
{
    QString phoneA = a.m_phones.isEmpty() ? "" : a.m_phones.first();
    QString phoneB = b.m_phones.isEmpty() ? "" : b.m_phones.first();

    int cmp = QString::compare(phoneA, phoneB, Qt::CaseInsensitive);
    if (cmp == 0) {
        return Contact::compareBySurname(a, b, ascending);
    }

    if (ascending) {
        return cmp < 0;
    } else {
        return cmp > 0;
    }
}
