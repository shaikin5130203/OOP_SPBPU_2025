#include "menu.h"
#include<iostream>
#include "contact_book.h"
using namespace std;
void menu(){
contact_book book;
int choice = 0;
while(choice!=8){
cout<< "1. Add contact\n2. Delete contact\n3. Edit contact\n4. Print all contacts\n5. Search contact\n6. Save\n7. Load\n8. Exit\n";
cin >> choice;
switch(choice){
    case 1:{
    book.add_contact();
    is_saved = 0;
    break;
    }
    case 2:{
        book.delete_contact();
        is_saved = 0;
        break;
    }
    case 3:{
        book.edit_contact();
        is_saved = 0;
        break;
    }
    case 4:{
      book.print_all();
      break;
    }
    case 5:{
        book.find_contact();
        break;
    }
    case 6:{
        book.save();
        break;
    }
    case 7:{
        book.load();
        break;
    }
    case 8:{
        if(!is_saved){
            char exit_choice;
            cout << "Changes are not saved. Confirm exit (Y/N): ";
            cin >> exit_choice;
            if(exit_choice != 'Y' && exit_choice != 'y'){
                choice = 0;
            }
        }
        break;
    }
}
}
}