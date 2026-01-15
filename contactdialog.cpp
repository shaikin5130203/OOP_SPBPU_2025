#include "contactdialog.h"
#include "checker.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QApplication>
#include <QClipboard>

ContactDialog::ContactDialog(QWidget *parent, bool editMode)
    : QDialog(parent), isEditMode(editMode)
{
    setWindowTitle(editMode ? "Редактировать контакт" : "Добавить контакт");
    setMinimumSize(500, 550);

    setupUI();
    setupValidators();
    validateInput();
}

ContactDialog::~ContactDialog() {}

void ContactDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Поля ввода
    QGridLayout *formLayout = new QGridLayout();

    // Фамилия
    formLayout->addWidget(new QLabel("Фамилия*:"), 0, 0);
    surnameEdit = new QLineEdit();
    surnameEdit->setPlaceholderText("Иванов/Ivanov");
    formLayout->addWidget(surnameEdit, 0, 1);

    // Имя
    formLayout->addWidget(new QLabel("Имя*:"), 1, 0);
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Иван/Ivan");
    formLayout->addWidget(nameEdit, 1, 1);

    // Отчество
    formLayout->addWidget(new QLabel("Отчество (при наличии):"), 2, 0);
    patronymicEdit = new QLineEdit();
    patronymicEdit->setPlaceholderText("Иванович");
    formLayout->addWidget(patronymicEdit, 2, 1);

    // Email
    formLayout->addWidget(new QLabel("Email*:"), 3, 0);
    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("yourname@gmail.com");
    emailEdit->setReadOnly(true);
    formLayout->addWidget(emailEdit, 3, 1);

    // Дата рождения
    formLayout->addWidget(new QLabel("Дата рождения*:"), 4, 0);
    birthDateEdit = new QLineEdit();
    birthDateEdit->setPlaceholderText("дд.мм.гггг");
    formLayout->addWidget(birthDateEdit, 4, 1);

    mainLayout->addLayout(formLayout);

    // Телефоны
    QGroupBox *phoneGroup = new QGroupBox("Телефоны* (минимум 1)");
    QVBoxLayout *phoneLayout = new QVBoxLayout(phoneGroup);

    QHBoxLayout *phoneInputLayout = new QHBoxLayout();
    phoneEdit = new QLineEdit();
    phoneEdit->setPlaceholderText("+7(999)123-45-67 или 8(999)123-45-67");
    addPhoneButton = new QPushButton("Добавить");
    removePhoneButton = new QPushButton("Удалить выбранный");

    phoneInputLayout->addWidget(phoneEdit);
    phoneInputLayout->addWidget(addPhoneButton);
    phoneInputLayout->addWidget(removePhoneButton);

    phoneListWidget = new QListWidget();
    phoneListWidget->setSelectionMode(QListWidget::SingleSelection);

    phoneLayout->addLayout(phoneInputLayout);
    phoneLayout->addWidget(phoneListWidget);

    mainLayout->addWidget(phoneGroup);

    // Кнопки диалога
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    // Подключение сигналов
    connect(addPhoneButton, &QPushButton::clicked, this, &ContactDialog::onAddPhone);
    connect(removePhoneButton, &QPushButton::clicked, this, &ContactDialog::onRemovePhone);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ContactDialog::onAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    connect(nameEdit, &QLineEdit::textChanged, this, &ContactDialog::onNameChanged);
    connect(surnameEdit, &QLineEdit::textChanged, this, &ContactDialog::onNameChanged);
    connect(emailEdit, &QLineEdit::textChanged, this, &ContactDialog::validateInput);
    connect(birthDateEdit, &QLineEdit::textChanged, this, &ContactDialog::validateInput);
    connect(phoneListWidget, &QListWidget::itemChanged, this, &ContactDialog::validateInput);

    // Обновление состояния кнопки OK
    validateInput();
}

void ContactDialog::setupValidators()
{
    // Валидатор для имени/фамилии
    QRegularExpression nameRegex("^[A-Za-zА-Яа-я]*(\\-[A-ZА-Яа-яa-z]*)*$");
    QRegularExpressionValidator *nameValidator = new QRegularExpressionValidator(nameRegex, this);

    // Валидатор для отчества
    QRegularExpression patronymicRegex("^[A-Za-zА-Яа-я]*(\\-[A-ZА-Яа-яa-z]*)*$|^$");
    QRegularExpressionValidator *patronymicValidator = new QRegularExpressionValidator(patronymicRegex, this);

    // Валидатор для даты (дд.мм.гггг)
    QRegularExpression dateRegex("^\\d{0,2}\\.?\\d{0,2}\\.?\\d{0,4}$");
    QRegularExpressionValidator *dateValidator = new QRegularExpressionValidator(dateRegex, this);

    surnameEdit->setValidator(nameValidator);
    nameEdit->setValidator(nameValidator);
    patronymicEdit->setValidator(patronymicValidator);
    birthDateEdit->setValidator(dateValidator);
}

void ContactDialog::setContact(const Contact& contact)
{
    currentContact = contact;

    surnameEdit->setText(contact.getSurname());
    nameEdit->setText(contact.getName());
    patronymicEdit->setText(contact.getPatronymic());
    emailEdit->setText(contact.getEmail());
    birthDateEdit->setText(contact.getBirthDateString());

    phoneListWidget->clear();
    QStringList phones = contact.getPhones();
    for (const QString& number : phones) {
        phoneListWidget->addItem(number);
    }

    validateInput();
}

Contact ContactDialog::getContact() const
{
    Contact contact;

    contact.setSurname(surnameEdit->text());
    contact.setName(nameEdit->text());
    contact.setPatronymic(patronymicEdit->text());
    contact.setEmail(emailEdit->text());
    contact.setBirthDate(birthDateEdit->text());

    QStringList phones;
    for (int i = 0; i < phoneListWidget->count(); ++i) {
        phones.append(phoneListWidget->item(i)->text());
    }
    contact.setPhones(phones);

    return contact;
}

void ContactDialog::onAddPhone()
{
    QString phone = phoneEdit->text().trimmed();
    if (phone.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер телефона.");
        return;
    }

    if (!Checker::validatePhone(phone)) {
        QMessageBox::warning(this, "Ошибка",
            "Неверный формат телефона.\nПримеры:\n"
            "+7(999)123-45-67\n"
            "8(999)123-45-67\n");
        return;
    }

    QString formattedPhone = Checker::formatPhone(phone);

    // Проверка уникальности в списке уже добавленных телефонов
    for (int i = 0; i < phoneListWidget->count(); ++i) {
        if (Checker::normalizePhone(phoneListWidget->item(i)->text()) ==
            Checker::normalizePhone(formattedPhone)) {
            QMessageBox::warning(this, "Ошибка", "Этот номер уже добавлен.");
            return;
        }
    }

    phoneListWidget->addItem(formattedPhone);
    phoneEdit->clear();
    validateInput();
}

void ContactDialog::onRemovePhone()
{
    int row = phoneListWidget->currentRow();
    if (row >= 0) {
        delete phoneListWidget->takeItem(row);
        validateInput();
    }
}

void ContactDialog::onAccept()
{
    if (!validateForAccept()) {
        return; // Не закрываем диалог при ошибках
    }
    accept();
}

bool ContactDialog::validateForAccept()
{
    QStringList errors;

    // Проверка фамилии
    if (surnameEdit->text().isEmpty()) {
        errors.append("Фамилия обязательна.");
    } else if (!Checker::validateSurname(surnameEdit->text())) {
        errors.append("Фамилия не может оканчиваться дефисом.");
    }

    // Проверка имени
    if (nameEdit->text().isEmpty()) {
        errors.append("Имя обязательно.");
    } else if (!Checker::validateName(nameEdit->text())) {
        errors.append("Имя не может оканчиваться дефисом.");
    }

    // Проверка отчества
    if (patronymicEdit->text().isEmpty()){
        patronymicEdit->text() = "-";
    }
    else if (!Checker::validatePatronymic(patronymicEdit->text())) {
        errors.append("Отчество не может оканчиваться дефисом.");
    }

    // Проверка email
    if (emailEdit->text().isEmpty()) {
        errors.append("Email обязателен.");
    } else if (!Checker::validateEmail(emailEdit->text())) {
        errors.append("Неверный формат email. Пример: example@gmail.com");
    }

    // Проверка даты рождения
    if (birthDateEdit->text().isEmpty()) {
        errors.append("Дата рождения обязательна.");
    } else if (!Checker::validateDate(birthDateEdit->text())) {
        errors.append("Неверная дата рождения. Используйте формат ДД.ММ.ГГГГ");
    }

    // Проверка телефонов
    if (phoneListWidget->count() == 0) {
        errors.append("Добавьте хотя бы один номер телефона.");
    }
    if (!errors.isEmpty()) {
        QString errorText = "Обнаружены ошибки:\n• " + errors.join("\n• ");
        QMessageBox::warning(this, "Ошибка ввода", errorText);
        return false;
    }

    return true;
}

void ContactDialog::validateInput()
{
    // Простая проверка для активации/деактивации кнопки OK
    bool isValid = !surnameEdit->text().isEmpty() &&
                   !nameEdit->text().isEmpty() &&
                   !emailEdit->text().isEmpty() &&
                   !birthDateEdit->text().isEmpty() &&
                   phoneListWidget->count() > 0;

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

void ContactDialog::onNameChanged()
{
    // Автоматически обновляем email при изменении имени
        updateEmailFromName();

    validateInput();
}

void ContactDialog::onEmailGenerated()
{
    updateEmailFromName();
}

void ContactDialog::updateEmailFromName()
{
    QString name = nameEdit->text().toLower();
    if (name.isEmpty())
        return;

    // Таблица транслитерации
    static const QMap<QChar, QString> map = {
        {u'а',"a"}, {u'б',"b"}, {u'в',"v"}, {u'г',"g"}, {u'д',"d"},
        {u'е',"e"}, {u'ё',"yo"}, {u'ж',"zh"}, {u'з',"z"}, {u'и',"i"},
        {u'й',"y"}, {u'к',"k"}, {u'л',"l"}, {u'м',"m"}, {u'н',"n"},
        {u'о',"o"}, {u'п',"p"}, {u'р',"r"}, {u'с',"s"}, {u'т',"t"},
        {u'у',"u"}, {u'ф',"f"}, {u'х',"kh"}, {u'ц',"ts"}, {u'ч',"ch"},
        {u'ш',"sh"}, {u'щ',"sch"}, {u'ъ',""}, {u'ы',"y"}, {u'ь',""},
        {u'э',"e"}, {u'ю',"yu"}, {u'я',"ya"}
    };

    QString transliterated;
    for (QChar c : name) {
        if (map.contains(c))
            transliterated += map[c];
        else
            transliterated += c; // латиница, цифры, символы
    }

    emailEdit->setText(Checker::generateEmail(transliterated));
    validateInput();
}

