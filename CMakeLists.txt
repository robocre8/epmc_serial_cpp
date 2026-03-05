cmake_minimum_required(VERSION 3.16)

project(epmc_serial
    VERSION 1.3.1
    LANGUAGES CXX
)

# --------------------------------------------------
# Header-only library
# --------------------------------------------------
add_library(epmc_serial INTERFACE)

target_include_directories(epmc_serial INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(epmc_serial INTERFACE cxx_std_17)

# --------------------------------------------------
# Dependency: LibSerial
# --------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSERIAL REQUIRED libserial)

target_include_directories(epmc_serial INTERFACE
    ${LIBSERIAL_INCLUDE_DIRS}
)

target_link_libraries(epmc_serial INTERFACE
    ${LIBSERIAL_LIBRARIES}
)

target_compile_options(epmc_serial INTERFACE
    ${LIBSERIAL_CFLAGS_OTHER}
)

# Namespaced alias
add_library(epmc_serial::epmc_serial ALIAS epmc_serial)

# --------------------------------------------------
# Examples
# --------------------------------------------------
option(EPMC_SERIAL_BUILD_EXAMPLES "Build EPMC Serial examples" OFF)

if(EPMC_SERIAL_BUILD_EXAMPLES)
    add_executable(motor_control examples/motor_control.cpp)
    target_link_libraries(motor_control PRIVATE epmc_serial::epmc_serial)
endif()

# --------------------------------------------------
# Install
# --------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    TARGETS epmc_serial
    EXPORT epmc_serialTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT epmc_serialTargets
    NAMESPACE epmc_serial::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/epmc_serial
)

# --------------------------------------------------
# Package config files
# --------------------------------------------------
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/epmc_serialConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/epmc_serialConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/epmc_serialConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/epmc_serial
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/epmc_serialConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/epmc_serialConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/epmc_serial
)
