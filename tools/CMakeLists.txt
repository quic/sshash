add_library(sshash-utils STATIC elf-parser.hpp elf-parser.cc sha.hpp sha.cc)
target_include_directories(sshash-utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sshash-utils PUBLIC OpenSSL::SSL)

add_executable(sshash-elf elf-tool.cc)
target_link_libraries(sshash-elf PRIVATE sshash sshash-utils Boost::program_options)

add_executable(sshash-text IMPORTED [GLOBAL])

install(TARGETS sshash-elf DESTINATION bin COMPONENT tools)
install(PROGRAMS sshash-text DESTINATION bin COMPONENT tools)
