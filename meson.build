project('grpc-pro', 'cpp',
        default_options : ['cpp_std=c++11'])

protoc = find_program('protoc', required : true)
grpc_cpp_plugin = find_program('grpc_cpp_plugin', required : true)
protobuf_dep = dependency('protobuf', required : true)
grpc_dep = dependency('grpc', required : true)
grpcpp_dep = dependency('grpc++', required : true)
threads = dependency('threads')

gen = generator(protoc, \
  output    : ['@BASENAME@.pb.cc', '@BASENAME@.pb.h', '@BASENAME@.grpc.pb.cc', '@BASENAME@.grpc.pb.h'], \
  arguments : ['--proto_path=@CURRENT_SOURCE_DIR@/src/', '--plugin=protoc-gen-grpc=' + grpc_cpp_plugin.path(),
               '--grpc_out=@BUILD_DIR@', '--cpp_out=@BUILD_DIR@', '@CURRENT_SOURCE_DIR@/src/@PLAINNAME@'])

generated = gen.process('src/um.proto')

inc_dir = include_directories('include/client')

src_client = ['src/client/main.cpp', generated]

executable('client',
           sources : src_client,
           include_directories : [inc_dir],#, inc_client],
           dependencies : [threads, protobuf_dep, grpc_dep,
           grpcpp_dep]
          )

inc_dir = include_directories('include/server')

src_client = ['src/server/main.cpp', generated]

executable('server',
           sources : src_client,
           include_directories : [inc_dir],#, inc_client],
           dependencies : [threads, protobuf_dep, grpc_dep,
           grpcpp_dep]
          )
