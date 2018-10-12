cmake_minimum_required(VERSION 2.8)

set(project_name challenge)
project(${project_name})

add_custom_target(challenge_runner_test_run_target)

add_custom_command(OUTPUT SYMBOLIC challenge_runner_test_run
	DEPENDS ${challenge_runner_test_run_target}
	COMMAND python3.6 runner.py ARGS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "runner.py ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}"
	VERBATIM)

add_custom_target(challenge_runner DEPENDS challenge_runner_test_run)

function(challenge_program task_directory)
	set(challenge_program_binary ${project_name}_${task_directory})
	add_executable(${challenge_program_binary} ${task_directory}/main.cpp)
	add_dependencies(challenge_runner_test_run_target ${challenge_program_binary})
endfunction()

function(challenge_program_with_tester task_directory)
	challenge_program(${task_directory})
	set(challenge_program_with_tester_binary ${project_name}_${task_directory}_tester)
	add_executable(${challenge_program_with_tester_binary} ${task_directory}/main.cpp)
	target_compile_definitions(${challenge_program_with_tester_binary} PRIVATE "TESTER_VERSION=1")
	add_dependencies(challenge_runner_test_run_target ${challenge_program_with_tester_binary})
endfunction()

challenge_program("boolean_function")
challenge_program_with_tester("intervals")
challenge_program("meet_in_the_middle")
challenge_program_with_tester("square_sweep")
challenge_program_with_tester("timed_paths")
challenge_program_with_tester("tree_centroid")
challenge_program("tree_requests")
