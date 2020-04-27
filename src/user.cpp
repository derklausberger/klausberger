#include "user.h"

#include <string>
#include <iostream>

using namespace std;

string User::get_name() {
    return name;
}

void User::set_name(string name_) {
    name = name_;
}

string User::get_pw() {
    return pw;
}

void User::set_pw(string pw_) {
    pw = pw_;
}

void User::print() {
    cout << "Name: " << name << " Password: " << pw << endl;
}
/* object is missing in definition/declaration -> todo

bool User::has_right(string object) {
    for (auto& right : right) {
        if (right.get_right().compare(object) == 0) {
            return true;
        }
    }

    return false;
}

bool User::add_right(string object, string right) {
    rights.push_back(right);
}
*/
