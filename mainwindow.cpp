#include "mainwindow.h"
#include "contactdialog.h"
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QShortcut>
#include <QKeySequence>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QScrollBar>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      tableWidget(nullptr),
      searchLineEdit(nullptr),
      contactCountLabel(nullptr),
      searchInfoLabel(nullptr),
      addButton(nullptr),
      editButton(nullptr),
      deleteButton(nullptr),
      saveButton(nullptr),
      loadButton(nullptr),
      exportButton(nullptr),
      refreshButton(nullptr),
      currentSortColumn(0),
      currentSortOrder(Qt::AscendingOrder),
      useDatabase(true) // По умолчанию используем БД
{
    setWindowTitle("Телефонная книга");
    setMinimumSize(1000, 600);

    setupUI();
    setupMenu();
    setupConnections();

    // Загружаем контакты из базы данных при старте
    loadContactsFromDatabase();

    statusBar()->showMessage("Готово");
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Панель поиска
    QGroupBox *searchGroup = new QGroupBox("Поиск контактов");
    QHBoxLayout *searchLayout = new QHBoxLayout(searchGroup);

    searchLineEdit = new QLineEdit();
    searchLineEdit->setPlaceholderText("Введите имя, фамилию, телефон или email...");

    QPushButton *searchButton = new QPushButton("Найти");
    QPushButton *clearButton = new QPushButton("Очистить");
    refreshButton = new QPushButton("Обновить");

    searchLayout->addWidget(searchLineEdit);
    searchLayout->addWidget(clearButton);
    searchLayout->addWidget(refreshButton);
    connect(searchLineEdit, &QLineEdit::textChanged, this, &MainWindow::onSearch);
    connect(clearButton, &QPushButton::clicked, this, [this]() {
        searchLineEdit->clear();
        if (useDatabase) {
            loadContactsFromDatabase();
        } else {
            onRefresh();
        }
    });
    // Таблица контактов
    tableWidget = new QTableWidget();
    tableWidget->horizontalHeader()->setStretchLastSection(true);

    setupTable();

    // Панель информации
    QGroupBox *infoGroup = new QGroupBox("Информация");
    QHBoxLayout *infoLayout = new QHBoxLayout(infoGroup);

    contactCountLabel = new QLabel("Контактов: 0");
    searchInfoLabel = new QLabel("");

    infoLayout->addWidget(contactCountLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(searchInfoLabel);

    // Панель кнопок действий
    QHBoxLayout *actionLayout = new QHBoxLayout();

    addButton = new QPushButton("Добавить");
    editButton = new QPushButton("Редактировать");
    deleteButton = new QPushButton("Удалить");
    saveButton = new QPushButton("Сохранить");
    loadButton = new QPushButton("Загрузить из файла");
    exportButton = new QPushButton("Экспорт");
    QPushButton *modeButton = new QPushButton(useDatabase ? "Режим: База данных" : "Режим: Файлы");

    addButton->setMinimumHeight(35);
    editButton->setMinimumHeight(35);
    deleteButton->setMinimumHeight(35);
    saveButton->setMinimumHeight(35);
    loadButton->setMinimumHeight(35);
    exportButton->setMinimumHeight(35);
    modeButton->setMinimumHeight(35);

    actionLayout->addWidget(addButton);
    actionLayout->addWidget(editButton);
    actionLayout->addWidget(deleteButton);
    actionLayout->addStretch();
    actionLayout->addWidget(loadButton);
    actionLayout->addWidget(saveButton);
    loadButton->setVisible(false);
    saveButton->setVisible(false);

    actionLayout->addWidget(modeButton);

    // Добавление в главный layout
    mainLayout->addWidget(searchGroup);
    mainLayout->addWidget(tableWidget, 1);
    mainLayout->addWidget(infoGroup);
    mainLayout->addLayout(actionLayout);

    // Горячие клавиши
    new QShortcut(QKeySequence("Ctrl+N"), this, SLOT(onAddContact()));
    new QShortcut(QKeySequence("Ctrl+E"), this, SLOT(onEditContact()));
    new QShortcut(QKeySequence("Delete"), this, SLOT(onDeleteContact()));
    new QShortcut(QKeySequence("Ctrl+S"), this, SLOT(onSave()));
    new QShortcut(QKeySequence("Ctrl+O"), this, SLOT(onLoad()));
    new QShortcut(QKeySequence("F5"), this, SLOT(onRefresh()));

    // Подключение кнопки переключения режима
    connect(modeButton, &QPushButton::clicked, this, [this, modeButton]() {
        useDatabase = !useDatabase;
        saveButton->setVisible(!useDatabase);
        loadButton->setVisible(!useDatabase);
        modeButton->setText(useDatabase ? "Режим: База данных" : "Режим: Файлы");
        if (useDatabase) {

            loadContactsFromDatabase();
        } else {
            refreshTable();
        }
        statusBar()->showMessage(useDatabase ? "Переключено на работу с базой данных" : "Переключено на работу с файлами", 3000);
    });
}

void MainWindow::setupTable()
{
    QStringList headers;
    headers << "ID" << "Фамилия" << "Имя" << "Отчество"
            << "Email" << "Дата рождения" << "Телефоны";

    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    // Настройки таблицы
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Включаем сортировку по клику на заголовок
    tableWidget->setSortingEnabled(true);

    // Настройка колонок
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionsClickable(true);

    // Устанавливаем сортировку по умолчанию (по ID)
    tableWidget->sortByColumn(0, Qt::AscendingOrder);
    currentSortColumn = 0;
    currentSortOrder = Qt::AscendingOrder;

    // Ширина колонок
    tableWidget->setColumnWidth(0, 60);   // ID
    tableWidget->setColumnWidth(1, 120);  // Фамилия
    tableWidget->setColumnWidth(2, 100);  // Имя
    tableWidget->setColumnWidth(3, 120);  // Отчество
    tableWidget->setColumnWidth(4, 180);  // Email
    tableWidget->setColumnWidth(5, 110);  // Дата рождения
}

void MainWindow::setupMenu()
{
    QMenu *fileMenu = menuBar()->addMenu("Файл");

    QAction *newAction = new QAction("Новый контакт", this);
    newAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(newAction);

    fileMenu->addSeparator();

    QAction *saveAction = new QAction("Сохранить в файл", this);
    saveAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveAction);

    QAction *loadAction = new QAction("Загрузить из файла", this);
    loadAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(loadAction);

    QAction *exportAction = new QAction("Экспорт...", this);
    fileMenu->addAction(exportAction);

    fileMenu->addSeparator();

    QAction *exitAction = new QAction("Выход", this);
    exitAction->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(exitAction);

    QMenu *editMenu = menuBar()->addMenu("Правка");

    QAction *editContactAction = new QAction("Редактировать контакт", this);
    editContactAction->setShortcut(QKeySequence("Ctrl+E"));
    editMenu->addAction(editContactAction);

    QAction *deleteContactAction = new QAction("Удалить контакт", this);
    deleteContactAction->setShortcut(QKeySequence::Delete);
    editMenu->addAction(deleteContactAction);

    editMenu->addSeparator();

    QAction *findAction = new QAction("Поиск", this);
    findAction->setShortcut(QKeySequence::Find);
    editMenu->addAction(findAction);

    QMenu *viewMenu = menuBar()->addMenu("Вид");

    QAction *sortByIdAction = new QAction("Сортировать по ID", this);
    viewMenu->addAction(sortByIdAction);

    QAction *sortBySurnameAction = new QAction("Сортировать по фамилии", this);
    viewMenu->addAction(sortBySurnameAction);

    QAction *sortByNameAction = new QAction("Сортировать по имени", this);
    viewMenu->addAction(sortByNameAction);

    QAction *sortByEmailAction = new QAction("Сортировать по email", this);
    viewMenu->addAction(sortByEmailAction);

    QAction *sortByDateAction = new QAction("Сортировать по дате рождения", this);
    viewMenu->addAction(sortByDateAction);

    // Меню База данных
    QMenu *dbMenu = menuBar()->addMenu("База данных");

    QAction *dbSettingsAction = new QAction("Настройки подключения", this);
    dbMenu->addAction(dbSettingsAction);

    QAction *dbStatsAction = new QAction("Статистика", this);
    dbMenu->addAction(dbStatsAction);

    QAction *refreshDBAction = new QAction("Обновить из БД", this);
    refreshDBAction->setShortcut(QKeySequence("F5"));
    dbMenu->addAction(refreshDBAction);

    QMenu *helpMenu = menuBar()->addMenu("Справка");
    QAction *aboutAction = new QAction("О программе", this);
    helpMenu->addAction(aboutAction);

    // Подключение меню
    connect(newAction, &QAction::triggered, this, &MainWindow::onAddContact);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSave);
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoad);
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExport);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(editContactAction, &QAction::triggered, this, &MainWindow::onEditContact);
    connect(deleteContactAction, &QAction::triggered, this, &MainWindow::onDeleteContact);
    connect(findAction, &QAction::triggered, this, &MainWindow::focusSearch);

    connect(sortByIdAction, &QAction::triggered, this, [this]() {
        sortTable(0, Qt::AscendingOrder);
    });
    connect(sortBySurnameAction, &QAction::triggered, this, [this]() {
        sortTable(1, Qt::AscendingOrder);
    });
    connect(sortByNameAction, &QAction::triggered, this, [this]() {
        sortTable(2, Qt::AscendingOrder);
    });
    connect(sortByEmailAction, &QAction::triggered, this, [this]() {
        sortTable(4, Qt::AscendingOrder);
    });
    connect(sortByDateAction, &QAction::triggered, this, [this]() {
        sortTable(5, Qt::AscendingOrder);
    });

    connect(dbSettingsAction, &QAction::triggered, this, &MainWindow::onDatabaseSettings);
    connect(dbStatsAction, &QAction::triggered, this, &MainWindow::onShowStatistics);
    connect(refreshDBAction, &QAction::triggered, this, &MainWindow::onRefresh);

    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupConnections()
{
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoad);
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::onExport);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::onRefresh);

    connect(searchLineEdit, &QLineEdit::returnPressed, this, &MainWindow::onSearch);
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableDoubleClick);
    connect(tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(tableWidget->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateButtonStates);

    // Подключение сортировки при клике на заголовок
    connect(tableWidget->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MainWindow::onHeaderClicked);
}

void MainWindow::loadContactsFromDatabase()
{
    try {
        QList<Contact> contacts = ContactDAO::instance().getAllContacts();
        contactBook.getAllContacts() = contacts; // Синхронизируем с локальным хранилищем
        refreshTable(contacts);
        statusBar()->showMessage(QString("Загружено %1 контактов из базы данных").arg(contacts.size()), 3000);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка",
            QString("Не удалось подключиться к базе данных:\n%1\nРабота в режиме файлов.").arg(e.what()));
        useDatabase = false;
        refreshTable();
    }
}

void MainWindow::onHeaderClicked(int logicalIndex)
{
    Qt::SortOrder newOrder = Qt::AscendingOrder;

    // Если кликнули на ту же колонку, меняем порядок сортировки
    if (currentSortColumn == logicalIndex) {
        newOrder = (currentSortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        // Если кликнули на другую колонку, сбрасываем на сортировку по возрастанию
        currentSortColumn = logicalIndex;
        newOrder = Qt::AscendingOrder;
    }

    currentSortOrder = newOrder;

    // Выполняем сортировку
    sortTable(logicalIndex, newOrder);

    // Обновляем индикатор сортировки в заголовке
    tableWidget->horizontalHeader()->setSortIndicator(logicalIndex, newOrder);
    onRefresh();
}

void MainWindow::sortTable(int column, Qt::SortOrder order)
{
    if (useDatabase) {
        // Для БД сортируем локально
        contactBook.sortByColumn(column, order == Qt::AscendingOrder);
        refreshTable();
    } else {
        // Для файлового режима
        contactBook.sortByColumn(column, order == Qt::AscendingOrder);
        refreshTable();
    }

    // Обновляем статус
    QString columnName;
    switch (column) {
        case 0: columnName = "ID"; break;
        case 1: columnName = "Фамилии"; break;
        case 2: columnName = "Имени"; break;
        case 3: columnName = "Отчеству"; break;
        case 4: columnName = "Email"; break;
        case 5: columnName = "Дате рождения"; break;
        case 6: columnName = "Телефонам"; break;
        default: columnName = "ID"; break;
    }

    QString orderText = (order == Qt::AscendingOrder) ? "по возрастанию" : "по убыванию";
    statusBar()->showMessage(QString("Сортировка по %1 %2").arg(columnName).arg(orderText), 3000);
}

void MainWindow::refreshTable()
{
    QList<Contact> contacts = contactBook.getAllContacts();

    // Сохраняем текущую позицию прокрутки и выделение
    int scrollValue = tableWidget->verticalScrollBar()->value();
    int selectedRow = tableWidget->currentRow();
    int selectedId = -1;

    if (selectedRow >= 0) {
        QTableWidgetItem *item = tableWidget->item(selectedRow, 0);
        if (item) {
            selectedId = item->text().toInt();
        }
    }

    tableWidget->setRowCount(contacts.size());
    tableWidget->setSortingEnabled(false); // Отключаем сортировку на время обновления

    for (int i = 0; i < contacts.size(); ++i) {
        const Contact& contact = contacts[i];

        // ID
        QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(contact.getId()));
        idItem->setData(Qt::UserRole, contact.getId());
        idItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(i, 0, idItem);

        // Фамилия
        QTableWidgetItem *surnameItem = new QTableWidgetItem(contact.getSurname());
        tableWidget->setItem(i, 1, surnameItem);

        // Имя
        QTableWidgetItem *nameItem = new QTableWidgetItem(contact.getName());
        tableWidget->setItem(i, 2, nameItem);

        // Отчество
        QTableWidgetItem *patronymicItem = new QTableWidgetItem(contact.getPatronymic());
        tableWidget->setItem(i, 3, patronymicItem);

        // Email
        QTableWidgetItem *emailItem = new QTableWidgetItem(contact.getEmail());
        emailItem->setToolTip(contact.getEmail());
        tableWidget->setItem(i, 4, emailItem);

        // Дата рождения
        QTableWidgetItem *dateItem = new QTableWidgetItem(contact.getBirthDateString());
        dateItem->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(i, 5, dateItem);

        // Телефоны
        QStringList phones = contact.getPhones();
        QString phonesText = phones.join("\n");
        QTableWidgetItem *phoneItem = new QTableWidgetItem(phonesText);
        phoneItem->setToolTip(phonesText);
        tableWidget->setItem(i, 6, phoneItem);
    }

    // Восстанавливаем позицию прокрутки
    tableWidget->verticalScrollBar()->setValue(scrollValue);

    // Восстанавливаем выделение
    if (selectedId >= 0) {
        for (int i = 0; i < tableWidget->rowCount(); ++i) {
            QTableWidgetItem *item = tableWidget->item(i, 0);
            if (item && item->text().toInt() == selectedId) {
                tableWidget->setCurrentCell(i, 0);
                break;
            }
        }
    }

    tableWidget->setSortingEnabled(true);
    updateInfo();
}

void MainWindow::refreshTable(const QList<Contact>& contacts)
{
    // Сохраняем текущую позицию прокрутки
    int scrollValue = tableWidget->verticalScrollBar()->value();

    tableWidget->setRowCount(contacts.size());
    tableWidget->setSortingEnabled(false);

    for (int i = 0; i < contacts.size(); ++i) {
        const Contact& contact = contacts[i];

        tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(contact.getId())));
        tableWidget->setItem(i, 1, new QTableWidgetItem(contact.getSurname()));
        tableWidget->setItem(i, 2, new QTableWidgetItem(contact.getName()));
        tableWidget->setItem(i, 3, new QTableWidgetItem(contact.getPatronymic()));
        tableWidget->setItem(i, 4, new QTableWidgetItem(contact.getEmail()));
        tableWidget->setItem(i, 5, new QTableWidgetItem(contact.getBirthDateString()));

        QStringList phones = contact.getPhones();
        tableWidget->setItem(i, 6, new QTableWidgetItem(phones.join("\n")));
    }

    tableWidget->verticalScrollBar()->setValue(scrollValue);
    tableWidget->setSortingEnabled(true);
    updateInfo(contacts.size());
}

void MainWindow::updateInfo(int foundCount)
{
    int total = contactBook.getContactCount();

    if (foundCount == -1) {
        contactCountLabel->setText(QString("Всего контактов: %1").arg(total));
        searchInfoLabel->clear();
    } else {
        contactCountLabel->setText(QString("Всего контактов: %1").arg(total));
        searchInfoLabel->setText(QString("Найдено: %1").arg(foundCount));
    }

    updateButtonStates();
}

void MainWindow::updateButtonStates()
{
    bool hasSelection = tableWidget->currentRow() >= 0;
    editButton->setEnabled(hasSelection);
    deleteButton->setEnabled(hasSelection);
}

// В методе onAddContact обновляем обработку исключений:
void MainWindow::onAddContact()
{
    ContactDialog dialog(this, false);

    if (dialog.exec() == QDialog::Accepted) {
        Contact newContact = dialog.getContact();

        if (useDatabase) {
            try {
                // Сохраняем в базу данных
                if (ContactDAO::instance().addContact(newContact)) {
                    loadContactsFromDatabase(); // Обновляем из БД
                    statusBar()->showMessage(QString("Контакт %1 добавлен в базу данных").arg(newContact.getFullName()), 3000);
                }
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "Ошибка",
                    QString("Не удалось добавить контакт:\n%1").arg(e.what()));
            }
        } else {
            // Сохраняем в локальное хранилище
            if (contactBook.addContact(newContact)) {
                refreshTable();
                statusBar()->showMessage(QString("Контакт %1 добавлен").arg(newContact.getFullName()), 3000);
            } else {
                QMessageBox::warning(this, "Ошибка",
                    "Не удалось добавить контакт. Возможно, email или телефон уже существуют.");
            }
        }
    }
}

void MainWindow::onEditContact()
{
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите контакт для редактирования");
        return;
    }

    // Получаем ID контакта из таблицы
    QTableWidgetItem *idItem = tableWidget->item(currentRow, 0);
    if (!idItem) {
        QMessageBox::warning(this, "Ошибка", "Не удалось получить ID контакта");
        return;
    }

    int contactId = idItem->text().toInt();

    Contact contactToEdit;
    if (useDatabase) {
        contactToEdit = ContactDAO::instance().getContactById(contactId);
    } else {
        contactToEdit = contactBook.getContactById(contactId);
    }

    if (contactToEdit.getId() == -1) {
        QMessageBox::warning(this, "Ошибка", "Контакт не найден");
        return;
    }

    ContactDialog dialog(this, true);
    dialog.setContact(contactToEdit);

    if (dialog.exec() == QDialog::Accepted) {
        Contact updatedContact = dialog.getContact();
        updatedContact.setId(contactId); // Сохраняем ID

        if (useDatabase) {
            // Обновляем в базе данных
            if (ContactDAO::instance().updateContact(updatedContact)) {
                loadContactsFromDatabase();
                statusBar()->showMessage(QString("Контакт %1 обновлен в базе данных").arg(updatedContact.getFullName()), 3000);
            } else {
                QMessageBox::warning(this, "Ошибка",
                    "Не удалось обновить контакт в базе данных. Возможно, email или телефон уже существуют.");
            }
        } else {
            // Обновляем в локальном хранилище
            if (contactBook.updateContact(updatedContact)) {
                refreshTable();
                statusBar()->showMessage(QString("Контакт %1 обновлен").arg(updatedContact.getFullName()), 3000);
            } else {
                QMessageBox::warning(this, "Ошибка",
                    "Не удалось обновить контакт. Возможно, email или телефон уже существуют.");
            }
        }
    }
}

void MainWindow::onDeleteContact()
{
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Предупреждение", "Выберите контакт для удаления");
        return;
    }

    // Получаем ID контакта
    QTableWidgetItem *idItem = tableWidget->item(currentRow, 0);
    if (!idItem) {
        QMessageBox::warning(this, "Ошибка", "Не удалось получить ID контакта");
        return;
    }

    int contactId = idItem->text().toInt();

    QString surname = tableWidget->item(currentRow, 1)->text();
    QString name = tableWidget->item(currentRow, 2)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение удаления",
        QString("Вы уверены, что хотите удалить контакт %1 %2?").arg(surname).arg(name),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        bool success = false;
        if (useDatabase) {
            success = ContactDAO::instance().deleteContact(contactId);
            if (success) loadContactsFromDatabase();
        } else {
            success = contactBook.deleteContact(contactId);
            if (success) refreshTable();
        }

        if (success) {
            statusBar()->showMessage(QString("Контакт %1 %2 удален").arg(surname).arg(name), 3000);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить контакт");
        }
    }
}

void MainWindow::onSearch()
{
    QString searchText = searchLineEdit->text().trimmed();

    if (searchText.isEmpty()) {
        if (useDatabase) {
            loadContactsFromDatabase();
        } else {
            refreshTable();
        }
        return;
    }

    QList<Contact> results;
    if (useDatabase) {
        // Поиск в базе данных
        results = ContactDAO::instance().searchByName(searchText);
        if (results.isEmpty()) {
            results = ContactDAO::instance().searchBySurname(searchText);
        }
        if (results.isEmpty()) {
            results = ContactDAO::instance().searchByPhone(searchText);
        }
        if (results.isEmpty()) {
            results = ContactDAO::instance().searchByEmail(searchText);
        }
    } else {
        // Поиск в локальном хранилище
        results = contactBook.searchByName(searchText);
        if (results.isEmpty()) {
            results = contactBook.searchBySurname(searchText);
        }
        if (results.isEmpty()) {
            results = contactBook.searchByPhone(searchText);
        }
        if (results.isEmpty()) {
            results = contactBook.searchByEmail(searchText);
        }
    }

    if (results.isEmpty()) {
        QMessageBox::information(this, "Поиск",
            QString("По запросу \"%1\" ничего не найдено").arg(searchText));
        if (useDatabase) {
            loadContactsFromDatabase();
        } else {
            refreshTable();
        }
    } else {
        refreshTable(results);
        statusBar()->showMessage(QString("Найдено %1 контактов").arg(results.size()), 3000);
    }
}

void MainWindow::onSave()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Сохранить телефонную книгу",
        "phonebook.csv",
        "CSV файлы (*.csv);;Текстовые файлы (*.txt);;Все файлы (*.*)"
    );

    if (!filename.isEmpty()) {
        if (contactBook.saveToFile(filename)) {
            statusBar()->showMessage("Файл сохранен: " + filename, 3000);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл");
        }
    }
}

void MainWindow::onLoad()
{
    if (contactBook.getContactCount() > 0) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Подтверждение",
            "Текущие контакты будут заменены. Продолжить?",
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    QString filename = QFileDialog::getOpenFileName(
        this,
        "Загрузить телефонную книгу",
        "",
        "CSV файлы (*.csv);;Текстовые файлы (*.txt);;Все файлы (*.*)"
    );

    if (!filename.isEmpty()) {
        if (contactBook.loadFromFile(filename)) {
            if (useDatabase) {
                // Импортируем загруженные контакты в базу данных
                QList<Contact> contacts = contactBook.getAllContacts();
                ContactDAO::instance().importContacts(contacts);
                loadContactsFromDatabase(); // Обновляем из БД
            } else {
                refreshTable();
            }
            statusBar()->showMessage("Файл загружен: " + filename, 3000);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить файл");
        }
    }
}

void MainWindow::onExport()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Экспорт контактов",
        "contacts_export.csv",
        "CSV файлы (*.csv);;Текстовые файлы (*.txt)"
    );

    if (!filename.isEmpty()) {
        QList<Contact> contacts;
        if (useDatabase) {
            contacts = ContactDAO::instance().getAllContacts();
        } else {
            contacts = contactBook.getAllContacts();
        }

        if (ContactDAO::instance().exportContacts(filename, contacts)) {
            statusBar()->showMessage("Экспорт завершен: " + filename, 3000);
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось экспортировать контакты");
        }
    }
}

void MainWindow::onDatabaseSettings()
{
    DatabaseSettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Проверяем подключение
        try {
            QList<Contact> contacts = ContactDAO::instance().getAllContacts();
            QMessageBox::information(this, "Успех",
                QString("Подключение к базе данных успешно установлено.\nЗагружено %1 контактов.").arg(contacts.size()));
            loadContactsFromDatabase();
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка",
                QString("Не удалось подключиться к базе данных:\n%1").arg(e.what()));
        }
    }
}
void MainWindow::onShowStatistics()
{
    showStatistics();
}

void MainWindow::showStatistics()
{
    if (useDatabase) {
        try {
            QMap<QString, int> byLetter = ContactDAO::instance().getContactsByFirstLetter();
            QMap<int, int> byYear = ContactDAO::instance().getContactsByYear();
            int total = ContactDAO::instance().getContactCount();

            QString statsText = QString("Статистика базы данных:\n\nВсего контактов: %1\n\n").arg(total);

            statsText += "Распределение по первой букве фамилии:\n";
            for (auto it = byLetter.begin(); it != byLetter.end(); ++it) {
                statsText += QString("  %1: %2 контактов\n").arg(it.key()).arg(it.value());
            }

            statsText += "\nРаспределение по году рождения:\n";
            for (auto it = byYear.begin(); it != byYear.end(); ++it) {
                statsText += QString("  %1 год: %2 контактов\n").arg(it.key()).arg(it.value());
            }

            QMessageBox::information(this, "Статистика базы данных", statsText);
        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка",
                QString("Не удалось получить статистику из базы данных:\n%1").arg(e.what()));
        }
    } else {
        QMap<QString, int> byLetter = contactBook.getContactsByFirstLetter();
        QMap<int, int> byYear = contactBook.getContactsByYear();
        int total = contactBook.getContactCount();

        QString statsText = QString("Статистика локального хранилища:\n\nВсего контактов: %1\n\n").arg(total);

        statsText += "Распределение по первой букве фамилии:\n";
        for (auto it = byLetter.begin(); it != byLetter.end(); ++it) {
            statsText += QString("  %1: %2 контактов\n").arg(it.key()).arg(it.value());
        }

        statsText += "\nРаспределение по году рождения:\n";
        for (auto it = byYear.begin(); it != byYear.end(); ++it) {
            statsText += QString("  %1 год: %2 контактов\n").arg(it.key()).arg(it.value());
        }

        QMessageBox::information(this, "Статистика локального хранилища", statsText);
    }
}

void MainWindow::onTableDoubleClick(int row, int column)
{
    Q_UNUSED(column);
    if (row >= 0) {
        onEditContact();
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QTableWidgetItem *item = tableWidget->itemAt(pos);
    if (!item) return;

    int row = item->row();

    QMenu contextMenu(this);

    QAction *editAction = contextMenu.addAction("Редактировать");
    QAction *deleteAction = contextMenu.addAction("Удалить");
    contextMenu.addSeparator();
    QAction *copyIdAction = contextMenu.addAction("Копировать ID");
    QAction *copyNameAction = contextMenu.addAction("Копировать ФИО");
    QAction *copyPhoneAction = contextMenu.addAction("Копировать телефон");
    QAction *copyEmailAction = contextMenu.addAction("Копировать email");

    QAction *selectedAction = contextMenu.exec(tableWidget->viewport()->mapToGlobal(pos));

    if (selectedAction == editAction) {
        tableWidget->setCurrentCell(row, 0);
        onEditContact();
    } else if (selectedAction == deleteAction) {
        tableWidget->setCurrentCell(row, 0);
        onDeleteContact();
    } else if (selectedAction == copyIdAction) {
        QString id = tableWidget->item(row, 0)->text();
        QApplication::clipboard()->setText(id);
        statusBar()->showMessage("ID скопирован в буфер обмена", 2000);
    } else if (selectedAction == copyNameAction) {
        QString surname = tableWidget->item(row, 1)->text();
        QString name = tableWidget->item(row, 2)->text();
        QString patronymic = tableWidget->item(row, 3)->text();
        QString fullName = surname + " " + name + " " + patronymic;
        QApplication::clipboard()->setText(fullName);
        statusBar()->showMessage("ФИО скопировано в буфер обмена", 2000);
    } else if (selectedAction == copyPhoneAction) {
        QString phone = tableWidget->item(row, 6)->text();
        QApplication::clipboard()->setText(phone);
        statusBar()->showMessage("Телефон скопирован в буфер обмена", 2000);
    } else if (selectedAction == copyEmailAction) {
        QString email = tableWidget->item(row, 4)->text();
        QApplication::clipboard()->setText(email);
        statusBar()->showMessage("Email скопирован в буфер обмена", 2000);
    }
}

void MainWindow::focusSearch()
{
    searchLineEdit->setFocus();
    searchLineEdit->selectAll();
}

void MainWindow::showAbout()
{
    QString modeText = useDatabase ? "База данных" : "Локальные файлы";
    QMessageBox::about(this, "О программе",
        "<h3>Телефонная книга</h3>"
        "<p>Версия 3.0</p>"
        "<p>Программа для управления контактами с поддержкой:</p>"
        "<ul>"
        "<li>Базы данных PostgreSQL</li>"
        "<li>Локальных файлов (CSV)</li>"
        "<li>Импорта/экспорта данных</li>"
        "<li>Проверки корректности данных</li>"
        "</ul>"
        "<p>Текущий режим работы: <b>" + modeText + "</b></p>"
        "<p>Переключение режима доступно через кнопку внизу окна</p>");
}

void MainWindow::onRefresh()
{
    if (useDatabase) {
        loadContactsFromDatabase();
        statusBar()->showMessage("Данные обновлены из базы данных", 3000);
    } else {
        refreshTable();
        statusBar()->showMessage("Данные обновлены", 3000);
    }
}
