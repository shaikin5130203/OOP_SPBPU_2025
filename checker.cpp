#include "checker.h"
#include <string>
#include <regex>
#include <cstdio>
#include <ctime>
using namespace std;
vector<string> unique_numbers;
vector<string> unique_emails;
regex trim_pattern(R"(^\s+|\s+$)");
 string trim_unique_number(string number){
    number = regex_replace(number, regex(R"(\D)"), "");
    number = regex_replace(number, regex(R"(^(\+?7|8))"), "");
    return number;
}
bool check_name(string& name){
regex name_pattern(R"(^[A-ZА-Я]{1}[a-zа-я0-9]+(?:[\s-][A-ZА-Я]{1}[a-zа-я0-9]+)*$)"); //Русские буквы проходят проверку, но только до буквы п, не понимаю почему
return regex_match(name, name_pattern);
}
bool check_patronymic(string& name){
regex name_pattern(R"(^[A-ZА-Я]{1}[a-zа-я0-9]+(?:[\s-][A-ZА-Я]{1}[a-zа-я0-9]+)*$)");
regex none_patr_pattern("-");
return (regex_match(name, name_pattern)||regex_match(name, none_patr_pattern));
}
bool check_number(string& number){
    regex number_pattern(R"(^\s*\+{0,1}[0-9]{1}(\({0,1}[0-9]{3}\){0,1})?[0-9]{3}-{0,1}[0-9]{2}-{0,1}[0-9]{2}\s*$)");
    return regex_match(number, number_pattern);
}
bool check_email(string& email){
    regex email_pattern(R"(^\s*[a-z0-9-]+\s*@\s*[a-z0-9]+.[a-z]{2,3}\s*$)");
    return regex_match(email, email_pattern);
}
bool check_date_format(string& date){
    regex date_pattern(R"(^\s*[0-9]{2}\s*.\s*[0-9]{2}\s*.\s*[0-9]{4}\s*$)");
    return regex_match(date, date_pattern);
}
bool trim_name(string& name){
    if(check_name(name)){
    name = regex_replace(name, trim_pattern, "");
       return 1;
    } return 0;
}
bool trim_patronymic(string& patr){
    if(check_patronymic(patr)){
    patr = regex_replace(patr, trim_pattern, "");
       return 1;
    } return 0;
}
bool trim_number(string& number){
if(check_number(number)){
       number = regex_replace(number, trim_pattern, "");
       return 1;
    } return 0;
}
bool trim_email(string& email){
    if(check_email(email)){
        regex email_trim_pattern(R"(\s*@\s*)");
        email = regex_replace(email, trim_pattern, "");
        email = regex_replace(email, email_trim_pattern, "@");
        return 1;
    }return 0;
}
bool trim_date_format(string& date){
    if(check_date_format(date)){
        regex date_trim_pattern(R"(\s*)");
        date = regex_replace(date, date_trim_pattern, "");
        return 1;
    } return 0;
}
bool check_date(string& date){
    if(trim_date_format(date)){
        time_t current_s = time(0);
        tm* current = localtime(&current_s);
        int day, month, year;
        bool is_short_month = 0, is_february = 0, is_leap = 0;
        sscanf(date.c_str(), "%d.%d.%d", &day, &month, &year);
        if(day > 31 || month > 12){
            return 0;
        }
        if(year-1970 > current->tm_year || (month > current->tm_mon && year-1970==current->tm_year) || (day > current->tm_mday && month == current->tm_mon && year-1970==current->tm_year)){
            return 0;
        }
        if(month==2){
        is_february = 1;
        }
        if((year%4 == 0 && year%100!=0) || (year%100==0 && year%400 == 0)){
            is_leap = 1;
        }
        if ((month > 7 && month%2 == 1) || (month <= 7 && month%2 == 0)){
            is_short_month = 1;
        }
        if ((is_short_month && day > 30) || (is_february && is_leap && day > 29) || (is_february && !is_leap && day > 28)){
            return 0;
        }
        return 1;
    } return 0;
}