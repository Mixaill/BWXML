project(BWXML)
cmake_minimum_required( VERSION 2.8 )

#
# Compiler options
#
option(USE_STATIC_BOOST "Use static linking with boost" ON)

if(USE_STATIC_BOOST)
    set(Boost_USE_STATIC_LIBS ON)
endif()

if(NOT MSVC)
    list(APPEND CMAKE_CXX_FLAGS "-std=c++0x")
endif()

#
# Depends
#
find_package(Boost COMPONENTS filesystem program_options serialization system thread REQUIRED)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Fatal error: Boost required.\n")
endif (NOT Boost_FOUND)

#
# Build
#
aux_source_directory(BWXML SOURCES)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(BWXML ${Boost_INCLUDE_DIRS})

file (GLOB headers BWXML/*.h*)
add_executable(bwxml ${SOURCES} ${headers})
target_link_libraries(bwxml ${Boost_LIBRARIES})

#
# Installation
#
if(WIN32)
    install(TARGETS bwxml RUNTIME DESTINATION .)
else(WIN32)
    install(TARGETS bwxml RUNTIME DESTINATION bin)
endif(WIN32)

#
# CPack
#
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(CPACK_PACKAGE_ARCHITECTURE amd64)
else()
  set(CPACK_PACKAGE_ARCHITECTURE i686)
endif()

set(CPACK_PACKAGE_NAME "bwxml")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 5)
set(CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}) 
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A tool for packing and unpacking BigWorld compressed data sections from/to plain XML")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_ARCHITECTURE})
set(CPACK_PACKAGE_CONTACT "Mikhail Paulyshka <me@mixaill.tk>")

include(CPack)

