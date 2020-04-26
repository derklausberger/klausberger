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
    app.add_option("-u,--user", user, ss.str());
    ss.str(string());

    ss << "Set password for new user\n"
       << " or for existing user\n"
       << " (username needed,\n"
       << " set username with -u,--user)";
    string password;
    app.add_option("-p,--password", password, ss.str());
    bool list;
    app.add_flag("-l,--list", list, "Prints list of all users");
    ss.str(string());

    ss << "Add new user"
       << " (username and password needed,\n"
       << " set username with -u,--user,\n"
       << " set password with -p,--password)";
    bool add;
    app.add_flag("-a,--add", add, ss.str());
    ss.str(string());

    ss << "Modify existing user\n"
       << " (new username and/or new password needed,\n"
       << " set new password with -p,--password,\n"
       << " set new username with -n,--new)";
    bool modify;
    app.add_flag("-m,--modify", modify, ss.str());
    ss.str(string());

    ss << "Delete existing user"
       << " (username needed,\n"
       << " set username with -u,--user)";
    bool del;
    app.add_flag("-d,--delete", del, ss.str());
    ss.str(string());

    ss << "Select object by name\n"
       << " (username needed,\n"
       << " set username with -u,--user)\n"
       << "  (list existing rights with --list-rights,\n"
       << "  modify existing rights with --modify-rights,\n"
       << "  add new rights with --add-rights,\n"
       << "  delete existing rights with --delete-rights)";
    string object;
    app.add_flag("-o,--object", object, ss.str());
    ss.str(string());

    ss << "list existing rights\n"
       << " (object needed,\n"
       << " set object with -o,--object)";
    bool list_rights;
    app.add_flag("--list-rights", list_rights, ss.str());
    ss.str(string());

    ss << "modify existing rights\n"
       << " (object needed,\n"
       << " set object with -o,--object)";
    string modify_rights;
    app.add_option("--modify-rights", modify_rights, ss.str());
    ss.str(string());

    ss << "add new rights\n"
       << " (object needed,\n"
       << " set object with -o,--object)";
    string add_rights;
    app.add_option("--add-rights", add_rights, ss.str());
    ss.str(string());

    ss << "delete existing rights\n"
       << " (object needed,\n"
       << " set object with -o,--object)";
    bool delete_rights;
    app.add_flag("--delete-rights", delete_rights, ss.str());
    ss.str(string());

    //add object, remove object, list objects, modify objects ???

    try {
        app.parse(argc, argv);

        if (list) {
            manager->print();
        } else if (add) {
            //cout << "Username: " << flush;
            //getline(cin, input);

            if (!manager->contains(user)) {
                manager->add(user, "");
            } else {
                // if user already exists
            }
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
