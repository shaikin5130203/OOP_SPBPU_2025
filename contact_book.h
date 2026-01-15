#include<vector>
#include"contact.h"
#pragma once
using namespace std;
extern bool is_saved;//От значения переменной зависит, будет ли при выходе из программы появляться предложение сохранить изменения
class contact_book{
vector<contact> contacts;
public:
vector<contact*> vector_of_pointers(const vector<contact>& vect);//Превращает вектор объектов класса contact в вектор указателей на эти объекты
vector<contact> vector_of_objects(const vector<contact*>& vect);//То же самое, но наоборот
void add_contact();
vector<contact*> search_contact_name(const string& name, vector<contact*>& contacts);
vector<contact*> search_contact_surname(const string& surname, vector<contact*>& contacts);
vector<contact*> search_contact_patr(const string& patr, vector<contact*>& contacts);
vector<contact*> search_contact_email(const string& email, vector<contact*>& contacts);
void find_contact();//Процедура поиска пользователя напрямую из главного меню с его выводом на терминал
vector<contact> non_void_find_contact(const vector<contact>& contacts);//Вспомогательная функция поиска для других процедур, таких как удаление или редактирование контакта
void save();
void print_all();
void load();
void delete_contact();
contact* search_to_edit(const vector<contact>& contacts);
void edit_contact();
};