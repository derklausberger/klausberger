#include "manager.h"

#include <string>
#include <iostream>

using namespace std;

void Manager::print() {
    for (auto& user : users) {
        user.print();
    }
}

bool Manager::contains(string username) {
    for (auto& user : users) {
        if (user.get_name().compare(username) == 0) {
            return true;
        }
    }

    return false;
}

bool Manager::add(string name, string pw) {
    if (!contains(name)) {
        users.push_back(User{name, pw});
        return true;
    }

    return false;
}

bool Manager::mod_pw(string name, string pw_) {
    for (auto& user : users) {
        if (user.get_name().compare(name) == 0) {
            user.set_pw(pw_);
            return true;
        }
    }

    return false;
}

bool Manager::mod_name(string name, string new_name) {
    for (auto& user : users) {
        if (user.get_name().compare(name) == 0) {
            user.set_name(new_name);
            return true;
        }
    }

    std::cout << "hallo" << std::endl;
    return false;
}

bool Manager::del(string name) {
    for (auto& user : users) {
        if (user.get_name().compare(name) == 0) {
            users.remove(user);
            return true;
        }
    }

    return false;
}

bool Manager::set_right(string name, string object, string right) {
    for (auto& user : users) {
        if (user.get_name().compare(name) == 0) {
            return user.set_right(object, right);
        }
    }

    return false;
}

bool Manager::rem_right(string name, string object) {
    for (auto& user : users) {
        if (user.get_name().compare(name) == 0) {
            return user.rem_right(object);
        }
    }

    return false;
}

void Manager::print_rights(string name, string object) {
    for (auto& user : users) {
        if (user.get_name().compare(name) == 0) {
            if (!object.empty()) {
                user.print_rights(object);
            } else {
                user.print_rights();
            }
        }
    }
}
