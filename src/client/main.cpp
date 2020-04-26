#include <string>

#include <grpcpp/grpcpp.h>
#include "um.grpc.pb.h"

using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using um::UM;
using um::UmRequest;
using um::UmReply;

class UmTestClient {
    private:
        std::unique_ptr<UM::Stub> stub_;
    public:
        UmTestClient(std::shared_ptr<Channel> channel) : stub_(UM::NewStub(channel)) {}

    string sendRequest(string msg) {
        UmRequest request;

        request.set_msg(msg);

        UmReply reply;

        ClientContext context;

        Status status = stub_->sendRequest(&context, request, &reply);

        if(status.ok()){
            return reply.result();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "-1";
        }
    }
};

void Run() {
    std::string address("localhost:8888");
    UmTestClient client(
        grpc::CreateChannel(
            address,
            grpc::InsecureChannelCredentials()
        )
    );

    string response;

    string msg;

	cout << "Name: ";
	cin >> msg;

    response = client.sendRequest(msg);
    std::cout << "Answer received: " << response << std::endl;
}

int main(){
    Run();

    return 0;
}
