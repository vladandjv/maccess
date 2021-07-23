cmake_minimum_required(VERSION 3.11.0)
project (maccess)

#Bring the headers into the project
include_directories(./maccess)
include_directories(./)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES_maccess "./maccess/*.c")
file(GLOB SOURCES_src "./scr/*.c")

#For static libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../lib") 
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin")

#Generate the static library from the sources
add_library(maccess STATIC ${SOURCES_maccess})

install(TARGETS maccess DESTINATION lib)

add_executable(e_load e_load.c)
add_executable(e_delete e_delete.c)
add_executable(e_print e_print.c)
add_executable(e_create e_create.c)
add_executable(e_test e_test.c)
add_executable(e_rebuild_data e_rebuild_data.c)
add_executable(e_rebuild_index e_rebuild_index.c)
add_executable(e_shmrm e_shmrm.c)
add_executable(btree_parameters btree_parameters.c)

if (UNIX)
    target_link_libraries(e_load maccess)
    target_link_libraries(e_delete maccess)
    target_link_libraries(e_print maccess)
    target_link_libraries(e_create maccess)
    target_link_libraries(e_test maccess)
    target_link_libraries(e_rebuild_data maccess)
    target_link_libraries(e_rebuild_index maccess)
    target_link_libraries(e_shmrm maccess)
    target_link_libraries(btree_parameters -lm)
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pedantic -ggdb3") 
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pedantic -O3") 
endif()

