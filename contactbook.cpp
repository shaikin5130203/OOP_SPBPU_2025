#include "contactbook.h"
#include "checker.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <algorithm>

ContactBook::ContactBook() {}

bool ContactBook::addContact(Contact &contact)
{
    // Генерируем ID для нового контакта
    int newId = getNextId();
    contact.setId(newId);

    // Проверяем уникальность email
    if (!isEmailUnique(contact.getEmail())) {
        return false;
    }

    // Проверяем уникальность всех телефонов
    QStringList phones = contact.getPhones();
    for (const QString &phone : phones) {
        if (!isPhoneUnique(phone)) {
            return false;
        }
    }

    m_contacts.append(contact);
    return true;
}

bool ContactBook::updateContact(const Contact &contact)
{
    int index = findContactIndexById(contact.getId());
    if (index == -1) {
        return false;
    }

    // Проверяем уникальность email (кроме текущего контакта)
    if (!isEmailUnique(contact.getEmail(), contact.getId())) {
        return false;
    }

    // Проверяем уникальность телефонов (кроме текущего контакта)
    QStringList phones = contact.getPhones();
    for (const QString &phone : phones) {
        if (!isPhoneUnique(Checker::normalizePhone(phone), contact.getId())) {
            return false;
        }
    }

    m_contacts[index] = contact;
    return true;
}

bool ContactBook::deleteContact(int contactId)
{
    int index = findContactIndexById(contactId);
    if (index == -1) {
        return false;
    }

    m_contacts.removeAt(index);
    return true;
}

bool ContactBook::deleteContact(const Contact &contact)
{
    return deleteContact(contact.getId());
}

Contact ContactBook::getContactById(int id) const
{
    int index = findContactIndexById(id);
    if (index == -1) {
        return Contact();
    }
    return m_contacts[index];
}

QList<Contact> ContactBook::searchByName(const QString &name) const
{
    QList<Contact> results;
    QString searchName = name.toLower();

    for (const Contact &contact : m_contacts) {
        if (contact.getName().toLower().contains(searchName)) {
            results.append(contact);
        }
    }

    return results;
}

QList<Contact> ContactBook::searchBySurname(const QString &surname) const
{
    QList<Contact> results;
    QString searchSurname = surname.toLower();

    for (const Contact &contact : m_contacts) {
        if (contact.getSurname().toLower().contains(searchSurname)) {
            results.append(contact);
        }
    }

    return results;
}

QList<Contact> ContactBook::searchByPhone(const QString &phone) const
{
    QList<Contact> results;
    QString searchPhone = phone.toLower();

    for (const Contact &contact : m_contacts) {
        for (const QString &contactPhone : contact.getPhones()) {
            if (contactPhone.contains(searchPhone, Qt::CaseInsensitive)) {
                results.append(contact);
                break;
            }
        }
    }

    return results;
}

QList<Contact> ContactBook::searchByEmail(const QString &email) const
{
    QList<Contact> results;
    QString searchEmail = email.toLower();

    for (const Contact &contact : m_contacts) {
        if (contact.getEmail().toLower().contains(searchEmail)) {
            results.append(contact);
        }
    }

    return results;
}

void ContactBook::sortById(bool ascending)
{
    std::sort(m_contacts.begin(), m_contacts.end(),
        [ascending](const Contact& a, const Contact& b) {
            return Contact::compareById(a, b, ascending);
        });
}

void ContactBook::sortBySurname(bool ascending)
{
    std::sort(m_contacts.begin(), m_contacts.end(),
        [ascending](const Contact& a, const Contact& b) {
            return Contact::compareBySurname(a, b, ascending);
        });
}

void ContactBook::sortByName(bool ascending)
{
    std::sort(m_contacts.begin(), m_contacts.end(),
        [ascending](const Contact& a, const Contact& b) {
            return Contact::compareByName(a, b, ascending);
        });
}

void ContactBook::sortByPatronymic(bool ascending)
{
    std::sort(m_contacts.begin(), m_contacts.end(),
        [ascending](const Contact& a, const Contact& b) {
            return Contact::compareByPatronymic(a, b, ascending);
        });
}

void ContactBook::sortByEmail(bool ascending)
{
    std::sort(m_contacts.begin(), m_contacts.end(),
        [ascending](const Contact& a, const Contact& b) {
            return Contact::compareByEmail(a, b, ascending);
        });
}

void ContactBook::sortByBirthDate(bool ascending)
{
    std::sort(m_contacts.begin(), m_contacts.end(),
        [ascending](const Contact& a, const Contact& b) {
            return Contact::compareByBirthDate(a, b, ascending);
        });
}

void ContactBook::sortByPhones(bool ascending)
{
    std::sort(m_contacts.begin(), m_contacts.end(),
        [ascending](const Contact& a, const Contact& b) {
            return Contact::compareByPhones(a, b, ascending);
        });
}

void ContactBook::sortByColumn(int column, bool ascending)
{
    switch (column) {
        case 0: sortById(ascending); break;
        case 1: sortBySurname(ascending); break;
        case 2: sortByName(ascending); break;
        case 3: sortByPatronymic(ascending); break;
        case 4: sortByEmail(ascending); break;
        case 5: sortByBirthDate(ascending); break;
        case 6: sortByPhones(ascending); break;
        default: sortById(ascending); break;
    }

}

QMap<QString, int> ContactBook::getContactsByFirstLetter() const
{
    QMap<QString, int> result;

    for (const Contact &contact : m_contacts) {
        if (!contact.getSurname().isEmpty()) {
            QString firstLetter = contact.getSurname().left(1).toUpper();
            result[firstLetter]++;
        }
    }

    return result;
}

QMap<int, int> ContactBook::getContactsByYear() const
{
    QMap<int, int> result;

    for (const Contact &contact : m_contacts) {
        if (contact.getBirthDate().isValid()) {
            int year = contact.getBirthDate().year();
            result[year]++;
        }
    }

    return result;
}

bool ContactBook::saveToFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    for (const Contact &contact : m_contacts) {
        out << contact.getId() << ","
            << contact.getSurname() << ","
            << contact.getName() << ","
            << contact.getPatronymic() << ","
            << contact.getEmail() << ","
            << contact.getBirthDate().toString("dd.MM.yyyy") << ",";

        QStringList phones = contact.getPhones();
        for (int i = 0; i < phones.size(); ++i) {
            out << phones[i];
            if (i < phones.size() - 1) out << ";";
        }
        out << "\n";
    }

    file.close();
    return true;
}

bool ContactBook::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    m_contacts.clear();

    QTextStream in(&file);
    in.setCodec("UTF-8");

    int maxId = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");

        if (parts.size() < 7) {
            continue;
        }

        Contact contact;

        int id = parts[0].toInt();
        contact.setId(id);
        maxId = qMax(maxId, id);

        if (!contact.setSurname(parts[1])) continue;
        if (!contact.setName(parts[2])) continue;
        if (!contact.setPatronymic(parts[3])) continue;
        if (!contact.setEmail(parts[4])) continue;
        if (!contact.setBirthDate(parts[5])) continue;

        QStringList phones = parts[6].split(";");
        for (const QString &phone : phones) {
            if (!phone.isEmpty()) {
                contact.addPhone(phone);
            }
        }

        m_contacts.append(contact);
    }

    file.close();
    return true;
}

int ContactBook::getNextId() const
{
    int maxId = 0;
    for (const Contact &contact : m_contacts) {
        maxId = qMax(maxId, contact.getId());
    }
    return maxId + 1;
}

bool ContactBook::isEmailUnique(const QString &email, int excludeId) const
{
    QString searchEmail = email.toLower();

    for (const Contact &contact : m_contacts) {
        if (contact.getId() == excludeId) continue;

        if (contact.getEmail().toLower() == searchEmail) {
            return false;
        }
    }

    return true;
}

bool ContactBook::isPhoneUnique(const QString &phone, int excludeId) const
{
    QString normalizedPhone = Checker::normalizePhone(phone);

    for (const Contact &contact : m_contacts) {
        if (contact.getId() == excludeId) continue;

        for (const QString &contactPhone : contact.getPhones()) {
            if (Checker::normalizePhone(contactPhone) == normalizedPhone) {
                return false;
            }
        }
    }

    return true;
}

int ContactBook::findContactIndexById(int id) const
{
    for (int i = 0; i < m_contacts.size(); ++i) {
        if (m_contacts[i].getId() == id) {
            return i;
        }
    }
    return -1;
}
