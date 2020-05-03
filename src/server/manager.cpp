#include "manager.h"

#include <string>
#include <iostream>

using namespace std;

void Manager::print() {
    for (auto& user : users) {
        user.print();
    }
}

User* Manager::get_user(string name) {
    for (auto& user : users) {
        if (user.get_name().compare(name) == 0) {
            return &user;
        }
    }

    return nullptr;
}

bool Manager::contains(string name) {
    return (get_user(name) != nullptr);
}

bool Manager::add(string name, string pw) {
    if (!contains(name)) {
        users.push_back(User{name, pw});
        return true;
    }

    return false;
}

bool Manager::mod_pw(string name, string pw_) {
    User* user = get_user(name);

    if (user != nullptr) {
        user->set_pw(pw_);
        return true;
    }

    return false;
}

bool Manager::mod_name(string name, string new_name) {
    User* user = get_user(name);

    if (user != nullptr) {
        user->set_name(new_name);
        return true;
    }

    return false;
}

bool Manager::del(string name) {
    User* user = get_user(name);

    if (user != nullptr) {
        users.remove(*user);
        return true;
    }

    return false;
}

bool Manager::set_right(string name, string object, string right) {
    User* user = get_user(name);

    if (user != nullptr) {
        return user->set_right(object, right);
    }

    return false;
}

bool Manager::rem_right(string name, string object) {
    User* user = get_user(name);

    if (user != nullptr) {
        return user->rem_right(object);
    }

    return false;
}

bool Manager::print_rights(string name, string object) {
    User* user = get_user(name);

    if (user != nullptr) {
        if (!object.empty()) {
            user->print_rights(object);
        } else {
            user->print_rights();
        }

        return true;
    }

    return false;
}

bool Manager::login(string name, string pw) {
    User* user = get_user(name);

    if (user != nullptr) {
        return (user->get_pw().compare(pw) == 0);
    }

    return false;
}
