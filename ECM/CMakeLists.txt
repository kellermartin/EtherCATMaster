# project name and language
#project(ethercatmaster LANGUAGES CXX)

file(GLOB ECM_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*cpp)

file(GLOB ECM_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/EtherCATMaster/*.h)

add_library(ethercatmaster)



target_sources(ethercatmaster
  PRIVATE
    ${ECM_SOURCES}
  PUBLIC
    ${ECM_HEADERS}
)

target_link_libraries(ethercatmaster soem)

target_include_directories(ethercatmaster
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include/
  )