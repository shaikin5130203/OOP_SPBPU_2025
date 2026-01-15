#ifndef CHECKER_H
#define CHECKER_H

#include <QString>
#include <QDate>

class Checker
{
public:
    // Проверка имени (только латиница, первая заглавная)
    static bool validateName(const QString& name);
    static QString formatName(const QString& name);

    // Проверка фамилии
    static bool validateSurname(const QString& surname);
    static QString formatSurname(const QString& surname);

    // Проверка отчества (может быть "-" если нет)
    static bool validatePatronymic(const QString& patronymic);
    static QString formatPatronymic(const QString& patronymic);

    // Проверка номера телефона
    static bool validatePhone(const QString& phone);
    static QString formatPhone(const QString& phone);
    static QString normalizePhone(const QString& phone); // Для проверки уникальности
    // Проверка email
    static bool validateEmail(const QString& email);
    static QString formatEmail(const QString& email);

    // Проверка даты рождения
    static bool validateDate(const QString& date);
    static QString formatDate(const QString& date);

    // Генерация email из имени
    static QString generateEmail(const QString& name);

    // Вспомогательные функции
    static bool isLatin(const QString& str);
    static bool isFirstUpper(const QString& str);
};

#endif // CHECKER_H
