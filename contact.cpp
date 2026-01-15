#include <iostream>
#include "checker.h"
#include <vector>
#include "contact.h"
using namespace std;
extern vector<string> unique_numbers;
extern vector<string> unique_emails;
void contact::clear_numbers(){
numbers.clear();
return;
}
contact::~contact(){
name ="";
surname="";
patr="";
email="";
birth_date = "";
numbers.clear();
return;
}
bool contact::operator==(const contact& A){
return (name == A.name && surname == A.surname && patr == A.patr && birth_date == A.birth_date && email == A.email);
}
string contact::get_name(){
return name;
}
bool contact::set_name(string name){
if (trim_name(name)){
    this->name = name;
    return 1;
} return 0;
}
 string contact::get_surname(){
return surname;
}
bool contact::set_surname(string surname){
if (trim_name(surname)){
    this->surname = surname;
    return 1;
} return 0;
}
string contact::get_patr(){
return patr;
}
bool contact::set_patr(string patr){
if (trim_patronymic(patr)){
    this->patr = patr;
    return 1;
} return 0;
}
string contact::get_email(){
return email;
}
bool contact::set_email(string email){
    if (trim_email(email)){
        for (long long unsigned int i = 0; i < unique_emails.size(); i++){
            if(email == unique_emails[i]){
                cout << "Email already exists\n";
                return 0;
            }
        }
        unique_emails.push_back(email);
        this->email = email;
        return 1;
} return 0;
}
string contact::get_birth_date(){
return birth_date;
}
bool contact::set_birth_date(string birth_date){
    if (check_date(birth_date)){
    this->birth_date = birth_date;
    return 1;
} return 0;
}
vector<string> contact::get_numbers(){
return numbers;
}
bool contact::add_number(string number){
if (trim_number(number)){
    for (long long unsigned int i = 0; i < unique_numbers.size(); i++){
        if (trim_unique_number(number) == unique_numbers[i]){
            cout << "Number already exists\n";
            return 0;
        }
        }
    unique_numbers.push_back(trim_unique_number(number));
    numbers.push_back(number);
    return 1;
} return 0;
}
void contact::set_numbers(vector<string> numbers){
this->numbers = numbers;
return;
}
void contact::print_contact(){
cout << surname <<" "<<name<<" "<<patr<<" "<<birth_date<<" "<<email<<"\nNumbers: \n";
for (long long unsigned int i = 0; i < numbers.size(); i++){
    cout << numbers[i]<<"\n";
}
return;
}