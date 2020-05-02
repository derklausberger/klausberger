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
using grpc::StatusCode;

using um::UM;
using um::LoginRequest;
using um::LoginReply;
using um::ListRequest;
using um::ListReply;
using um::DeleteRequest;
using um::DeleteReply;
using um::ReadRequest;
using um::ReadReply;
using um::WriteRequest;
using um::WriteReply;
using um::ExecuteRequest;
using um::ExecuteReply;

Manager manager{};
list<User*> connections;
string servername;

User* connected(string name) {
    for (auto& user : connections) {
        if (user->get_name().compare(name) == 0) {
            return user;
        }
    }

    return nullptr;
}

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
            stringstream reply;
            reply << "login failed!\n" << user << " does not exist!";

            cout << user << " tried to connect! (user does not exist)" << endl;
            cout << servername << "$ " << flush;

            return Status(StatusCode::CANCELLED, reply.str());
        } else if (!manager->login(user, pw)){
            stringstream reply;
            reply << "login failed!\nwrong password for " << user;

            cout << user << " tried to connect! (wrong password)" << endl;
            cout << servername << "$ " << flush;

            return Status(StatusCode::CANCELLED, reply.str());
        } else if (manager->login(user, pw)) {
            // Add the new connection to the list of the connected clients
            connections->push_back(manager->get_user(user));

            // Send a welcome message to the connected client
            reply->set_result("login successful!\nwelcome " + user + "!");

            // Output new connection
            cout << user << " connected!" << endl;
            cout << servername << "$ " << flush;

            return Status::OK;
        } else {
            return Status::CANCELLED;
        }
    }

    User* connected(string name) {
        for (auto& user : *connections) {
            if (user->get_name().compare(name) == 0) {
                return user;
            }
        }

        return nullptr;
    }

    Status ListRights(
        ServerContext* context,
        const ListRequest* request,
        ListReply* reply
    ) override {
        context = context;
        string username = request->user();
        string object = request->obj();

        User* user = connected(username);
        if (user != nullptr) {
            if (!object.empty()) {
                if (user->has_rights_for_obj(object)) {
                    reply->set_result(user->get_rights(object));
                } else {
                    reply->set_result("No assigned rights for this object");
                }
                cout << user->get_name() << " requested his rights for object "
                    << object << endl;
                cout << servername << "$ " << flush;
            } else {
                reply->set_result(user->get_rights());
                cout << endl << user->get_name() << " requested his rights." << endl;
                cout << servername << "$ " << flush;
            }
            return Status::OK;
        } else {
            return Status::CANCELLED;
        }
    }

    Status Delete(
        ServerContext* context,
        const DeleteRequest* request,
        DeleteReply* reply
    ) override {
        context = context;
        string username = request->user();
        string object = request->obj();

        User* user = connected(username);
        if (user != nullptr) {
            if (user->has_rights_for_obj(object)) {
                if (user->allowed_to_delete(object)) {
                    reply->set_result("Delete successful!");
                } else {
                    reply->set_result("Delete failed!");
                }
            } else {
                reply->set_result("No assigned rights for this object");
            }
            cout << endl << user->get_name() << " deleted " << object << endl;
            cout << servername << "$ " << flush;

            return Status::OK;
        } else {
            return Status::CANCELLED;
        }
    }

    Status Read(
        ServerContext* context,
        const ReadRequest* request,
        ReadReply* reply
    ) override {
        context = context;
        string username = request->user();
        string object = request->obj();

        User* user = connected(username);
        if (user != nullptr) {
            if (user->has_rights_for_obj(object)) {
                if (user->allowed_to_read(object)) {
                    reply->set_result("Read successful!");
                } else {
                    reply->set_result("Read failed!");
                }
            } else {
                reply->set_result("No assigned rights for this object");
            }
            cout << user->get_name() << " read " << object << endl;

            return Status::OK;
        } else {
            return Status::CANCELLED;
        }
    }

    Status Write(
        ServerContext* context,
        const WriteRequest* request,
        WriteReply* reply
    ) override {
        context = context;
        string username = request->user();
        string object = request->obj();

        User* user = connected(username);
        if (user != nullptr) {
            if (user->has_rights_for_obj(object)) {
                if (user->allowed_to_write(object)) {
                    reply->set_result("Write successful!");
                } else {
                    reply->set_result("Write failed!");
                }
            } else {
                reply->set_result("No assigned rights for this object");
            }
            cout << user->get_name() << " requested his rights for object "
                << object << endl;
            return Status::OK;
        } else {
            return Status::CANCELLED;
        }
    }

    Status Execute(
        ServerContext* context,
        const ExecuteRequest* request,
        ExecuteReply* reply
    ) override {
        context = context;
        string username = request->user();
        string object = request->obj();

        User* user = connected(username);
        if (user != nullptr) {
            if (user->has_rights_for_obj(object)) {
                if (user->allowed_to_execute(object)) {
                    reply->set_result("Execute successful!");
                } else {
                    reply->set_result("Execute failed!");
                }
            } else {
                reply->set_result("No assigned rights for this object");
            }
            cout << user->get_name() << " requested his rights for object "
                << object << endl;
            return Status::OK;
        } else {
            return Status::CANCELLED;
        }
    }
};

// command line processing
int clp_server(int argc, char *argv[]) {
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
            if (connected(user) != nullptr) {
                cerr << "user logged in, can not change name or password!";
            } else if (manager.contains(user)) {
                if (pw.empty() && new_name.empty()) {
                    cerr << "-m,--modify requires at least user or password"
                        << endl;
                } else {
                    if (!pw.empty()) {
                        manager.mod_pw(user, pw);
                    }
                    if (!new_name.empty()) {
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
            if (manager.contains(user)) {
                manager.print_rights(user, obj);
            } else {
                cerr << "Error: user does not exist" << endl;
            }
        } else if (!obj.empty()) {
            if (!manager.contains(user)) {
                cerr << "Error: user does not exist" << endl;
            } else if (!rights.empty()) {
                if (!manager.set_right(user, obj, rights)) {
                    cout << user << " " << obj << " " << rights << endl;
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
        } else if (!pw.empty() && !add_user && !mod_user) {
            cerr << "--password requires --add or --modify" << endl;
        } else if (!user.empty()) {
            cerr << "--user requires at least one option\n"
                << "Run with --help for more information." << endl;
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
        /* für client-seitige Authentifizierung
        sslOpts.client_certificate_request =
        GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;*/

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

    //auto creds = grpc::GoogleRefreshTokenCredentials("");

    builder.AddListeningPort(address, creds);
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    //server->Wait();

    char *server_name = new char[servername.size()+1];
    strcpy(server_name, servername.c_str());

    string input;

    while (true) {
        cout << servername << "$ " << flush;
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

            clp_server(result.size(), result.data());
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

    servername = argv[0];

    Run();

    return 0;
}
