#include "checker.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDate>
#include <unordered_map>

bool Checker::validateName(const QString& name)
{
    QRegularExpression regex("^[A-Za-zА-Яа-я]*(\\-[A-Za-zА-Яа-я]*)*[A-Za-zА-Яа-я]$");
    return regex.match(name).hasMatch();
}

QString Checker::formatName(const QString& name)
{
    QString formatted = name.trimmed();
    if (!formatted.isEmpty()) {
        formatted[0] = formatted[0].toUpper();
        for (int i = 1; i < formatted.length(); ++i) {
            formatted[i] = formatted[i].toLower();
        }
    }
    return formatted;
}


bool Checker::validateSurname(const QString& surname)
{
    return validateName(surname);
}

QString Checker::formatSurname(const QString& surname)
{
    return formatName(surname);
}

bool Checker::validatePatronymic(const QString& patronymic)
{
    if (patronymic == "-" || patronymic == "") return true;
    return validateName(patronymic);
}

QString Checker::formatPatronymic(const QString& patronymic)
{
    if (patronymic == "-" || patronymic == "") return "-";
    return formatName(patronymic);
}

bool Checker::validatePhone(const QString& phone)
{
    // Разные форматы номеров: +7(XXX)XXX-XX-XX, 8(XXX)XXX-XX-XX, XXX-XX-XX
    QRegularExpression regex(
                "^"
                       "(\\+7|8)?"  // необязательный код страны
                       "\\(?\\d{3}\\)?"  // код оператора (3 цифры) в скобках или без
                       "\\d{3}"  // первые 3 цифры номера
                       "[-]?"
                       "\\d{2}"  // следующие 2 цифры
                       "[-]?"
                       "\\d{2}"  // последние 2 цифры
                       "$" );
    return regex.match(phone).hasMatch();
}

QString Checker::formatPhone(const QString& phone)
{
    QString cleaned = phone;
    cleaned.remove(QRegularExpression("[^\\d+]")); // Удаляем все кроме цифр и +

    if (cleaned.startsWith("+7")) {
        // Формат: +7(XXX)XXX-XX-XX
        if (cleaned.length() == 12) { // +7XXXXXXXXXX
            return QString("+7(%1)%2-%3-%4")
                .arg(cleaned.mid(2, 3))
                .arg(cleaned.mid(5, 3))
                .arg(cleaned.mid(8, 2))
                .arg(cleaned.mid(10, 2));
        }
    } else if (cleaned.startsWith("8")) {
        // Формат: 8(XXX)XXX-XX-XX
        if (cleaned.length() == 11) { // 8XXXXXXXXXX
            return QString("8(%1)%2-%3-%4")
                .arg(cleaned.mid(1, 3))
                .arg(cleaned.mid(4, 3))
                .arg(cleaned.mid(7, 2))
                .arg(cleaned.mid(9, 2));
        }
    } else if (cleaned.length() == 6) {
        // Формат: XXX-XX-XX
        return QString("%1-%2-%3")
            .arg(cleaned.mid(0, 3))
            .arg(cleaned.mid(3, 2))
            .arg(cleaned.mid(5, 2));
    }

    return cleaned; // Возвращаем как есть если не подходит под форматы
}

QString Checker::normalizePhone(const QString& phone)
{
    // Приводим к единому формату для проверки уникальности
    QString cleaned = phone;
    cleaned.remove(QRegularExpression("[^\\d]"));

    if (cleaned.startsWith("7") || cleaned.startsWith("8")) {
        cleaned = cleaned.mid(1); // Убираем первую цифру
    }

    return cleaned;
}

bool Checker::validateEmail(const QString& email)
{
    QRegularExpression regex(
        "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"
    );
    return regex.match(email).hasMatch();
}

QString Checker::formatEmail(const QString& email)
{
    return email.trimmed().toLower();
}

bool Checker::validateDate(const QString& date)
{
    QRegularExpression regex("^\\d{2}\\.\\d{2}\\.\\d{4}$");
    if (!regex.match(date).hasMatch()) {
        return false;
    }

    // Проверка корректности даты
    QStringList parts = date.split('.');
    if (parts.size() != 3) return false;

    int day = parts[0].toInt();
    int month = parts[1].toInt();
    int year = parts[2].toInt();

    // Проверяем диапазоны
    if (year < 1900 || year > QDate::currentDate().year()) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;

    // Проверяем существование даты
    QDate qdate(year, month, day);
    if (!qdate.isValid()) return false;

    // Дата не должна быть в будущем
    if (qdate > QDate::currentDate()) return false;

    return true;
}

QString Checker::formatDate(const QString& date)
{
    QString formatted = date.trimmed();
    // Добавляем ведущие нули если нужно
    QStringList parts = formatted.split('.');
    if (parts.size() == 3) {
        if (parts[0].length() == 1) parts[0] = "0" + parts[0];
        if (parts[1].length() == 1) parts[1] = "0" + parts[1];
        formatted = parts[0] + "." + parts[1] + "." + parts[2];
    }
    return formatted;
}

QString Checker::generateEmail(const QString& name)
{
    QString email = name.toLower() + "@gmail.com";
    return email;
}

bool Checker::isLatin(const QString& str)
{
    QRegularExpression regex("^[A-Za-z]+$");
    return regex.match(str).hasMatch();
}

bool Checker::isFirstUpper(const QString& str)
{
    if (str.isEmpty()) return false;
    return str[0].isUpper();
}
