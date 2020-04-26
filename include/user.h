#ifndef USER_H
#define USER_H

#include <string>

// erster Buchstabe ist groß, weil delete ein Schlüsselwort ist und
// dann nicht verwendet werden kann (bzw. ich weiß nicht wie)
enum class Action {Read,Write,Execute,Delete};

class User {
private:
    std::string name;
    std::string pw;
public:
    User (std::string _name, std::string _pw) : name{_name}, pw{_pw} {}

    std::string get_name();

    void print();
};

#endif /* end of include guard:  */
