cmake_minimum_required(VERSION 3.7.2)
project(common-test)

set(COMMON_SOURCE_FILES
    lib/sqlite/sqlite3.c
    db/db.cpp
    db/upgrade.cpp
    repository/repository.cpp
    repository/repositoryconfig.cpp
    repository/taskrepository.cpp
    task/tasks.cpp
)

set(DB_CREATE_BUILD create-db)
set(DB_CREATE_SOURCE dbtest/main.cpp)

set(UNIT_TEST_BUILD unit-tests)
set(UNIT_TEST_SOURCE test/main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

add_executable(${DB_CREATE_BUILD} ${COMMON_SOURCE_FILES} ${DB_CREATE_SOURCE} )
target_link_libraries(${DB_CREATE_BUILD} Threads::Threads ${CMAKE_DL_LIBS})

add_executable(${UNIT_TEST_BUILD} ${COMMON_SOURCE_FILES} ${UNIT_TEST_SOURCE})
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
target_compile_definitions(${UNIT_TEST_BUILD} PRIVATE "BOOST_TEST_DYN_LINK=1")
target_include_directories(${UNIT_TEST_BUILD} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${UNIT_TEST_BUILD}
    Threads::Threads
    ${CMAKE_DL_LIBS}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

include_directories(".")
