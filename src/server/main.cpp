#include "um.grpc.pb.h"
#include "CLI11.hpp"
#include "manager.h"

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>
#include <sstream>
#include <fstream>

using namespace std;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using um::UM;
using um::LoginRequest;
using um::LoginReply;

Manager manager{};
list<User*> connections;

class UmServiceImplementation final : public UM::Service {
private:
    Manager* manager;
    list<User*>* connections;
public:
    UmServiceImplementation(Manager* manager_,
        list<User*>* connections_) : manager{manager_},
        connections{connections_}{}

    Status Login(
        ServerContext* context,
        const LoginRequest* request,
        LoginReply* reply
    ) override {
        context = context;
        string user = request->user();
        string pw = request->pw();

        if (!manager->contains(user)) {
            reply->set_result("login failed!\n" + user + " does not exist");
            cout << user << " tried to connect! (user does not exist)" << endl;
            return Status::CANCELLED;
        } else if (!manager->login(user, pw)){
            reply->set_result("login failed!\nwrong password for " + user);
            cout << user << " tried to connect! (wrong password)" << endl;
            return Status::CANCELLED;
        } else if (manager->login(user, pw)) {
            // Add the new connection to the list of the connected clients
            connections->push_back(manager->get_user(user));

            // Send a welcome message to the connected client
            reply->set_result("login successful!\nwelcome " + user);

            // Output new connection
            cout << user << " connected!" << endl;
            return Status::OK;
        } else {
            return Status::CANCELLED;
        }
    }
};

// command line processing
int clp(int argc, char *argv[]) {
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
    auto del_user_opt = user_operations
        ->add_flag("-d,--delete", del_user, ss.str())->needs(user_opt);
    ss.str(string());


    ss << "Select object by name\n"
       << "  (list existing rights with --list-rights,\n"
       << "  modify existing rights with --modify-rights,\n"
       << "  add new rights with --add-rights,\n"
       << "  delete existing rights with --delete-rights)";
    string obj;
    auto obj_opt = app.add_option("-o,--object", obj, ss.str())
        ->needs(user_opt)->excludes(del_user_opt, mod_user_opt);
    ss.str(string());

    auto right_operations = app.add_option_group("Right-Options")
        ->require_option(0, 1);

    ss << "List existing rights\n"
       << " from user or user and object";
    bool list_rights = false;
    auto list_obj_opt = right_operations
        ->add_flag("--list-rights", list_rights, ss.str())
        ->needs(user_opt);
    ss.str(string());

    ss << "Set or modify rights\n"
       << " 4 letters in correct order: rwxd\n"
       << " replace letter with - to not set or delete right";
    string rights;
    auto set_obj_opt = right_operations
        ->add_option("-s,--set-rights", rights, ss.str())
        ->needs(user_opt, obj_opt)->excludes(list_obj_opt);
    ss.str(string());

    ss << "Delete existing rights";
    bool rem_rights = false;
    right_operations
        ->add_flag("-r,--remove-rights", rem_rights, ss.str())
        ->needs(user_opt, obj_opt)->excludes(set_obj_opt, list_obj_opt);
    ss.str(string());

    //add object, remove object, list objects, modify objects ???

    try {
        app.parse(argc, argv);

        if (list) {
            manager.print();
        } else if (add_user) {
            if (!manager.add(user, pw)) {
                cerr << "user already exists" << endl;
            }
        } else if (mod_user) {
            if (manager.contains(user)) {
                if (pw.empty() && new_name.empty()) {
                    cerr << "-m,--modify requires at least user or password"
                        << endl;
                } else {
                    if (!pw.empty()) {
                        // cout << "pw" << endl;
                        manager.mod_pw(user, pw);
                    }
                    if (!new_name.empty()) {
                        // cout << "name" << endl;
                        manager.mod_name(user, new_name);
                    }
                }
            } else {
                cerr << "user does not exist" << endl;
            }
        } else if (del_user) {
            if (!manager.del(user)) {
                cerr << "user does not exist" << endl;
            }
        } else if (list_rights) {
            manager.print_rights(user, obj);
        } else if (!obj.empty()) {
            if (!rights.empty()) {
                if (!manager.set_right(user, obj, rights)) {
                    cerr << "Error: rights are not formatted correctly\n"
                        << " make sure the order matches: rwxd" << endl;
                }
            } else if (rem_rights) {
                if (!manager.rem_right(user, obj)) {
                    cerr << "Error: no existing rights for user refering to"
                        << " object" << endl;
                }
            } else {
                cerr << "object requires 1 of following options: -s,-d,-l"
                    << endl;
            }
        }
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    return 0;
}

grpc::string readFile(const grpc::string& filename) {
    string data;
    ifstream file(filename.c_str(), ios::in);

  	if (file.is_open()) {
  		stringstream ss;
  		ss << file.rdbuf ();

  		file.close ();

  		data = ss.str ();
  	} else {
        cout << "fehler" << filename << endl;
    }

    return data;
}

void Run() {
    std::string address("localhost:8888");
    UmServiceImplementation service{&manager, &connections};

    ServerBuilder builder;

    grpc::SslServerCredentialsOptions sslOpts{};
    // für client-seitige Authentifizierung
    sslOpts.client_certificate_request =
    GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;

    sslOpts.pem_key_cert_pairs.push_back(
       grpc::SslServerCredentialsOptions::PemKeyCertPair{
         readFile("../openssl/server.key"),
         readFile("../openssl/server.crt")});
    // für client-seitige Authentifizierung
    //sslOpts.pem_root_certs = readFile("../openssl/client.crt");

    auto creds = grpc::SslServerCredentials(sslOpts);

    //grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp ={};
    //grpc::SslServerCredentialsOptions ssl_opts;
    //ssl_opts.pem_root_certs="";
    //ssl_opts.pem_key_cert_pairs.push_back(pkcp);
    //std::shared_ptr<grpc::ServerCredentials> creds = grpc::SslServerCredentials(grpc::SslServerCredentialsOptions());

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    //server->Wait();

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

            clp(result.size(), result.data());
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
