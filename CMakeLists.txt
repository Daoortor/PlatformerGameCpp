cmake_minimum_required(VERSION 3.23)
project(PlatformerGameCpp)

SET(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

include_directories(gui/assets model/levels model/include gui/include server/include)

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# Protobuf-compiler
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Proto file
get_filename_component(hw_proto "server/include/json_file_exchange.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/json_file_exchange.pb.cc") # TODO: move?
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/json_file_exchange.pb.h") # TODO: move?
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/json_file_exchange.grpc.pb.cc") # TODO: move?
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/json_file_exchange.grpc.pb.h") # TODO: move?
add_custom_command(
        OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
        DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(game
        gui/main.cpp
        gui/src/windows/level-window.cpp
        model/src/block.cpp
        model/src/game.cpp
        model/src/observers.cpp
        model/src/player.cpp
        model/src/board.cpp
        tools/utilities.cpp
        gui/src/elements/button.cpp
        gui/src/windows/menu.cpp
        gui/src/performer.cpp
        gui/src/elements/scrollbar.cpp
        gui/src/elements/textbox.cpp
        gui/src/windows/level-play.cpp
        gui/src/windows/level-editor.cpp
        gui/src/statistics.cpp

        server/src/client.cpp
        server/src/source.cpp
        ${hw_proto_srcs}
        ${hw_grpc_srcs})

add_executable(test
        model/src/block.cpp
        model/src/game.cpp
        model/src/observers.cpp
        model/src/player.cpp
        model/src/board.cpp
        tools/utilities.cpp
        tests/tests.cpp
        tests/doctest_main.cpp)

add_executable(server
        server/src/server.cpp
        server/src/source.cpp
        ${hw_proto_srcs}
        ${hw_grpc_srcs}
        )

target_link_libraries(game
        sfml-graphics
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
target_link_libraries(server
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
