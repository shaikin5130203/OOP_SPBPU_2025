#include "contact_book.h"
#include "contact.h"
#include "checker.h"
#include <iostream>
using namespace std;
bool is_saved = 1;
vector<contact*> contact_book::vector_of_pointers(const vector<contact>& vect){
    vector<contact*> res;
        for(long long unsigned int i = 0; i < vect.size(); i++){
        res.push_back(&contacts[i]);
    }
    return res;
}
vector<contact> contact_book::vector_of_objects(const vector<contact*>& vect){
    vector<contact> res;
        for(long long unsigned int i = 0; i < vect.size(); i++){
        res.push_back(contacts[i]);
    }
    return res;
}
void contact_book::add_contact(){
bool is_invalid = 1;
contact new_contact;
string name, surname, patr, b_date, email, number;
char choice = 'y';
cout << "Name, surname and patronymic must start with a capital letter\n";
while(is_invalid){
cout <<"Enter the name: ";
cin >> name;
if (!new_contact.set_name(name)){
    cout << "Invalid name\n";
} else is_invalid = 0;
}
is_invalid = 1;
while(is_invalid){
    is_invalid = 1;
cout <<"Enter the surname: ";
cin >> surname;
if (!new_contact.set_surname(surname)){
    cout << "Invalid surname\n";
} else is_invalid = 0;
}
is_invalid = 1;
while(is_invalid){
    is_invalid = 1;
cout <<"Enter the patronymic (if none, type -): ";
cin >> patr;
if (!new_contact.set_patr(patr)){
    cout << "Invalid patronymic\n";
} else is_invalid = 0;
}
is_invalid = 1;
while(is_invalid){
cout <<"Enter the birth date (dd.mm.yyyy): ";
cin >> b_date;
if (!new_contact.set_birth_date(b_date)){
    cout << "Invalid birth date\n";
} else is_invalid = 0;
}
is_invalid = 1;
while(is_invalid){
cout <<"All letters the email must be lowercase\n";
cout <<"Enter the email: ";
cin >> email;
if (!new_contact.set_email(email)){
    cout << "Invalid email\n";
} else is_invalid = 0;
}
is_invalid = 1;
while(choice == 'Y'||choice == 'y'){
choice = 'a';
is_invalid = 1;
while(is_invalid){
cout <<"Enter the phone number: ";
cin >> number;
if (!new_contact.add_number(number)){
    cout << "Invalid number\n";
} else is_invalid = 0;
}
while (choice != 'Y' && choice != 'y' && choice != 'N' && choice != 'n'){
cout << "Do you want to add an extra number? (Y/N)\n";
cin >> choice;
}
}
contacts.push_back(new_contact);
cout <<"Contact added\n";
}
vector<contact*> contact_book::search_contact_name(const string& name, vector<contact*>& contacts){
vector<contact*> found;
for(long long unsigned int i = 0; i < contacts.size(); i++){
if(contacts[i]->get_name() == name){
found.push_back(contacts[i]);
}
} 
return found;
}
vector<contact*> contact_book::search_contact_surname(const string& surname, vector<contact*>& contacts){
vector<contact*> found;
for(long long unsigned int i = 0; i < contacts.size(); i++){
if(contacts[i]->get_surname() == surname){
found.push_back(contacts[i]);
}
} 
return found;
}
vector<contact*> contact_book::search_contact_patr(const string& patr, vector<contact*>& contacts){
vector<contact*> found;
for(long long unsigned int i = 0; i < contacts.size(); i++){
if(contacts[i]->get_patr() == patr){
found.push_back(contacts[i]);
}
} 
return found;
}
vector<contact*> contact_book::search_contact_email(const string& email, vector<contact*>& contacts){
vector<contact*> found;
for(long long unsigned int i = 0; i < contacts.size(); i++){
if(contacts[i]->get_email() == email){
found.push_back(contacts[i]);
}
} 
return found;
}
void contact_book::find_contact(){
    int choice = 0;
    char choice_2 = 'y';
    vector<contact*> found = vector_of_pointers(contacts);
    while ((choice_2=='y' || choice_2=='Y')){
    cout << "How do you want to search?\n1. By name\n2. By surname\n3. By patronymic\n4. By Email\n5. Back\n";
    cin >> choice;
    switch(choice){
        case 1:{
        string name;
        cout << "Enter the name: ";
        cin>>name;
        if (search_contact_name(name, found).size()==0){
            cout << "Not found\n";
            break;
        }
        found = search_contact_name(name,found);
        for(long long unsigned int i = 0; i < search_contact_name(name,found).size(); i++){
        search_contact_name(name,found)[i]->print_contact();
        }
        break;
        }
        case 2:{
            string surname;
        cout << "Enter the surname: ";
        cin>>surname;
        if (search_contact_surname(surname, found).size()==0){
            cout << "Not found\n";
            break;
        }
        found = search_contact_surname(surname,found);
        for(long long unsigned int i = 0; i < search_contact_surname(surname, found).size(); i++){
        search_contact_surname(surname, found)[i]->print_contact();
        }
        break;
        }
        case 3:{
            string patr;
        cout << "Enter the patronymic: ";
        cin>>patr;
        if (search_contact_patr(patr, found).size()==0){
            cout << "Not found\n";
            break;
        }
        found = search_contact_patr(patr,found);
        for(long long unsigned int i = 0; i < search_contact_patr(patr, found).size(); i++){
        search_contact_patr(patr, found)[i]->print_contact();
        }
        break;
        } 
        case 4:{
            string email;
        cout << "Enter the email: ";
        cin>>email;
        if (search_contact_email(email, found).size()==0){
            cout << "Not found\n";
            break;
        }
        found = search_contact_email(email,found);
       for(long long unsigned int i = 0; i < search_contact_email(email, found).size(); i++){
        search_contact_patr(email, found)[i]->print_contact();
        }
        break;
        }
        default: return;
    }
    cout << "Do you want to add another filter? (Y/N)\n";
    cin >> choice_2;
}
}
vector<contact> contact_book::non_void_find_contact(const vector<contact>& contacts){
    int choice = 0;
    char choice_2 = 'y';
    vector<contact*> found = vector_of_pointers(contacts);
    while ((choice_2=='y' || choice_2=='Y')){
    cout << "How do you want to search?\n1. By name\n2. By surname\n3. By patronymic\n4. By Email\n5. Back\n";
    cin >> choice;
    switch(choice){
        case 1:{
        string name;
        cout << "Enter the name: ";
        cin>>name;
        if (search_contact_name(name, found).size()==0){
            cout << "Not found\n";
            break;
        }
        found = search_contact_name(name,found);
        for(long long unsigned int i = 0; i < search_contact_name(name,found).size(); i++){
        search_contact_name(name,found)[i]->print_contact();
        }
        break;
        }
        case 2:{
            string surname;
        cout << "Enter the surname: ";
        cin>>surname;
        if (search_contact_surname(surname, found).size()==0){
            cout << "Not found\n";
            break;
        }
        found = search_contact_surname(surname,found);
        for(long long unsigned int i = 0; i < search_contact_surname(surname, found).size(); i++){
        search_contact_surname(surname, found)[i]->print_contact();
        }
        break;
        }
        case 3:{
            string patr;
        cout << "Enter the patronymic: ";
        cin>>patr;
        if (search_contact_patr(patr, found).size()==0){
            cout << "Not found\n";
            break;
        }
        found = search_contact_patr(patr,found);
        for(long long unsigned int i = 0; i < search_contact_patr(patr, found).size(); i++){
        search_contact_patr(patr, found)[i]->print_contact();
        }
        break;
        } 
        case 4:{
            string email;
        cout << "Enter the email: ";
        cin>>email;
        if (search_contact_email(email, found).size()==0){
            cout << "Not found\n";
            break;
        }
        found = search_contact_email(email,found);
       for(long long unsigned int i = 0; i < search_contact_email(email, found).size(); i++){
        search_contact_patr(email, found)[i]->print_contact();
        }
        break;
        }
        default: {

            found.clear();
            return vector_of_objects(found);
        }
    }
    cout << "Do you want to add another filter? If you select No, all shown contacts will be deleted. (Y/N)\n";
    cin >> choice_2;
    if(choice_2!='Y' && choice_2!='y'){
        return vector_of_objects(found);
    }
}
    found.clear();
    return vector_of_objects(found);
}
void contact_book::save(){
    FILE* f;
    f = fopen("book.txt", "w");
    if (f!=NULL){
    for(long long unsigned int i =0; i < contacts.size(); i++){
        fprintf(f, "%s %s %s %s %s\n", contacts[i].get_surname().c_str(), contacts[i].get_name().c_str(), contacts[i].get_patr().c_str(), contacts[i].get_birth_date().c_str(), contacts[i].get_email().c_str());
        for (long long unsigned int j = 0; j<contacts[i].get_numbers().size();j++){
            fprintf(f, "%s\n", contacts[i].get_numbers()[j].c_str());
        }
        fprintf(f, "%s", "0\n");
        if(i!=contacts.size()-1){fprintf(f, "%s", "01\n");}
    }
    fprintf(f, "%s", "00\n");
    fclose(f);
    f = fopen("numbers.txt", "w");
    for(long long unsigned int i =0; i < unique_numbers.size(); i++){
        fprintf(f, "%s\n", unique_numbers[i].c_str());
    }
    fclose(f);
        f = fopen("emails.txt", "w");
    for(long long unsigned int i =0; i < unique_emails.size(); i++){
        fprintf(f, "%s\n", unique_emails[i].c_str());
    }
    fclose(f);
    cout << "Saved!\n";
    is_saved = 1;
    return;
}  else {
    cout << "Error with file opening\n";
    fclose(f);
    return;
}
}
void contact_book::print_all(){
    if(contacts.size()==0){
        cout << "Nothing to show\n";
        return;
    }
    int choice = 0;
    vector<contact*> sorted = vector_of_pointers(contacts);
    auto n = sorted.size();
    cout << "Sort by:\n1. Name\n2. Surname\n3. Patronymic\n4. Email\n5. Birth date\n";
    cin >> choice;
    switch(choice){
        case 1:{
            for (long long unsigned int i = 0; i < n - 1; i++){
                auto m_ind = i;
                for (auto j = i + 1; j < n; j++){
                    if (sorted[j]->get_name() < sorted[m_ind]->get_name()){
                        swap(sorted[j], sorted[m_ind]);
                    }
                }
            }
            break;
        }
        case 2:{
            for (long long unsigned int i = 0; i < n - 1; i++){
                auto m_ind = i;
                for (auto j = i + 1; j < n; j++){
                    if (sorted[j]->get_surname() < sorted[m_ind]->get_surname()){
                        swap(sorted[j], sorted[m_ind]);
                    }
                }
            }
            break;
        }
        case 3:{
            for (long long unsigned int i = 0; i < n - 1; i++){
                auto m_ind = i;
                for (auto j = i + 1; j < n; j++){
                    if (sorted[j]->get_patr() < sorted[m_ind]->get_patr()){
                        swap(sorted[j], sorted[m_ind]);
                    }
                }
            }
            break;
        }
        case 4:{
            for (long long unsigned int i = 0; i < n - 1; i++){
                auto m_ind = i;
                for (auto j = i + 1; j < n; j++){
                    if (sorted[j]->get_email() < sorted[m_ind]->get_email()){
                        swap(sorted[j], sorted[m_ind]);
                    }
                }
            }
            break;
        }
        case 5:{
            int day, month, year, day1, month1, year1;
            for (long long unsigned int i = 0; i < n - 1; i++){
                auto m_ind = i;
                for (auto j = i + 1; j < n; j++){
                    sscanf(sorted[j]->get_birth_date().c_str(), "%d.%d.%d", &day, &month, &year);
                    sscanf(sorted[m_ind]->get_birth_date().c_str(), "%d.%d.%d", &day1, &month1, &year1);
                    if ((year < year1) || (year == year1 && month < month1) || (year == year1 && month == month1 && day < day1)){
                        swap(sorted[j], sorted[m_ind]);
                    }
                }
            }
            break;
        }
    }
      for (long long unsigned int i = 0;i < sorted.size(); i++){
        sorted[i]->print_contact();
      }
      return;
}
void contact_book::load(){
    vector<contact> loaded;
    vector<contact> buf_contacts = contacts;
    vector<string> buf_emails = unique_emails;
    vector<string> buf_numbers = unique_numbers;
    unique_emails.clear();
    unique_numbers.clear();
    contacts.clear();
    char nul[10];
    string nul1="Aa";
    FILE* f = fopen("book.txt", "r");
    if (f!=NULL){
        char b_name[255], b_surname[255],b_patr[255],b_b_date[255],b_email[255],b_number[255];
        string name, surname, patr, b_date, email, number;
        while(nul1!="00"){
        contact loading_contact;
        number = "1";
        fscanf(f, "%s %s %s %s %s\n", b_surname, b_name, b_patr, b_b_date, b_email);
        surname = b_surname;
        name = b_name;
        patr = b_patr;
        b_date = b_b_date;
        email = b_email;
        if(!loading_contact.set_surname(surname)||!loading_contact.set_name(name)||!loading_contact.set_patr(patr)||!loading_contact.set_birth_date(b_date)||!loading_contact.set_email(email)){
            cout << "Save is corrupted or empty 1\n";
            return;
        }
        while(number!="0"){
            fscanf(f, "%s\n", b_number);
            number = b_number;
            if(!loading_contact.add_number(number)&&number!="0"){
            cout << "Save is corrupted or empty 2\n";
            return;
            }
        }
        fscanf(f, "%s\n", nul);
        nul1 = nul;
        loaded.push_back(loading_contact);
    }
}
fclose(f);
if(loaded.size()==0){
    cout<< "There is no saved file\n";
    contacts = buf_contacts;
    unique_emails = buf_emails;
    unique_numbers = buf_numbers;
    return; 
}
contacts = loaded;
cout << "Loaded!\n";
return;
}
void contact_book::delete_contact(){
vector<contact> found = non_void_find_contact(contacts);
if(found.empty()){
    return;
}

for (long long unsigned int i = 0; i < unique_emails.size(); i++){
    for (long long unsigned int j = 0; j < found.size(); j++){
        if (unique_emails[i] == found[j].get_email()){
            unique_emails.erase(unique_emails.begin()+i);
        }
    }
}
for (long long unsigned int i = 0; i < unique_numbers.size(); i++){
    for (long long unsigned int j = 0; j < found.size(); j++){
        for(long long unsigned int k = 0; k < found[j].get_numbers().size(); k++){
        if (unique_numbers[i] == found[j].get_numbers()[k]){
            unique_numbers.erase(unique_numbers.begin()+i);
        }
    }
    }
}
for (long long unsigned int i = 0; i < contacts.size(); i++){
    for (long long unsigned int j = 0; j < found.size(); j++){
        if (contacts[i] == found[j]){

            contacts.erase(contacts.begin()+i);
        }
    }
}
cout << "Deleted!\n";
return;
} 
contact* contact_book::search_to_edit(const vector<contact>& contacts){
    int choice = 0;
    char choice_2 = 'y';
    vector<contact*> found = vector_of_pointers(contacts);
    while (choice_2=='y' || choice_2=='Y' || found.size() != 1){
    cout << "How do you want to search?\n1. By name\n2. By surname\n3. By patronymic\n4. By Email\n5. Back\n";
    cin >> choice;
    switch(choice){
        case 1:{
        string name;
        cout << "Enter the name: ";
        cin>>name;
        if (search_contact_name(name, found).size()==0){
            cout << "Nothing found\n";
            break;
        }
        found = search_contact_name(name,found);
        for(long long unsigned int i = 0; i < search_contact_name(name,found).size(); i++){
        search_contact_name(name,found)[i]->print_contact();
        }
        break;
        }
        case 2:{
            string surname;
        cout << "Enter the surname: ";
        cin>>surname;
        if (search_contact_surname(surname, found).size()==0){
            cout << "Nothing found\n";
            break;
        }
        found = search_contact_surname(surname,found);
        for(long long unsigned int i = 0; i < search_contact_surname(surname, found).size(); i++){
        search_contact_surname(surname, found)[i]->print_contact();
        }
        break;
        }
        case 3:{
            string patr;
        cout << "Enter the patronymic: ";
        cin>>patr;
        if (search_contact_patr(patr, found).size()==0){
            cout << "Nothing found\n";
            break;
        }
        found = search_contact_patr(patr,found);
        for(long long unsigned int i = 0; i < search_contact_patr(patr, found).size(); i++){
        search_contact_patr(patr, found)[i]->print_contact();
        }
        break;
        } 
        case 4:{
            string email;
        cout << "Enter the email: ";
        cin>>email;
        if (search_contact_email(email, found).size()==0){
            cout << "Nothing found\n";
            break;
        }
        found = search_contact_email(email,found);
       for(long long unsigned int i = 0; i < search_contact_email(email, found).size(); i++){
        search_contact_patr(email, found)[i]->print_contact();
        }
        break;
        }
        default: {
            return NULL;
            break;
        }
        return NULL;
    }
    if(found.size() > 1){
        cout << "You need to add filters until there is one contact left\n";
        continue;
    } else if(found.size()==1) {return found[0];}
}
    return found[0];

}
void contact_book::edit_contact(){
    contact* found = search_to_edit(contacts);
    int choice;
    if (found != NULL){
    cout << "What do you want to edit?\n1. Name\n2. Surname\n3. Patronymic\n4. Birth date\n5. Email\n6. Numbers\n7. Nothing\n";
    cin >> choice;
    switch(choice){
        case 1:{
            string name;
            bool is_valid = 0;
            while(!is_valid){
            cout <<"Enter new name: ";
            cin >> name;
            if(!found->set_name(name)){
                cout<<"Invalid name\n";
                continue;
            }
            is_valid = 1;
            }
            break;
        }
        case 2:{
            string surname;
            bool is_valid = 0;
            while(!is_valid){
            cout <<"Enter new surname: ";
            cin >> surname;
            if(!found->set_surname(surname)){
            cout<<"Invalid surname\n";
            continue;
            }
            is_valid = 1;
            }
            break;
        }
        case 3:{
            string patr;
            bool is_valid = 0;
            while(!is_valid){
            cout <<"Enter new patronymic: ";
            cin >> patr;
            if(!found->set_patr(patr)){
                cout<<"Invalid patronymic\n";
                continue;
            }
            is_valid = 1;
            }
            break;
        }
        case 4:{
            string date;
            bool is_valid = 0;
            while(!is_valid){
            cout <<"Enter new birth date: ";
            cin >> date;
            if(!found->set_birth_date(date)){
            cout<<"Invalid date\n";
            continue;
            }
            is_valid = 1;
            }
            break;
        }
        case 5:{
            string email;
            bool is_valid = 0;
            while(!is_valid){
            cout <<"Enter new email: ";
            cin >> email;
           if(!found->set_email(email)){
            cout<<"Invalid email\n";
            continue;
            }
            is_valid = 1;
                for(long long unsigned int i = 0; i < unique_emails.size();i++){
                if(found->get_email()==unique_emails[i]){
                    unique_emails.erase(unique_emails.begin() + i);
                    break;
                }
            }
            unique_emails.push_back(email);
            }
            break;
        }
        case 6:{
            int choice_1;
            cout <<"1. Add number\n2. Delete number\n";
            cin >> choice_1;
            switch(choice_1){
                case 1:{
                string number;
                bool is_valid = 0;
                while(!is_valid){
                cout <<"Enter new number: ";
                cin >> number;
                if(!found->add_number(number)){
                cout<<"Invalid number\n";
                continue;
                }
                is_valid = 1;
                unique_numbers.push_back(trim_unique_number(number));
                }
                break;
            }
                case 2:{
                    if (found->get_numbers().size()==1){
                        cout << "Contact must have at least one number\n";
                        return;
                        break;
                    }
                    int choice_1;
                    long long unsigned int choice_2;
                    vector<string> boof;
                    bool is_valid = 0;
                    for(long long unsigned int i = 1; i<=found->get_numbers().size(); i++){
                        cout << i <<". "<<found->get_numbers()[i-1]<<"\n";
                    }
                    while (!is_valid){
                    cout <<"What number do you want to delete?: ";
                    cin >> choice_1;
                    if(choice_1 < 1){
                        cout << "Invalid choice\n";
                        continue;
                    }
                    choice_2 = static_cast<long long unsigned int>(choice_1);
                    if (choice_2 > found->get_numbers().size()){
                        cout << "Invalid choice\n";
                        continue;
                    }
                    is_valid = 1;
                    }
                    for(long long unsigned int i = 0; i < unique_numbers.size();i++){
                if(found->get_numbers()[choice_2-1]==unique_numbers[i]){
                    unique_numbers.erase(unique_numbers.begin() + i);
                    break;
                }
            }
                    boof = found->get_numbers();
                    boof.erase(boof.begin() + choice_2 - 1);
                    found->set_numbers(boof);
                    
                    break;
                }
                default: {
                    return;
                    break;
                }
            }
        }
    }
    cout << "Edited!\n";
    return;
}
}
