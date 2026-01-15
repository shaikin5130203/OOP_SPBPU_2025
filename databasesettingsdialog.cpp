#include "databasesettingsdialog.h"
#include "databaseconnection.h"
#include <QMessageBox>
#include <QFormLayout>
#include <QGroupBox>

DatabaseSettingsDialog::DatabaseSettingsDialog(QWidget *parent) :
    QDialog(parent),
    hostEdit(nullptr),
    portSpinBox(nullptr),
    databaseEdit(nullptr),
    userEdit(nullptr),
    passwordEdit(nullptr),
    testButton(nullptr),
    saveButton(nullptr),
    cancelButton(nullptr),
    buttonBox(nullptr)
{
    setupUI();

    // Загружаем текущие настройки
    auto& db = DatabaseConnection::instance();
    hostEdit->setText(db.host());
    portSpinBox->setValue(db.port());
    databaseEdit->setText(db.databaseName());
    userEdit->setText(db.userName());
    passwordEdit->setText(db.password());
}

DatabaseSettingsDialog::~DatabaseSettingsDialog()
{
    // Все виджеты будут удалены автоматически благодаря иерархии родителей
}

void DatabaseSettingsDialog::setupUI()
{
    setWindowTitle("Настройки подключения к базе данных");
    setMinimumSize(400, 300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Группа настроек
    QGroupBox *settingsGroup = new QGroupBox("Параметры подключения");
    QFormLayout *formLayout = new QFormLayout(settingsGroup);

    // Хост
    hostEdit = new QLineEdit();
    hostEdit->setPlaceholderText("localhost");
    formLayout->addRow("Хост:", hostEdit);

    // Порт
    portSpinBox = new QSpinBox();
    portSpinBox->setRange(1, 65535);
    portSpinBox->setValue(5432);
    formLayout->addRow("Порт:", portSpinBox);

    // Имя базы данных
    databaseEdit = new QLineEdit();
    databaseEdit->setPlaceholderText("phonebook");
    formLayout->addRow("База данных:", databaseEdit);

    // Пользователь
    userEdit = new QLineEdit();
    userEdit->setPlaceholderText("postgres");
    formLayout->addRow("Пользователь:", userEdit);

    // Пароль
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Пароль:", passwordEdit);

    mainLayout->addWidget(settingsGroup);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    testButton = new QPushButton("Тест подключения");
    saveButton = new QPushButton("Сохранить");
    cancelButton = new QPushButton("Отмена");

    buttonLayout->addWidget(testButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Подключение сигналов
    connect(testButton, &QPushButton::clicked, this, &DatabaseSettingsDialog::onTestConnection);
    connect(saveButton, &QPushButton::clicked, this, &DatabaseSettingsDialog::onSave);
    connect(cancelButton, &QPushButton::clicked, this, &DatabaseSettingsDialog::onCancel);
}

QString DatabaseSettingsDialog::host() const
{
    return hostEdit->text().trimmed();
}

int DatabaseSettingsDialog::port() const
{
    return portSpinBox->value();
}

QString DatabaseSettingsDialog::databaseName() const
{
    return databaseEdit->text().trimmed();
}

QString DatabaseSettingsDialog::userName() const
{
    return userEdit->text().trimmed();
}

QString DatabaseSettingsDialog::password() const
{
    return passwordEdit->text();
}

void DatabaseSettingsDialog::setHost(const QString& host)
{
    hostEdit->setText(host);
}

void DatabaseSettingsDialog::setPort(int port)
{
    portSpinBox->setValue(port);
}

void DatabaseSettingsDialog::setDatabaseName(const QString& databaseName)
{
    databaseEdit->setText(databaseName);
}

void DatabaseSettingsDialog::setUserName(const QString& userName)
{
    userEdit->setText(userName);
}

void DatabaseSettingsDialog::setPassword(const QString& password)
{
    passwordEdit->setText(password);
}

void DatabaseSettingsDialog::onTestConnection()
{
    DatabaseConnection& db = DatabaseConnection::instance();

    // Сохраняем временные настройки
    db.setConnectionParameters(host(), port(), databaseName(), userName(), password());

    if (db.testConnection()) {
        QMessageBox::information(this, "Подключение успешно",
            "Подключение к базе данных установлено успешно!");
    } else {
        QMessageBox::critical(this, "Ошибка подключения",
            QString("Не удалось подключиться к базе данных:\n%1").arg(db.lastError()));
    }
}

void DatabaseSettingsDialog::onSave()
{
    DatabaseConnection& db = DatabaseConnection::instance();

    // Сохраняем настройки
    db.setConnectionParameters(host(), port(), databaseName(), userName(), password());
    db.saveSettings();

    // Тестируем подключение
    if (!db.testConnection()) {
        QMessageBox::warning(this, "Внимание",
            "Настройки сохранены, но подключение не установлено.\n"
            "Проверьте параметры подключения.");
    }

    accept();
}

void DatabaseSettingsDialog::onCancel()
{
    reject();
}
