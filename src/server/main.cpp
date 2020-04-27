#include "um.grpc.pb.h"
#include "CLI11.hpp"
#include "manager.h"

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <sstream>

using namespace std;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using um::UM;
using um::UmRequest;
using um::UmReply;

class UmServiceImplementation final : public UM::Service {
    Status sendRequest(
        ServerContext* context,
        const UmRequest* request,
        UmReply* reply
    ) override {
        context = context;
        string msg = request->msg();

        reply->set_result("Hallo " + msg);
		cout << msg << " connected!" << endl;
        return Status::OK;
    }
};

// command line processing
int clp(int argc, char *argv[], Manager *manager) {
    CLI::App app{"Manages users and rights"};
    //CLI11_PARSE(app, result.size(), result.data());

    // auflisten, ändern,
    stringstream ss;

    ss << "Select user by name\n"
       << " (list existing users with -l,--list,\n"
       << " modify with -m, --modify\n"
       << " add with -a,--add,\n"
       << " delete with -d,--delete)";
    string user;
    auto user_opt = app.add_option("-u,--user", user, ss.str());
    ss.str(string());

    auto user_operations = app.add_option_group("User-Options")
        ->require_option(0, 1);

    bool list = false;
    user_operations
        ->add_flag("-l,--list", list, "Prints list of all users")
        ->excludes(user_opt);

    ss << "Set password for new user\n"
       << " or for existing user\n"
       << " (--modify or --add needs to be set)";
    string pw;
    auto pw_opt = app.add_option("-p,--password", pw, ss.str())
      ->needs(user_opt);
    ss.str(string());

    ss << "Add new user";
    bool add_user = false;
    user_operations
        ->add_flag("-a,--add", add_user, ss.str())->needs(user_opt, pw_opt);  //->needs(opt_password);
    ss.str(string());

    ss << "Modify existing user";
    bool mod_user = false;
    auto mod_user_opt = user_operations
        ->add_flag("-m,--modify", mod_user, ss.str())->needs(user_opt);
    ss.str(string());

    ss << "Set new username for existing user";
    string new_name;
    app.add_option("-n,--new-name", new_name, ss.str())
      ->needs(user_opt, mod_user_opt);
    ss.str(string());

    ss << "Delete existing user";
    bool del_user = false;
    user_operations
        ->add_flag("-d,--delete", del_user, ss.str())->needs(user_opt);
    ss.str(string());


    ss << "Select object by name\n"
       << "  (list existing rights with --list-rights,\n"
       << "  modify existing rights with --modify-rights,\n"
       << "  add new rights with --add-rights,\n"
       << "  delete existing rights with --delete-rights)";
    string obj;
    auto obj_opt = app.add_flag("-o,--object", obj, ss.str())
        ->needs(user_opt);
    ss.str(string());

    auto right_operations = app.add_option_group("Right-Options")
        ->require_option(0, 1);

    ss << "List existing rights";
    bool list_rights = false;
    auto list_obj_opt = right_operations
        ->add_flag("--list-rights", list_rights, ss.str())
        ->needs(user_opt, obj_opt);
    ss.str(string());

    ss << "Modify existing rights";
    string modify_rights;
    auto mod_obj_opt = right_operations
        ->add_option("--mod-rights", modify_rights, ss.str())
        ->needs(user_opt, obj_opt);
    ss.str(string());

    ss << "Add new rights";
    string add_rights;
    auto add_obj_opt = right_operations
        ->add_option("--add-rights", add_rights, ss.str())
        ->needs(user_opt, obj_opt);
    ss.str(string());

    ss << "Delete existing rights";
    bool del_rights = false;
    auto del_obj_opt = right_operations
        ->add_flag("--del-rights", del_rights, ss.str())
        ->needs(user_opt, obj_opt);
    ss.str(string());

    //add object, remove object, list objects, modify objects ???

    // list, add, mod, del

    try {
        app.parse(argc, argv);

        if (list) {
            manager->print();
        } else if (add_user) {
            if (!manager->add(user, pw)) {
                cerr << "user already exists" << endl;
            }
        } else if (mod_user) {
            if (manager->contains(user)) {
                if (pw.empty() && new_name.empty()) {
                    cerr << "-m,--modify requires at least user or password" << endl;
                } else {
                    if (!pw.empty()) {
                        // cout << "pw" << endl;
                        manager->mod_pw(user, pw);
                    }
                    if (!new_name.empty()) {
                        // cout << "name" << endl;
                        manager->mod_name(user, new_name);
                    }
                }
            } else {
                cerr << "user does not exist" << endl;
            }
        } else if (del_user) {
            if (!manager->del(user)) {
                cerr << "user does not exist" << endl;
            }
        } else if (list_rights) {
            //manager->print_rights(user);
        } else if (!add_rights.empty()) {
            //manager->add_rights(user, add_rights);
        }

        //cout << list << " " << user << endl;
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    return 0;
}

void Run() {
    std::string address("localhost:8888");
    UmServiceImplementation service;

    ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    //server->Wait();

    Manager manager{};

    string server_str = "./server";
    char *server_name = new char[server_str.size()+1];
    strcpy(server_name, server_str.c_str());

    string input;

    while (true) {
        cout << server_name << "$ " << flush;
        getline(cin, input);

        if (input.length() > 0) {
            vector<char *> result;
            istringstream iss(input);

            result.push_back(server_name);

            for(string s; iss >> s;) {
                char *c = new char[s.size() + 1];
                strcpy(c, s.c_str());

                result.push_back(c);
            }

            clp(result.size(), result.data(), &manager);
        } else {
        }
    }

    delete server_name;
}

int main(int argc, char* argv[]) {
    /*CLI::App app{"Manages users and rights"};
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }*/

    Run();

    return 0;
}
