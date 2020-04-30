#include <string>
#include <fstream>
#include <sstream>

#include <grpcpp/grpcpp.h>
#include "um.grpc.pb.h"

using namespace std;

using grpc::Server;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using um::UM;
using um::LoginRequest;
using um::LoginReply;

class UmClient {
    private:
        std::unique_ptr<UM::Stub> stub;
    public:
        UmClient(std::shared_ptr<Channel> channel)
            // Create a stub on the channel
            : stub(UM::NewStub(channel)) {}

    string Login(string user, string pw) {
        LoginRequest request;

        request.set_user(user);
        request.set_pw(pw);

        LoginReply reply;

        ClientContext context;

        // Make actual RPC calls on the stub.
        grpc::Status status = stub->Login(&context, request, &reply);

        if (status.ok()){
            return reply.result();
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
            return "-1";
        }
    }
};

grpc::string readFile(const grpc::string& filename) {
    string data;
    ifstream file(filename.c_str(), ios::in);

  	if (file.is_open()) {
  		stringstream ss;
  		ss << file.rdbuf ();

  		file.close ();

  		data = ss.str ();
  	} else {
        cout << "fehler" << endl;
    }

    return data;
}

void Run() {
    std::string address("localhost:8888");

    grpc::SslCredentialsOptions sslOpts;
    sslOpts.pem_root_certs = readFile("../openssl/server.crt");

    // für client-seitige Authentifizierung
    sslOpts.pem_private_key = readFile("../openssl/client.key");
    sslOpts.pem_cert_chain = readFile("../openssl/client.crt");

    // Create a default SSL ChannelCredentials object.
    auto ssl_channel_creds
        = grpc::SslCredentials(sslOpts);// grpc::SslCredentialsOptions());

    // Create a default Google ChannelCredentials object.
    //auto gtb_channel_creds = grpc::GoogleDefaultCredentials();

    auto channel_creds = grpc::InsecureChannelCredentials();

    // Create a channel using the credentials created in the previous step.
    auto channel = grpc::CreateChannel(address, ssl_channel_creds);

    UmClient client(channel);

    string response;

    string user;
    string pw;

	cout << "user: ";
	cin >> user;

    cout << "password: ";
	cin >> pw;

    response = client.Login(user, pw);
    std::cout << "Answer received: " << response << std::endl;

    while (true) {

    }
}

int main(){
    Run();

    return 0;
}
