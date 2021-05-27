cmake_minimum_required(VERSION 3.15.0)

project(sshash LANGUAGES CXX)

# FIXME: set these from Git
set(SSHASH_MAJOR_VERSION 1)
set(SSHASH_MINOR_VERSION 2)
set(SSHASH_PATCH_VERSION 0)
set(SSHASH_VERSION "${SSHASH_MAJOR_VERSION}.${SSHASH_MINOR_VERSION}.${SSHASH_PATCH_VERSION}")

# Set default pref to /usr (unless user provided an override)
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "..." FORCE)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WITH_TOOLS "enable sshash tools" ON)
option(WITH_TESTS "enable sshash tests" ON)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(HOGL 3.0)

add_subdirectory(src)

if (WITH_TOOLS)
	find_package(OpenSSL REQUIRED)
	add_subdirectory(tools)
endif()

if (WITH_TESTS)
	add_subdirectory(tests)
endif()

set(CONF_VERSION "${SSHASH_VERSION}")
set(CONF_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(CONF_LIBRARY_DIR "${CMAKE_INSTALL_PREFIX}/lib")
configure_file(sshash-config.cmake.in
	"${PROJECT_BINARY_DIR}/sshash-config.cmake" @ONLY)
configure_file(sshash-config.cmake.in
	"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/sshash-config.cmake" @ONLY)
configure_file(sshash-config-version.cmake.in
	"${PROJECT_BINARY_DIR}/sshash-config-version.cmake" @ONLY)

install(FILES
	"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/sshash-config.cmake"
	"${PROJECT_BINARY_DIR}/sshash-config-version.cmake"
	DESTINATION lib/cmake/sshash COMPONENT dev)

# DEB packaging
set(CPACK_PACKAGE_VENDOR  "Max Krasnyansky")
set(CPACK_PACKAGE_CONTACT "quic_maxk@quicinc.com")
set(CPACK_PACKAGE_VERSION_MAJOR "${SSHASH_MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${SSHASH_MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${SSHASH_PATCH_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${SSHASH_MAJOR_VERSION}.${SSHASH_MINOR_VERSION}.${SSHASH_PATCH_VERSION}")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "util")
set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")

set(CPACK_DEBIAN_DEV_PACKAGE_NAME "sshash-dev")
set(CPACK_DEBIAN_DEV_PACKAGE_DESCRIPTION "sshash library and headers")
set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "libboost-dev(>= 1.65.1)")

set(CPACK_DEBIAN_TOOLS_PACKAGE_NAME "sshash-tools")
set(CPACK_DEBIAN_TOOLS_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_TOOLS_PACKAGE_DESCRIPTION "sshash tools")
set(CPACK_DEBIAN_TOOLS_PACKAGE_DEPENDS "python3")

include(CPack)
