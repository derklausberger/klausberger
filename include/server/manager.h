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

    bool set_right(std::string, std::string, std::string);

    bool rem_right(std::string, std::string);

    bool print_rights(std::string, std::string);

    bool login(std::string, std::string);

    User* get_user(std::string);
};

#endif /* end of include guard:  */
