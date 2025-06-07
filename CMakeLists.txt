cmake_minimum_required(VERSION 3.15)
project(torchcs_properties)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TORCHCS_PROPERTIES_INSTALL "Enable installation of torchcs_properties" ON)
option(TORCHCS_PROPERTIES_SHARED "Build torchcs_properties as a shared library (.so/.dll)" OFF)
option(TORCHCS_PROPERTIES_STATIC "Build torchcs_properties as a static library (.a/.lib)" ON)
option(BUILD_EXAMPLE "Build example project in sample" OFF)

set(SOURCES
    src/properties.cpp
)

if(TORCHCS_PROPERTIES_SHARED AND TORCHCS_PROPERTIES_STATIC)
    message(FATAL_ERROR "Cannot build both shared and static libraries. Choose one.")
endif()

if(TORCHCS_PROPERTIES_SHARED)
    add_library(torchcs_properties SHARED ${SOURCES})
elseif(TORCHCS_PROPERTIES_STATIC)
    add_library(torchcs_properties STATIC ${SOURCES})
else()
    message(FATAL_ERROR "You must enable either shared or static build.")
endif()

add_library(torchcs::properties ALIAS torchcs_properties)

target_include_directories(torchcs_properties
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(torchcs_properties PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
    target_compile_options(torchcs_properties PRIVATE /W4 /WX)
endif()

if(BUILD_EXAMPLE)
    add_subdirectory(sample)
endif()

if(TORCHCS_PROPERTIES_INSTALL)
    install(TARGETS torchcs_properties
        EXPORT torchcs_propertiesTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )
    install(DIRECTORY include/ DESTINATION include)
    install(EXPORT torchcs_propertiesTargets
        FILE torchcs_propertiesTargets.cmake
        NAMESPACE torchcs::
        DESTINATION lib/cmake/torchcs_properties
    )
endif()
