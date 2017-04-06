project(BWXML)
cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

if(NOT VCPKG_TOOLCHAIN)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME ON)
endif()


# Boost
find_package(Boost COMPONENTS filesystem program_options serialization system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Library
set (BWXML_LIB_SOURCES "src/bwxml-lib/Base64.cpp"
                       "src/bwxml-lib/BWCommon.cpp"
                       "src/bwxml-lib/BWReader.cpp"
                       "src/bwxml-lib/BWWriter.cpp"
                       "src/bwxml-lib/DataStream.cpp")
add_library(libbwxml ${BWXML_LIB_SOURCES})
target_link_libraries(libbwxml ${Boost_LIBRARIES})

# CLI
set (BWXML_CLI_SOURCES "src/bwxml-cli/main.cpp")
add_executable(bwxml ${BWXML_CLI_SOURCES})
target_include_directories(bwxml PRIVATE "src/bwxml-lib")
target_link_libraries(bwxml ${Boost_LIBRARIES})
target_link_libraries(bwxml libbwxml)

#Python
option(BUILD_PYTHON_MODULE "Build python module" ON)
if(BUILD_PYTHON_MODULE)
    find_package(PythonInterp)
    find_package(PythonLibs)
    
    add_library(bwxml-pyd SHARED "src/bwxml-python3/pythonModule.cpp")

    target_include_directories(bwxml-pyd PRIVATE ${PYTHON_INCLUDE_DIRS})
    target_include_directories(bwxml-pyd PRIVATE "src/bwxml-lib")

    target_link_libraries(bwxml-pyd libbwxml)
    target_link_libraries(bwxml-pyd ${PYTHON_LIBRARIES})

    set_target_properties(bwxml-pyd PROPERTIES OUTPUT_NAME "bwxml")
    set_target_properties(bwxml-pyd PROPERTIES SUFFIX ".pyd")
endif()

# Installation
#
install(
    TARGETS libbwxml bwxml 
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib)

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
set(CPACK_PACKAGE_VERSION_PATCH 6)
set(CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}) 
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A tool for packing and unpacking BigWorld compressed data sections from/to plain XML")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_ARCHITECTURE})
set(CPACK_PACKAGE_CONTACT "Mikhail Paulyshka <me@mixaill.tk>")

include(CPack)

