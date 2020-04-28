#include "user.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <list>

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

void User::print_rights(string object) {
    string r = "";

    list<Action>* actions = &rights[object];

    if (find(actions->begin(), actions->end(), Action::Read) != actions->end()) {
        r += "r";
    } else {
        r += "-";
    }

    if (find(actions->begin(), actions->end(), Action::Write) != actions->end()) {
        r += "w";
    } else {
        r += "-";
    }

    if (find(actions->begin(), actions->end(), Action::Execute) != actions->end()) {
        r += "x";
    } else {
        r += "-";
    }

    if (find(actions->begin(), actions->end(), Action::Delete) != actions->end()) {
        r += "d";
    } else {
        r += "-";
    }

    cout << "Name: " << name << " Object: " << object << " Rights: " << r << endl;
}

void User::print_rights() {
    for (auto& obj : rights) {
        print_rights(obj.first);
    }
}

bool User::set_right(string object, string right) {
    if (right.length() == 4) {
        if (right.compare("----") == 0) {
            rem_right(object);
            return true;
        }

        if (right.at(0) == 'r') {
            rights[object].push_back(Action::Read);
        } else if (right.at(0) == '-') {
            rights[object].remove(Action::Read);
        } else {
            return false;
        }

        if (right.at(1) == 'w') {
            rights[object].push_back(Action::Write);
        } else if (right.at(1) == '-') {
            rights[object].remove(Action::Write);
        } else {
            return false;
        }

        if (right.at(2) == 'x') {
            rights[object].push_back(Action::Execute);
        } else if (right.at(2) == '-') {
            rights[object].remove(Action::Execute);
        } else {
            return false;
        }

        if (right.at(3) == 'd') {
            rights[object].push_back(Action::Delete);
        } else if (right.at(3) == '-') {
            rights[object].remove(Action::Delete);
        } else {
            return false;
        }

        return true;
    }

    return false;
}

bool User::rem_right(string object) {
    if (rights.find(object) != rights.end()) {
        rights.erase(object);

        return true;
    }

    return false;
}
