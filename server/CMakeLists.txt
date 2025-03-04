cmake_minimum_required(VERSION 2.8.12.2)
project(server)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(server
        BaseASN1.cpp
        BaseASN1.h
        Codec.cpp
        Codec.h
        CodecFactory.cpp
        CodecFactory.h
        ItcastLog.cpp
        ItcastLog.h
        main.cpp
        OCCIOP.cpp
        OCCIOP.h
        RequestCodec.cpp
        RequestCodec.h
        RequestFactory.cpp
        RequestFactory.h
        RespondCodec.cpp
        RespondCodec.h
        RespondFactory.cpp
        RespondFactory.h
        SecKeyShm.cpp
        SecKeyShm.h
        SequenceASN1.cpp
        SequenceASN1.h
        ServerOperation.cpp
        ServerOperation.h
        ShareMemory.cpp
        ShareMemory.h
        TcpServer.cpp
        TcpServer.h
        TcpSocket.cpp
        TcpSocket.h main.cpp main.cpp ItcastLog.h)
