#ifndef USER_H
#define USER_H

#include <string>
#include <map>
#include <list>

// erster Buchstabe ist groß, weil delete ein Schlüsselwort ist und
// dann nicht verwendet werden kann (bzw. ich weiß nicht wie)
enum class Action {Read,Write,Execute,Delete};

class User {
private:
    std::string name;
    std::string pw;

    std::map<std::string, std::list<Action>> rights;
public:
    User (std::string _name, std::string _pw) : name{_name}, pw{_pw} {}

    std::string get_name();

    void set_name(std::string);

    std::string get_pw();

    void set_pw(std::string);

    void print();

    bool operator == (const User& user) const { return name == user.name; }

    bool set_right(std::string, std::string);

    bool rem_right(std::string);

    void print_rights(std::string);

    void print_rights();
};

#endif /* end of include guard:  */
