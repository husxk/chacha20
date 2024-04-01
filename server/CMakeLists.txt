cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)

project(server C)

add_compile_options(
        -Wall -Wextra
        -g
        )

add_executable(${PROJECT_NAME}
        server.c
        ../chacha20/chacha20.c
        )

        target_include_directories(${PROJECT_NAME} PRIVATE
        ../chacha20/
        ../protocol/
        )
