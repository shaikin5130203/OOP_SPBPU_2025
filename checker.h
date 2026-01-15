#include <string>
#include <vector>
#pragma once
using namespace std;
extern vector<string> unique_numbers;
extern vector<string> unique_emails;
string trim_unique_number(string number);//Приводит все вводимые номера под единый формат, подходящий для проверки их уникальности
bool check_name(string& name);//Проверяет общее соответствие формату
bool check_patronymic(string& name);
bool trim_patronymic(string& patr);
bool check_number(string& number);
bool check_email(string& email);
bool check_date_format(string& date);
bool trim_name(string& name);//Окончательно подгоняет введенные данные под формат, убирая лишние пробелы и т.д.
bool trim_number(string& number);
bool trim_email(string& email);
bool trim_date_format(string& date);
bool check_date(string& date);
