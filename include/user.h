#ifndef USER_H
#define USER_H

enum Action {read,write,execute,delete};

class User {
private:
    string name;
    string pw;
public:
    User (string _name, string _pw) : name{_name}, pw{_pw} {}
}

#endif /* end of include guard:  */
