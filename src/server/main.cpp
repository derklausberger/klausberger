#include "um.grpc.pb.h"
#include "CLI11.hpp"

#include <grpcpp/grpcpp.h>

#include <iostream>
#include <vector>
#include <string>
#include <stdio.h>

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
int clp(int argc, char* argv[]) {
    CLI::App app{"Manages users and rights"};
    //CLI11_PARSE(app, result.size(), result.data());
    bool list;
    app.add_flag("-l,--list", list, "Prints list of all users.");

    string user;
    app.add_option("-u,--user", user, "Set user by user name (list all possible user names with -l,--list).");

    try {
        app.parse(argc, argv);

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

    string server_str = "./server";
    char *server_name = new char[server_str.size()+1];
    strcpy(server_name, server_str.c_str());

    string input;

    while (true) {
        cout << server_name << " " << flush;
        getline(cin, input);

        if (input.length() > 0) {
            vector<char*> result;
            istringstream iss(input);

            result.push_back(server_name);

            for(string s; iss >> s;) {
                char *c = new char[s.size()+1];
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
