import os
import subprocess
import sys

class FancyOutput:
	def seconds(self, f):
		return (str(f) + 's').ljust(6) + str('[!]' if f > 2.0 else '   ')

	def memory(self, m):
		return (str(m)).rjust(8) + ' kB ' + (str(m // 1024)).rjust(5) + ' MB'

	def return_code(self, ret):
		return str('ERROR ' + str(ret) if ret != 0 else '').ljust(10)

if __name__ == '__main__':
	if len(sys.argv) != 3:
		sys.stderr.write('Proper usage:\n')
		sys.stderr.write('\n')
		sys.stderr.write('    ' + sys.argv[0] + ' <solver_binary> <tester_binary>\n')
		sys.stderr.write('\n')
		sys.exit(2)

	solver_binary = sys.argv[1]
	tester_binary = sys.argv[2]

	fancy = FancyOutput()

	time_prefix = ['/usr/bin/time', '--format', '%e %S %U %M']

	tester_how_many_tests_process = subprocess.Popen([tester_binary], stdout = subprocess.PIPE, universal_newlines = True)
	tester_how_many_tests_stdout, tester_how_many_tests_stderr = tester_how_many_tests_process.communicate(timeout = 1)
	if (tester_how_many_tests_process.returncode != 0):
		sys.stderr.write('Tester ' + fancy.return_code(tester_how_many_tests_process.returncode))
		sys.exit(1)

	number_of_tests = int(tester_how_many_tests_stdout)

	for test_index in range(number_of_tests):
		test_index_string = str(test_index)
		tester_with_input_process = subprocess.Popen([tester_binary, 'input', test_index_string], stdout = subprocess.PIPE, universal_newlines = True)

		child_process = subprocess.Popen(time_prefix + [solver_binary], stdin = tester_with_input_process.stdout, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)

		try:
			outs, errors = child_process.communicate(timeout = 5)
			tester_with_input_process.communicate(timeout = 5)
			tester_with_output_process = subprocess.Popen([tester_binary, 'output', test_index_string], stdout = subprocess.PIPE, universal_newlines = True)
			tester_outs, tester_errors = tester_with_output_process.communicate(timeout = 5)

			print(test_index_string.rjust(3) + ' # ', end='')
			print_input = False
			horizontal_length = 64
			if (child_process.returncode != 0):
				signal_info = ' (can be signal ' + str(child_process.returncode - 128) + ')' if child_process.returncode > 128 else ''
				print(' FAIL! Process returned ' + str(tester_with_output_process.returncode) + signal_info)
				print_input = True
			else:
				time_output = errors.split() #time writes on stderr
				(clock_time, system_time, user_time, memory_used_in_kilobytes) = (float(time_output[0]), float(time_output[1]), float(time_output[2]), int(time_output[3]))
				invalid_output = (outs != tester_outs)
				print(' FAIL! ' if invalid_output else ' OK    ', end='')
				print(' clock_time: ' + fancy.seconds(clock_time) + ' system_time: ' + fancy.seconds(system_time) + ' user_time: ' + fancy.seconds(user_time) + ' memory: ' + fancy.memory(memory_used_in_kilobytes) + ' ' + fancy.return_code(child_process.returncode) + fancy.return_code(tester_with_input_process.returncode) + fancy.return_code(tester_with_output_process.returncode))
				if invalid_output:
					print_input = True
					print(' solver:'.rjust(horizontal_length, '-'))
					print(outs)
					print(' tester:'.rjust(horizontal_length, '-'))
					print(tester_outs)
			if print_input:
				tester_with_input_process_for_check = subprocess.Popen([tester_binary, 'input', test_index_string], stdout = subprocess.PIPE, universal_newlines = True)
				check_outs, check_errors = tester_with_input_process_for_check.communicate(timeout = 5)
				print(' input:'.rjust(horizontal_length, '-'))
				print(check_outs)


		except subprocess.TimeoutExpired:
			child_process.kill()
			tester_with_input_process.kill()
			tester_with_output_process.kill()
			tester_with_input_process_for_check.kill()
