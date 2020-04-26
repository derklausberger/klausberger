#include "manager.h"

#include <string>

using namespace std;

void Manager::print() {
    for (User user : users) {
        user.print();
    }
}

bool Manager::contains(string username) {
    for (User user : users) {
        if (user.get_name().compare(username) == 0) {
            return true;
        }
    }

    return false;
}

void Manager::add(string name, string pw) {
    users.push_back(User{name, pw});
}
