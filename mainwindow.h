#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "contactbook.h"
#include "contactdao.h"
#include "databasesettingsdialog.h"

QT_BEGIN_NAMESPACE
class QTableWidget;
class QLineEdit;
class QPushButton;
class QLabel;
class QGroupBox;
class QAction;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    void onSearch();
    void onSave();
    void onLoad();
    void onExport();
    void onTableDoubleClick(int row, int column);
    void onHeaderClicked(int logicalIndex);
    void focusSearch();
    void showAbout();
    void showContextMenu(const QPoint &pos);
    void updateButtonStates();
    void onDatabaseSettings();
    void onShowStatistics();
    void onRefresh();

private:
    ContactBook contactBook; // Для работы с файлами
    bool useDatabase; // Флаг использования БД

    // UI элементы
    QTableWidget *tableWidget;
    QLineEdit *searchLineEdit;
    QLabel *contactCountLabel;
    QLabel *searchInfoLabel;

    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *saveButton;
    QPushButton *loadButton;
    QPushButton *exportButton;
    QPushButton *refreshButton;

    // Вспомогательные методы
    void setupUI();
    void setupTable();
    void setupMenu();
    void setupConnections();
    void refreshTable();
    void refreshTable(const QList<Contact>& contacts);
    void updateInfo(int foundCount = -1);
    void loadContactsFromDatabase();
    void showStatistics();

    // Сортировка
    void sortTable(int column, Qt::SortOrder order);
    int currentSortColumn;
    Qt::SortOrder currentSortOrder;
};

#endif // MAINWINDOW_H
