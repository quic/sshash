set(SSHASH_HPP
	${PROJECT_SOURCE_DIR}/include/sshash/macros.hpp
	${PROJECT_SOURCE_DIR}/include/sshash/map.hpp)
set(SSHASH_CC map.cc)
add_library(sshash ${SSHASH_HPP} ${SSHASH_CC})

set(SSHASH_LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/src/sshash.link CACHE PATH "..." FORCE)

# Enable PIC even though we're static
set_target_properties(sshash PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(sshash PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Include our linker script for merging .sshash.str sections
target_link_libraries(sshash INTERFACE "-T${SSHASH_LINKER_SCRIPT}")

install(TARGETS sshash DESTINATION lib COMPONENT dev)
install(FILES ${SSHASH_HPP} DESTINATION include/sshash COMPONENT dev)
install(FILES ${SSHASH_LINKER_SCRIPT} DESTINATION lib/sshash COMPONENT dev)

if (HOGL_FOUND)
	add_library(sshash-fmt-plugin SHARED ssfmt-plugin.cc)
	target_link_libraries(sshash-fmt-plugin PRIVATE hogl sshash)
	install(TARGETS sshash-fmt-plugin DESTINATION lib COMPONENT dev)
endif()
