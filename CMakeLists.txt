cmake_minimum_required(VERSION 2.8)

set(project_name challenge)
project(${project_name})

function(program task_directory)
	set(solver ${task_directory}_solver)
	set(tester ${task_directory}_tester)
	set(solver_source ${CMAKE_CURRENT_BINARY_DIR}/${task_directory}_solver.cpp)
	set(tester_source ${CMAKE_CURRENT_BINARY_DIR}/${task_directory}_tester.cpp)
	add_executable(${solver} ${solver_source})
	add_executable(${tester} ${tester_source})
	add_custom_command(
		OUTPUT ${solver_source}
		DEPENDS ${task_directory}/main.cpp solver_main.cpp
		COMMAND cat ${task_directory}/main.cpp solver_main.cpp > ${solver_source}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	)
	add_custom_command(
		OUTPUT ${tester_source}
		DEPENDS ${task_directory}/main.cpp tester_main.cpp
		COMMAND cat ${task_directory}/main.cpp tester_main.cpp > ${tester_source}
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	)
	add_custom_command(
		TARGET ${solver} ${tester}
		POST_BUILD
		COMMAND echo "python3.6 ${CMAKE_CURRENT_SOURCE_DIR}/runner.py $<TARGET_FILE:${solver}> $<TARGET_FILE:${tester}>"
		COMMAND python3.6 ${CMAKE_CURRENT_SOURCE_DIR}/runner.py $<TARGET_FILE:${solver}> $<TARGET_FILE:${tester}>
		COMMENT "Testing ${task_directory}"
		VERBATIM)

endfunction()

#program("boolean_function")
#program("intervals")
#program("meet_in_the_middle")
program("square_sweep")
program("timed_paths")
program("tree_centroid")
program("tree_requests")
program("split_tree")