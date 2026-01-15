#include "databaseconnection.h"
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>

DatabaseConnection& DatabaseConnection::instance()
{
    static DatabaseConnection instance;
    return instance;
}

DatabaseConnection::DatabaseConnection(QObject* parent)
    : QObject(parent), m_port(5432)
{
    loadSettings();
}

DatabaseConnection::~DatabaseConnection()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseConnection::connect()
{
    if (isConnected()) {
        return true;
    }

    db = QSqlDatabase::addDatabase("QPSQL", "phonebook_connection");
    db.setHostName(m_host);
    db.setPort(m_port);
    db.setDatabaseName(m_databaseName);
    db.setUserName(m_userName);
    db.setPassword(m_password);

    // Настройки подключения
    db.setConnectOptions("connect_timeout=5");

    if (!db.open()) {
        m_lastError = db.lastError().text();
        return false;
    }

    // Проверяем наличие таблиц
    QSqlQuery query(db);
    QStringList requiredTables = {"contacts", "phone_numbers"};

    for (const QString& table : requiredTables) {
        if (!query.exec(QString("SELECT EXISTS (SELECT FROM information_schema.tables WHERE table_name = '%1')").arg(table))) {
            m_lastError = query.lastError().text();
            db.close();
            return false;
        }

        query.next();
        if (!query.value(0).toBool()) {
            m_lastError = QString("Таблица '%1' не найдена в базе данных").arg(table);
            db.close();
            return false;
        }
    }

    m_lastError.clear();
    return true;
}

bool DatabaseConnection::disconnect()
{
    if (db.isOpen()) {
        db.close();
    }
    return true;
}

bool DatabaseConnection::isConnected() const
{
    return db.isOpen();
}

QSqlDatabase DatabaseConnection::database() const
{
    return db;
}

QString DatabaseConnection::lastError() const
{
    return m_lastError;
}

bool DatabaseConnection::beginTransaction()
{
    if (!isConnected()) return false;

    QSqlQuery query(db);
    return query.exec("BEGIN");
}

bool DatabaseConnection::commitTransaction()
{
    if (!isConnected()) return false;

    QSqlQuery query(db);
    return query.exec("COMMIT");
}

bool DatabaseConnection::rollbackTransaction()
{
    if (!isConnected()) return false;

    QSqlQuery query(db);
    return query.exec("ROLLBACK");
}

bool DatabaseConnection::testConnection()
{
    if (!connect()) {
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT 1")) {
        m_lastError = query.lastError().text();
        disconnect();
        return false;
    }

    return true;
}

void DatabaseConnection::setConnectionParameters(const QString& host, int port,
                                                 const QString& databaseName,
                                                 const QString& userName,
                                                 const QString& password)
{
    m_host = host;
    m_port = port;
    m_databaseName = databaseName;
    m_userName = userName;
    m_password = password;
}

QString DatabaseConnection::host() const { return m_host; }
int DatabaseConnection::port() const { return m_port; }
QString DatabaseConnection::databaseName() const { return m_databaseName; }
QString DatabaseConnection::userName() const { return m_userName; }
QString DatabaseConnection::password() const { return m_password; }

void DatabaseConnection::saveSettings()
{
    QSettings settings("PhoneBook", "Database");
    settings.setValue("host", m_host);
    settings.setValue("port", m_port);
    settings.setValue("databaseName", m_databaseName);
    settings.setValue("userName", m_userName);
    settings.setValue("password", m_password);
}

void DatabaseConnection::loadSettings()
{
    QSettings settings("PhoneBook", "Database");
    m_host = settings.value("host", "localhost").toString();
    m_port = settings.value("port", 5432).toInt();
    m_databaseName = settings.value("databaseName", "phonebook").toString();
    m_userName = settings.value("userName", "postgres").toString();
    m_password = settings.value("password", "").toString();

    // Если настройки не сохранены, используем значения по умолчанию
    if (m_host.isEmpty()) m_host = "localhost";
    if (m_databaseName.isEmpty()) m_databaseName = "phonebook";
    if (m_userName.isEmpty()) m_userName = "postgres";
}
