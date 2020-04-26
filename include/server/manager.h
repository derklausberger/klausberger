#ifndef MANAGER_H
#define MANAGER_H

#include<list>
#include<string>

#include "user.h"

class Manager {
private:
    std::list<User> users;
public:
    Manager () {}

    void add_user(std::string name, std::string pw) {
        users.push_back(new User(name, pw));
    }
}

#endif /* end of inclde guard:  */
