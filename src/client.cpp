#include <string>

#include <grpcpp/grpcpp.h>
#include "um.grpc.pb.h"

using namespace std;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using um::UmTest;
using um::UmRequest;
using um::UmReply;

class UmTestClient {
    private:
        std::unique_ptr<UmTest::Stub> stub_;
    public:
        UmTestClient(std::shared_ptr<Channel> channel) : stub_(UmTest::NewStub(channel)) {}

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
            return -1;
        }
    }
};

void Run() {
    std::string address("0.0.0.0:8888");
    UmTestClient client(
        grpc::CreateChannel(
            address,
            grpc::InsecureChannelCredentials()
        )
    );

    string response;

    string msg = "Test";

    response = client.sendRequest(a, b);
    std::cout << "Answer received: " << msg << " " << response << std::endl;
}

int main(int argc, char* argv[]){
    Run();

    return 0;
}