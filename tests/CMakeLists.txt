add_executable(conf-test conf-test.cc)
target_link_libraries(conf-test PRIVATE sshash)

if (OPENSSL_FOUND AND WITH_TOOLS) 
	add_executable(sha1-test sha1-test.cc)
	target_link_libraries(sha1-test PRIVATE sshash-utils)
endif()

if (HOGL_FOUND)
	add_executable(hogl-test hogl-test.cc)
	target_link_libraries(hogl-test PRIVATE hogl sshash)
endif()
