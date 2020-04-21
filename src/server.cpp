#include <string>

#include <grpcpp/grpcpp.h>
#include "um.grpc.pb.h"

using namespace std;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using um::UmTest;
using um::UmRequest;
using um::UmReply;

class UmServiceImplementation final : public UM::Service {
    Status sendRequest(
        ServerContext* context,
        const UmRequest* request,
        UmReply* reply
    ) override {
        string msg = request->msg();

        reply->set_result(msg + " erfolgreich!´");

        return Status::OK;
    }
};

void Run() {
    std::string address("0.0.0.0:8888");
    UmServiceImplementation service;

    ServerBuilder builder;

    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << address << std::endl;

    server->Wait();
}

int main(int argc, char** argv) {
    Run();

    return 0;
}
