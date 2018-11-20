cmake_minimum_required(VERSION 2.8.3)

project(RE216_JALON01)

#add here source files you need to compile and link to the client app
SET(SRC_CLIENT
	client.c
    client-tools.c
    common.c
    client-tools.h
    common.h
	)


add_executable(RE216_JALON01_CLIENT ${SRC_CLIENT})


#add here source files you need to compile and link to the server app
SET(SRC_SERVER
        server.c
        server-tools.c
        common.c
        server-tools.h
        common.h
        )

add_executable(RE216_JALON01_SERVER  ${SRC_SERVER})


