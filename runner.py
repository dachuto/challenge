import os
import subprocess
import sys

def all_subdirs(top = '.'):
	for d in os.listdir(top):
		full_path = os.path.join(top, d)
		if os.path.isdir(full_path):
			yield d


class FancyOutput:
	def seconds(self, f):
		return (str(f) + 's').ljust(6) + str('[!]' if f > 2.0 else '   ')

	def memory(self, m):
		return (str(m)).rjust(8) + ' kB ' + (str(m // 1024)).rjust(5) + ' MB'

	def return_code(self, ret):
		return str('---===>>> ERROR ' + str(ret) + ' <<<===---' if ret != 0 else '').ljust(32)

if __name__ == "__main__":
	if len(sys.argv) != 3:
		sys.stderr.write('Proper usage:\n')
		sys.stderr.write('\n')
		sys.stderr.write('    ' + sys.argv[0] + ' <path_to_sources> <path_to_binaries>\n')
		sys.stderr.write('\n')
		sys.stderr.write('Note: launch from source dir.\n')
		sys.exit(2)

	path_to_sources = sys.argv[1]
	path_to_binaries = sys.argv[2]

	latest_subdir = max(all_subdirs(path_to_sources), key = lambda d: os.path.getmtime(os.path.join(path_to_sources, d)))

	fancy = FancyOutput()

	print('Testing:')
	print()
	print(latest_subdir)
	print()

	time_prefix = ['/usr/bin/time', '--format', '%e %S %U %M']
	app_binary_name = 'challenge_' + latest_subdir
	app_binary_path = os.path.join(path_to_binaries, app_binary_name)
	app_tester_binary_name = app_binary_name + '_tester'
	app_tester_binary_path = os.path.join(path_to_binaries, app_tester_binary_name)

	tester_how_many_tests_process = subprocess.Popen([app_tester_binary_path], stdout = subprocess.PIPE, universal_newlines = True)
	tester_how_many_tests_stdout, tester_how_many_tests_stderr = tester_how_many_tests_process.communicate(timeout = 1)
	if (tester_how_many_tests_process.returncode != 0):
		sys.stderr.write('Tester ' + fancy.return_code(tester_how_many_tests_process.returncode))
		sys.exit(1)

	number_of_tests = int(tester_how_many_tests_stdout)

	for test_index in range(number_of_tests):
		test_index_string = str(test_index)
		tester_with_input_process = subprocess.Popen([app_tester_binary_path, 'input', test_index_string], stdout = subprocess.PIPE, universal_newlines = True)

		child_process = subprocess.Popen(time_prefix + [app_binary_path], stdin = tester_with_input_process.stdout, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True)

		try:
			outs, errors = child_process.communicate(timeout = 5)
			tester_with_input_process.communicate(timeout = 5)
			tester_with_output_process = subprocess.Popen([app_tester_binary_path, 'output', test_index_string], stdout = subprocess.PIPE, universal_newlines = True)
			tester_outs, tester_errors = tester_with_output_process.communicate(timeout = 5)

			time_output = errors.split() #time writes on stderr
			(clock_time, system_time, user_time, memory_used_in_kilobytes) = (float(time_output[0]), float(time_output[1]), float(time_output[2]), int(time_output[3]))

			print(test_index_string.rjust(3) + ' # TEST' + ' clock_time: ' + fancy.seconds(clock_time) + ' system_time: ' + fancy.seconds(system_time) + ' user_time: ' + fancy.seconds(user_time) + ' memory: ' + fancy.memory(memory_used_in_kilobytes) + ' ' + fancy.return_code(child_process.returncode) + fancy.return_code(tester_with_input_process.returncode) + fancy.return_code(tester_with_output_process.returncode))
			if (outs != tester_outs):
				horizontal_length = 64
				print(outs)
				print(' Expected:'.rjust(horizontal_length, '='))
				print(tester_outs)
				print(' '.rjust(horizontal_length, '='))
		except subprocess.TimeoutExpired:
			child_process.kill()
			tester_with_input_process.kill()
			tester_with_output_process.kill()
