cmake_minimum_required (VERSION 2.8)

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")


if (CMAKE_COMPILER_IS_GNUCC)
    #set(CMAKE_C_COMPILER "/usr/bin/gcc-4.7")
    #set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.7")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g  -static-libgcc -gdwarf-2 -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g  -std=gnu++11 -gdwarf-2 -g3 -static-libstdc++ -static-libgcc -D_GLIBCXX_USE_NANOSLEEP")
endif (CMAKE_COMPILER_IS_GNUCC)

if (CMAKE_COMPILER_IS_GNUCC)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_LESS 4.7)
        message(FATAL_ERROR "gcc 4.7 or greater is required.")
    endif (GCC_VERSION VERSION_LESS 4.7)
endif (CMAKE_COMPILER_IS_GNUCC)

if (APPLE)
    include_directories ("/usr/local/include")
    link_directories ("/usr/local/lib")
endif (APPLE)

if ( "${CMAKE_GENERATOR}" STREQUAL "Xcode" )
    message( STATUS "Setting Xcode attributes")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++11 -stdlib=libc++ -g -Wall")
endif ("${CMAKE_GENERATOR}" STREQUAL "Xcode")

if (CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif (CMAKE_RUNTIME_OUTPUT_DIRECTORY)

if (COVERAGE)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
endif (COVERAGE)
