SET( CMAKE_CXX_FLAGS  "-std=c++11 -DBOOST_LOG_DYN_LINK" )

cmake_minimum_required(VERSION 2.6)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           ON)   # force dynamic linking for all libraries

add_definitions(-DBOOST_ALL_NO_LIB)



set(Boost_INCLUDE_DIR $ENV{BOOST_ROOT_BUILD}/boost/include)
set(Boost_LIBRARY_DIR $ENV{BOOST_ROOT_BUILD}/boost/lib)
find_package(Boost 1.55.0 COMPONENTS system thread log log_setup program_options regex filesystem REQUIRED)
find_package( OpenSSL )
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

if (OPENSSL_FOUND)
    add_definitions(-DBOOST_NETWORK_ENABLE_HTTPS)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

# Locate cpp-netlib

set ( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{CPP_NETLIB_BUILD} )
find_package ( cppnetlib 0.11.0 REQUIRED )
include_directories ( ${CPPNETLIB_INCLUDE_DIRS} )

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(./include)

# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests  src/message_util.cpp src/jira_util.cpp test/test_all.cpp src/jira_issue.cpp src/jira_query.cpp test/jira_issue.test.cpp test/jira_util.test.cpp  test/jira_query.test.cpp test/message_util.test.cpp )
target_link_libraries(runTests ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_REGEX_LIBRARY} ${CPPNETLIB_LIBRARIES} ${GTEST_LIBRARIES} ${Boost_THREAD_LIBRARY}  ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} pthread)

# Link runTests with what we want to test and the GTest and pthread library
add_executable(issuecheck src/message_util.cpp src/jira_util.cpp src/jira_issue.cpp src/jira_query.cpp src/issuecheck.cpp)
target_link_libraries(issuecheck ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_REGEX_LIBRARY} ${CPPNETLIB_LIBRARIES} ${GTEST_LIBRARIES} ${Boost_THREAD_LIBRARY}  ${Boost_LOG_LIBRARY} ${Boost_LOG_SETUP_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} pthread)


