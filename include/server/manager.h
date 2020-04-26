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

    void print();

    bool contains(std::string username);

    void add(std::string name, std::string pw);
};

#endif /* end of inclde guard:  */
