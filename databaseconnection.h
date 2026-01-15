#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSettings>

class DatabaseConnection : public QObject
{
    Q_OBJECT

public:
    static DatabaseConnection& instance();

    bool connect();
    bool disconnect();
    bool isConnected() const;

    QSqlDatabase database() const;
    QString lastError() const;

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    bool testConnection();

    // Настройки подключения
    void setConnectionParameters(const QString& host, int port,
                                 const QString& databaseName,
                                 const QString& userName,
                                 const QString& password);

    QString host() const;
    int port() const;
    QString databaseName() const;
    QString userName() const;
    QString password() const;  // Метод добавлен

    void saveSettings();
    void loadSettings();

private:
    DatabaseConnection(QObject* parent = nullptr);
    ~DatabaseConnection();

    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    QSqlDatabase db;
    QString m_host;
    int m_port;
    QString m_databaseName;
    QString m_userName;
    QString m_password;

    QString m_lastError;
};

#endif // DATABASECONNECTION_H
