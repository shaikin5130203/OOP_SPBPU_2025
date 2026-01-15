#include "contactdao.h"
#include "databaseconnection.h"
#include "checker.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <stdexcept>

ContactDAO& ContactDAO::instance()
{
    static ContactDAO instance;
    return instance;
}

ContactDAO::ContactDAO(QObject* parent)
    : QObject(parent)
{
}

ContactDAO::~ContactDAO()
{
}

bool ContactDAO::addContact(Contact& contact)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            qDebug() << "Failed to connect to database:" << db.lastError();
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    // Начинаем транзакцию
    if (!db.beginTransaction()) {
        qDebug() << "Failed to begin transaction:" << db.lastError();
        throw std::runtime_error(db.lastError().toStdString());
    }

    try {
        // Вставляем контакт
        QSqlQuery query(database);
        query.prepare(
            "INSERT INTO contacts (surname, name, patronymic, email, birth_date) "
            "VALUES (:surname, :name, :patronymic, :email, :birth_date) "
            "RETURNING id"
        );

        query.bindValue(":surname", contact.getSurname());
        query.bindValue(":name", contact.getName());
        query.bindValue(":patronymic", contact.getPatronymic());
        query.bindValue(":email", contact.getEmail());
        query.bindValue(":birth_date", contact.getBirthDate());

        if (!query.exec()) {
            qDebug() << "Failed to insert contact:" << query.lastError().text();
            db.rollbackTransaction();
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        // Получаем ID нового контакта
        if (query.next()) {
            int newId = query.value(0).toInt();
            contact.setId(newId);

            // Сохраняем телефонные номера
            if (!savePhoneNumbers(newId, contact.getPhones())) {
                db.rollbackTransaction();
                throw std::runtime_error("Не удалось сохранить номера телефонов");
            }

            // Фиксируем транзакцию
            if (!db.commitTransaction()) {
                qDebug() << "Failed to commit transaction:" << db.lastError();
                db.rollbackTransaction();
                throw std::runtime_error(db.lastError().toStdString());
            }

            // Добавляем запись в историю
            addToHistory(newId, "CREATE", "", QString("Contact created"));

            return true;
        }

        db.rollbackTransaction();
        throw std::runtime_error("Не удалось получить ID нового контакта");

    } catch (const std::exception& e) {
        qDebug() << "Exception in addContact:" << e.what();
        db.rollbackTransaction();
        throw;
    }
}

bool ContactDAO::updateContact(const Contact& contact)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    if (!db.beginTransaction()) {
        throw std::runtime_error(db.lastError().toStdString());
    }

    try {
        // Получаем старые значения для истории
        Contact oldContact = getContactById(contact.getId());

        // Обновляем контакт
        QSqlQuery query(database);
        query.prepare(
            "UPDATE contacts SET "
            "surname = :surname, "
            "name = :name, "
            "patronymic = :patronymic, "
            "email = :email, "
            "birth_date = :birth_date "
            "WHERE id = :id"
        );

        query.bindValue(":surname", contact.getSurname());
        query.bindValue(":name", contact.getName());
        query.bindValue(":patronymic", contact.getPatronymic());
        query.bindValue(":email", contact.getEmail());
        query.bindValue(":birth_date", contact.getBirthDate());
        query.bindValue(":id", contact.getId());

        if (!query.exec()) {
            db.rollbackTransaction();
            throw std::runtime_error(query.lastError().text().toStdString());
        }

        // Удаляем старые телефоны и сохраняем новые
        if (!deletePhoneNumbers(contact.getId()) ||
            !savePhoneNumbers(contact.getId(), contact.getPhones())) {
            db.rollbackTransaction();
            throw std::runtime_error("Не удалось обновить номера телефонов");
        }

        if (!db.commitTransaction()) {
            db.rollbackTransaction();
            throw std::runtime_error(db.lastError().toStdString());
        }

        // Добавляем изменения в историю
        if (oldContact.getSurname() != contact.getSurname()) {
            addToHistory(contact.getId(), "surname", oldContact.getSurname(), contact.getSurname());
        }
        if (oldContact.getName() != contact.getName()) {
            addToHistory(contact.getId(), "name", oldContact.getName(), contact.getName());
        }
        if (oldContact.getEmail() != contact.getEmail()) {
            addToHistory(contact.getId(), "email", oldContact.getEmail(), contact.getEmail());
        }
        if (oldContact.getBirthDate() != contact.getBirthDate()) {
            addToHistory(contact.getId(), "birth_date",
                        oldContact.getBirthDate().toString("dd.MM.yyyy"),
                        contact.getBirthDate().toString("dd.MM.yyyy"));
        }

        return true;

    } catch (const std::exception& e) {
        qDebug() << "Exception in updateContact:" << e.what();
        db.rollbackTransaction();
        throw;
    }
}

bool ContactDAO::deleteContact(int contactId)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare("DELETE FROM contacts WHERE id = :id");
    query.bindValue(":id", contactId);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return true;
}

bool ContactDAO::deleteContact(const Contact& contact)
{
    return deleteContact(contact.getId());
}

Contact ContactDAO::getContactById(int id)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare("SELECT * FROM contacts WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (query.next()) {
        return contactFromQuery(query);
    }

    return Contact();
}

QList<Contact> ContactDAO::getAllContacts()
{
    QList<Contact> contacts;

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare(
        "SELECT c.*, "
        "ARRAY_AGG(pn.phone_number ORDER BY pn.is_primary DESC, pn.id) as phone_numbers "
        "FROM contacts c "
        "LEFT JOIN phone_numbers pn ON c.id = pn.contact_id "
        "GROUP BY c.id "
        "ORDER BY c.surname, c.name"
    );

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        contacts.append(contactFromQuery(query));
    }

    return contacts;
}

QList<Contact> ContactDAO::getContacts(int limit, int offset)
{
    QList<Contact> contacts;

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QString sql =
        "SELECT c.*, "
        "ARRAY_AGG(pn.phone_number ORDER BY pn.is_primary DESC, pn.id) as phone_numbers "
        "FROM contacts c "
        "LEFT JOIN phone_numbers pn ON c.id = pn.contact_id "
        "GROUP BY c.id "
        "ORDER BY c.surname, c.name";

    if (limit > 0) {
        sql += QString(" LIMIT %1 OFFSET %2").arg(limit).arg(offset);
    }

    QSqlQuery query(database);
    query.prepare(sql);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        contacts.append(contactFromQuery(query));
    }

    return contacts;
}

QList<Contact> ContactDAO::searchByName(const QString& name)
{
    return searchByField("name", name);
}

QList<Contact> ContactDAO::searchBySurname(const QString& surname)
{
    return searchByField("surname", surname);
}

QList<Contact> ContactDAO::searchByPhone(const QString& phone)
{
    QList<Contact> contacts;

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QString normalizedPhone = Checker::normalizePhone(phone);

    QSqlQuery query(database);
    query.prepare(
        "SELECT DISTINCT c.* "
        "FROM contacts c "
        "JOIN phone_numbers pn ON c.id = pn.contact_id "
        "WHERE pn.phone_number LIKE :phone "
        "ORDER BY c.surname, c.name"
    );

    query.bindValue(":phone", "%" + normalizedPhone + "%");

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        Contact contact = contactFromQuery(query);
        contact.setPhones(getPhoneNumbers(contact.getId()));
        contacts.append(contact);
    }

    return contacts;
}

QList<Contact> ContactDAO::searchByEmail(const QString& email)
{
    return searchByField("email", email);
}

QList<Contact> ContactDAO::searchByBirthDateRange(const QDate& from, const QDate& to)
{
    QList<Contact> contacts;

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare(
        "SELECT c.*, "
        "ARRAY_AGG(pn.phone_number ORDER BY pn.is_primary DESC, pn.id) as phone_numbers "
        "FROM contacts c "
        "LEFT JOIN phone_numbers pn ON c.id = pn.contact_id "
        "WHERE c.birth_date BETWEEN :from_date AND :to_date "
        "GROUP BY c.id "
        "ORDER BY c.birth_date"
    );

    query.bindValue(":from_date", from);
    query.bindValue(":to_date", to);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        contacts.append(contactFromQuery(query));
    }

    return contacts;
}

int ContactDAO::getContactCount()
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare("SELECT COUNT(*) FROM contacts");

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

QMap<QString, int> ContactDAO::getContactsByFirstLetter()
{
    QMap<QString, int> result;

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare(
        "SELECT UPPER(LEFT(surname, 1)) as letter, COUNT(*) as count "
        "FROM contacts "
        "GROUP BY letter "
        "ORDER BY letter"
    );

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        result[query.value("letter").toString()] = query.value("count").toInt();
    }

    return result;
}

QMap<int, int> ContactDAO::getContactsByYear()
{
    QMap<int, int> result;

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare(
        "SELECT EXTRACT(YEAR FROM birth_date) as year, COUNT(*) as count "
        "FROM contacts "
        "GROUP BY year "
        "ORDER BY year"
    );

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        result[query.value("year").toInt()] = query.value("count").toInt();
    }

    return result;
}

bool ContactDAO::isEmailUnique(const QString& email, int excludeId)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);

    if (excludeId >= 0) {
        query.prepare("SELECT COUNT(*) FROM contacts WHERE email = :email AND id != :id");
        query.bindValue(":id", excludeId);
    } else {
        query.prepare("SELECT COUNT(*) FROM contacts WHERE email = :email");
    }

    query.bindValue(":email", email);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (query.next()) {
        return query.value(0).toInt() == 0;
    }

    return false;
}

bool ContactDAO::isPhoneUnique(const QString& phone, int excludeId)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);

    QString normalizedPhone = Checker::normalizePhone(phone);

    if (excludeId >= 0) {
        query.prepare(
            "SELECT COUNT(*) FROM phone_numbers pn "
            "JOIN contacts c ON pn.contact_id = c.id "
            "WHERE regexp_replace(pn.phone_number, '[^0-9]', '', 'g') LIKE :phone AND c.id != :id"
        );
        query.bindValue(":id", excludeId);
    } else {
        query.prepare("SELECT COUNT(*) FROM phone_numbers WHERE regexp_replace(phone_number, '[^0-9]', '', 'g') LIKE :phone");
    }

    query.bindValue(":phone", "%" + normalizedPhone + "%");

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    if (query.next()) {
        return query.value(0).toInt() == 0;
    }

    return false;
}

bool ContactDAO::importContacts(const QList<Contact>& contacts)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    if (!db.beginTransaction()) {
        throw std::runtime_error(db.lastError().toStdString());
    }

    try {
        for (const Contact& contact : contacts) {
            Contact tempContact = contact;

            // Проверяем уникальность email перед добавлением
            if (!isEmailUnique(tempContact.getEmail())) {
                db.rollbackTransaction();
                throw std::runtime_error("Email уже используется: " + tempContact.getEmail().toStdString());
            }

            // Проверяем уникальность телефонов перед добавлением
            QStringList phones = tempContact.getPhones();
            for (const QString &phone : phones) {
                if (!isPhoneUnique(phone)) {
                    db.rollbackTransaction();
                    throw std::runtime_error("Номер телефона уже используется: " + phone.toStdString());
                }
            }

            if (!addContact(tempContact)) {
                db.rollbackTransaction();
                throw std::runtime_error("Не удалось импортировать контакт");
            }
        }

        if (!db.commitTransaction()) {
            db.rollbackTransaction();
            throw std::runtime_error(db.lastError().toStdString());
        }

        return true;

    } catch (const std::exception& e) {
        db.rollbackTransaction();
        throw;
    }
}

bool ContactDAO::exportContacts(const QString& filename, const QList<Contact>& contacts)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("Не удалось открыть файл для записи");
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    // Заголовок CSV
    out << "Surname,Name,Patronymic,Email,BirthDate,Phones\n";

    for (const Contact& contact : contacts) {
        QStringList phones = contact.getPhones();
        QString phonesStr = phones.join(";");

        out << contact.getSurname() << ","
            << contact.getName() << ","
            << contact.getPatronymic() << ","
            << contact.getEmail() << ","
            << contact.getBirthDate().toString("dd.MM.yyyy") << ","
            << "\"" << phonesStr << "\"\n";
    }

    file.close();
    return true;
}

void ContactDAO::addToHistory(int contactId, const QString& fieldName,
                              const QString& oldValue, const QString& newValue)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        return;
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare(
        "INSERT INTO contact_history (contact_id, field_name, old_value, new_value) "
        "VALUES (:contact_id, :field_name, :old_value, :new_value)"
    );

    query.bindValue(":contact_id", contactId);
    query.bindValue(":field_name", fieldName);
    query.bindValue(":old_value", oldValue);
    query.bindValue(":new_value", newValue);

    query.exec(); // Не критично, если не запишется
}

// Вспомогательные методы

Contact ContactDAO::contactFromQuery(const QSqlQuery& query)
{
    int id = query.value("id").toInt();
    QString surname = query.value("surname").toString();
    QString name = query.value("name").toString();
    QString patronymic = query.value("patronymic").toString();
    QString email = query.value("email").toString();
    QDate birthDate = query.value("birth_date").toDate();

    Contact contact(name, surname, patronymic, email, birthDate);
    contact.setId(id);

    // Получаем телефоны
    QString phonesString = query.value("phone_numbers").toString();
    if (!phonesString.isEmpty() && phonesString != "{}") {
        // Преобразуем строку массива PostgreSQL в QStringList
        phonesString = phonesString.mid(1, phonesString.length() - 2); // Убираем { и }
        QStringList phones = phonesString.split(",");
        contact.setPhones(phones);
    } else {
        // Если телефонов нет, получаем их отдельным запросом
        contact.setPhones(getPhoneNumbers(id));
    }

    return contact;
}

bool ContactDAO::savePhoneNumbers(int contactId, const QStringList& phoneNumbers)
{
    if (phoneNumbers.isEmpty()) {
        return true;
    }

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        throw std::runtime_error(db.lastError().toStdString());
    }

    QSqlDatabase database = db.database();

    for (int i = 0; i < phoneNumbers.size(); ++i) {
        // Проверяем уникальность каждого номера телефона
        if (!isPhoneUnique(phoneNumbers[i], contactId)) {
            throw std::runtime_error("Номер телефона уже используется: " + phoneNumbers[i].toStdString());
        }

        QSqlQuery query(database);
        query.prepare(
            "INSERT INTO phone_numbers (contact_id, phone_number, is_primary) "
            "VALUES (:contact_id, :phone_number, :is_primary)"
        );

        query.bindValue(":contact_id", contactId);
        query.bindValue(":phone_number", phoneNumbers[i]);
        query.bindValue(":is_primary", i == 0); // Первый телефон - основной

        if (!query.exec()) {
            throw std::runtime_error(query.lastError().text().toStdString());
        }
    }

    return true;
}

QStringList ContactDAO::getPhoneNumbers(int contactId)
{
    QStringList phones;

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        throw std::runtime_error(db.lastError().toStdString());
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare(
        "SELECT phone_number FROM phone_numbers "
        "WHERE contact_id = :contact_id "
        "ORDER BY is_primary DESC, id"
    );

    query.bindValue(":contact_id", contactId);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        phones.append(query.value(0).toString());
    }

    return phones;
}

bool ContactDAO::deletePhoneNumbers(int contactId)
{
    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        throw std::runtime_error(db.lastError().toStdString());
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare("DELETE FROM phone_numbers WHERE contact_id = :contact_id");
    query.bindValue(":contact_id", contactId);

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    return true;
}

QList<Contact> ContactDAO::searchByField(const QString& fieldName, const QString& value)
{
    QList<Contact> contacts;

    auto& db = DatabaseConnection::instance();
    if (!db.isConnected()) {
        if (!db.connect()) {
            throw std::runtime_error(db.lastError().toStdString());
        }
    }

    QSqlDatabase database = db.database();

    QSqlQuery query(database);
    query.prepare(
        QString(
            "SELECT c.*, "
            "ARRAY_AGG(pn.phone_number ORDER BY pn.is_primary DESC, pn.id) as phone_numbers "
            "FROM contacts c "
            "LEFT JOIN phone_numbers pn ON c.id = pn.contact_id "
            "WHERE c.%1 ILIKE :value "
            "GROUP BY c.id "
            "ORDER BY c.surname, c.name"
        ).arg(fieldName)
    );

    query.bindValue(":value", "%" + value + "%");

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }

    while (query.next()) {
        contacts.append(contactFromQuery(query));
    }

    return contacts;
}
