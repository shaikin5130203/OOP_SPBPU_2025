#include <string>
#include "checker.h"
#include <vector>
#pragma once
using namespace std;
class contact{
    string name;
    string surname;
    string patr;
    vector<string> numbers;
    string email;
    string birth_date;
    public:
    void clear_numbers();
    ~contact();
    bool operator==(const contact& A);
    string get_name();
    bool set_name(string name);
    string get_surname();
    bool set_surname(string surname);
    string get_patr();
    bool set_patr(string patr);
    string get_email();
    bool set_email(string email);
    string get_birth_date();
    bool set_birth_date(string birth_date);
    vector<string> get_numbers();
    bool add_number(string number);
    void set_numbers(vector<string> numbers);
    void print_contact();
};