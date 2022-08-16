cmake_minimum_required(VERSION 3.13)
message("Configuring EtherCATMaster project")
# project name and language
project(ethercatmaster LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_subdirectory(SOEM)
add_subdirectory(ECM)
add_subdirectory(Test)


