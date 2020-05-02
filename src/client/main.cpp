#include "um.grpc.pb.h"
#include "CLI11.hpp"
#include "user.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <grpcpp/grpcpp.h>

#include <string>
#include <fstream>
#include <limits>
#include <sstream>
#include <signal.h>

using namespace std;

using grpc::Server;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

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
using um::LogoutRequest;
using um::LogoutReply;

class UmClient {
    private:
        unique_ptr<UM::Stub> stub;
        string user;
    public:
        UmClient(shared_ptr<Channel> channel)
            // Create a stub on the channel
            : stub(UM::NewStub(channel)) {}

    string get_username(){
        return user;
    }

    int Login(string username, string pw) {
        LoginRequest request;

        request.set_user(username);
        request.set_pw(pw);

        LoginReply reply;

        ClientContext context;

        // Make actual RPC calls on the stub.
        grpc::Status status = stub->Login(&context, request, &reply);

        if (status.ok()){
            user = username;
            cout << "Answer received: "
                << reply.result() << endl;
            return 0;
        } else if (status.error_code() == 1){
            cout << "Answer received: "
                << status.error_code() << ": " << status.error_message()
                << endl;
            return 1;
        } else {
            return -1;
        }
    }

    string ListRights(string obj) {
        ListRequest request;

        request.set_user(user);
        request.set_obj(obj);

        ListReply reply;

        ClientContext context;

        // Make actual RPC calls on the stub.
        grpc::Status status = stub->ListRights(&context, request, &reply);

        if (status.ok()){
            return reply.result();
        } else {
            cout << status.error_code() << ": " << status.error_message()
                << endl;
            return "-1";
        }
    }

    string Delete(string user, string obj) {
        DeleteRequest request;

        request.set_user(user);
        request.set_obj(obj);

        DeleteReply reply;

        ClientContext context;

        // Make actual RPC calls on the stub.
        grpc::Status status = stub->Delete(&context, request, &reply);

        if (status.ok()){
            return reply.result();
        } else {
            cout << status.error_code() << ": " << status.error_message()
                << endl;
            return "-1";
        }
    }

    string Write(string user, string obj) {
        WriteRequest request;

        request.set_user(user);
        request.set_obj(obj);

        WriteReply reply;

        ClientContext context;

        // Make actual RPC calls on the stub.
        grpc::Status status = stub->Write(&context, request, &reply);

        if (status.ok()){
            return reply.result();
        } else {
            cout << status.error_code() << ": " << status.error_message()
                << endl;
            return "-1";
        }
    }

    string Read(string user, string obj) {
        ReadRequest request;

        request.set_user(user);
        request.set_obj(obj);

        ReadReply reply;

        ClientContext context;

        // Make actual RPC calls on the stub.
        grpc::Status status = stub->Read(&context, request, &reply);

        if (status.ok()){
            return reply.result();
        } else {
            cout << status.error_code() << ": " << status.error_message()
                << endl;
            return "-1";
        }
    }

    string Execute(string user, string obj) {
        ExecuteRequest request;

        request.set_user(user);
        request.set_obj(obj);

        ExecuteReply reply;

        ClientContext context;

        // Make actual RPC calls on the stub.
        grpc::Status status = stub->Execute(&context, request, &reply);

        if (status.ok()){
            return reply.result();
        } else {
            cout << status.error_code() << ": " << status.error_message()
                << endl;
            return "-1";
        }
    }

    string Logout() {
        LogoutRequest request;

        request.set_user(user);

        LogoutReply reply;

        ClientContext context;

        // Make actual RPC calls on the stub.
        grpc::Status status = stub->Logout(&context, request, &reply);

        if (status.ok()) {
            cout << reply.result();
            return reply.result();
        } else {
            cout << status.error_code() << ": " << status.error_message()
                << endl;
            return "-1";
        }
    }
};

UmClient* client = nullptr;

int clp_user(int argc, char *argv[]) {
    CLI::App app{"Manages users and rights"};

    auto operations = app.add_option_group("Operations")
        ->require_option(0, 1);

    stringstream ss;

    ss << "Select object by name\n"
       << " (list existing rights for object with -l,--list,\n"
       << " read with -r, --read\n"
       << " write with -w,--write,\n"
       << " execute with -x,--execute,\n"
       << " delete with -d,--delete)";
    string obj;
    auto obj_opt = app.add_option("-o,--object", obj, ss.str());
    ss.str(string());

    ss << "list existing rights\n"
       << " from user or user and object";
    bool list = false;
    operations->add_flag("-l,--list", list, ss.str());
    ss.str(string());

    ss << "read object";
    bool read = false;
    operations->add_flag("-r,--read", read, ss.str())->needs(obj_opt);
    ss.str(string());

    ss << "write object";
    bool write = false;
    operations->add_flag("-w,--write", write, ss.str())->needs(obj_opt);
    ss.str(string());

    ss << "execute object";
    bool exe = false;
    operations->add_flag("-x,--execute", exe, ss.str())->needs(obj_opt);
    ss.str(string());

    ss << "delete object";
    bool del = false;
    operations->add_flag("-d,--delete", del, ss.str())->needs(obj_opt);
    ss.str(string());

    ss << "logout user and exit programm";
    bool logout = false;
    operations->add_flag("--logout", del, ss.str())->excludes(obj_opt);
    ss.str(string());

    /*ss << "Select user by name\n"
       << " (list existing users with -l,--list,\n"
       << " modify with -m, --modify\n"
       << " add with -a,--add,\n"
       << " delete with -d,--delete)";
    string user;
    auto user_opt = app.add_option("-d,--delete", user, ss.str());
    ss.str(string());*/

    try {
        app.parse(argc, argv);
        if (logout) {
            client->Logout();
            exit(0);
        } else if (list) {
            cout << client->ListRights(obj);
        } else if (del) {
            cout << client->Delete(client->get_username(), obj) << endl;
        } else if (read) {
            cout << client->Read(client->get_username(), obj) << endl;
        } else if (exe) {
            cout << client->Execute(client->get_username(), obj) << endl;
        } else if (write) {
            cout << client->Write(client->get_username(), obj) << endl;
        } else if (!obj.empty()) {
            cerr << "--object: requires one option of Option-Group: Operation\n"
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
        cout << "problem reading file: " << filename << endl;
    }

    return data;
}

void exiting() {
    std::cout << "Exiting";
}

void signal_handler(int signum) {
    cout << "logging off..." << endl;
    client->Logout();
    exit(signum);
}

void run_ssl() {
    string address("localhost:8888");

    grpc::SslCredentialsOptions sslOpts;
    sslOpts.pem_root_certs = readFile("../openssl/server.crt");

    // Create an SSL ChannelCredentials object.
    auto channel_creds = grpc::SslCredentials(sslOpts);

    // Create a channel using the credentials created in the previous step.
    auto channel = grpc::CreateChannel(address, channel_creds);

    UmClient c(channel);
    client = &c;

    int response = -1;

    string user;
    string pw;

    while (response != 0) {

    	cout << "username: ";
    	cin >> user;

        cout << "password: ";
    	cin >> pw;

        response = client->Login(user, pw);

        if (response == -1) {
            cout << "no server with your communication type running\n"
                << "try restarting client --help" << endl;
            exit(-1);
        }
    }

    signal(SIGINT, signal_handler);

    char *username = new char[user.size()+1];
    strcpy(username, user.c_str());
    string input;
    cin.ignore();
    while (true) {
        cout << user << "$ " << flush;
        getline(cin, input);

        if (input.length() > 0) {
            vector<char *> result;
            istringstream iss(input);

            result.push_back(username);

            for(string s; iss >> s;) {
                char *c = new char[s.size() + 1];
                strcpy(c, s.c_str());

                result.push_back(c);
            }

            clp_user(result.size(), result.data());
        } else {
        }
    }
}

void run_ins() {
    string address("localhost:8888");

    // Create a default Insecure ChannelCredentials object.
    auto channel_creds = grpc::InsecureChannelCredentials();

    // Create a channel using the credentials created in the previous step.
    auto channel = grpc::CreateChannel(address, channel_creds);

    UmClient c(channel);
    client = &c;

    int response = 1;

    string user;
    string pw;

    while (response != 0) {

    	cout << "username: ";
    	cin >> user;

        cout << "password: ";
    	cin >> pw;

        response = client->Login(user, pw);

        if (response == -1) {
            cout << "no server with your communication type running\n"
                << "restarting client with --help" << endl;
            exit(-1);
        }
    }

    signal(SIGINT, signal_handler);

    char *username = new char[user.size()+1];
    strcpy(username, user.c_str());
    string input;
    cin.ignore();
    while (true) {
        cout << user << "$ " << flush;
        getline(cin, input);

        if (input.length() > 0) {
            vector<char *> result;
            istringstream iss(input);

            result.push_back(username);

            for(string s; iss >> s;) {
                char *c = new char[s.size() + 1];
                strcpy(c, s.c_str());

                result.push_back(c);
            }

            clp_user(result.size(), result.data());
        } else {
        }
    }
}

int main(int argc, char* argv[]){
    CLI::App app{"Sets communiation security"};

    auto comm_group = app.add_option_group("Communication Security")
        ->require_option(0, 1);

    stringstream ss;

    ss << "use insecure communication [default]";
    bool insecure = false;
    comm_group->add_flag("-i,--insecure", insecure, ss.str());
    ss.str(string());

    ss << "use ssl/tls secured communication";
    bool secure = false;
    comm_group->add_flag("-s,--secure", secure, ss.str());
    ss.str(string());

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    secure ? run_ssl() : run_ins();

    return 0;
}
