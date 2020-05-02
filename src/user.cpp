#include "user.h"

#include <string>
#include <iostream>
#include <algorithm>
#include <map>
#include <list>
#include <sstream>

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

bool User::has_rights_for_obj(string object) {
    return (rights.find(object) != rights.end());
}

string User::get_rights(string object) {
    stringstream ss;

    ss << "Name: " << name << " Object: " << object << " Rights: ";
    list<Action>* actions = &rights[object];

    if (find(actions->begin(), actions->end(), Action::Read) != actions->end()) {
        ss << "r";
    } else {
        ss << "-";
    }

    if (find(actions->begin(), actions->end(), Action::Write) != actions->end()) {
        ss << "w";
    } else {
        ss << "-";
    }

    if (find(actions->begin(), actions->end(), Action::Execute) != actions->end()) {
        ss << "x";
    } else {
        ss << "-";
    }

    if (find(actions->begin(), actions->end(), Action::Delete) != actions->end()) {
        ss << "d";
    } else {
        ss << "-";
    }

    ss << endl;

    return ss.str();
}

void User::print_rights(string object) {
    cout << get_rights(object);
}

string User::get_rights() {
    stringstream ss;

    for (auto& obj : rights) {
        ss << get_rights(obj.first);
    }

    return ss.str();
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

bool User::allowed_to_read(string object) {
    list<Action>* tmp = &rights[object];
    return find(tmp->begin(), tmp->end(), Action::Read) != tmp->end();
}

bool User::allowed_to_write(string object) {
    list<Action>* tmp = &rights[object];
    return find(tmp->begin(), tmp->end(), Action::Write) != tmp->end();
}

bool User::allowed_to_execute(string object) {
    list<Action>* tmp = &rights[object];
    return find(tmp->begin(), tmp->end(), Action::Execute) != tmp->end();
}

bool User::allowed_to_delete(string object) {
    list<Action>* tmp = &rights[object];
    return find(tmp->begin(), tmp->end(), Action::Delete) != tmp->end();
}

bool User::rem_right(string object) {
    if (rights.find(object) != rights.end()) {
        rights.erase(object);

        return true;
    }

    return false;
}
