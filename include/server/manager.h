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

    bool contains(std::string);

    bool add(std::string, std::string);

    bool mod_pw(std::string, std::string);

    bool mod_name(std::string, std::string);

    bool del(std::string);
};

#endif /* end of inclde guard:  */
