common_test_srcs = [
	'main_test.cpp',
	'../src/rf24_sb_cmds.c',
	'../src/rf24_mb_cmds.c',
	'../src/rf24.c',
	]

sb_spi_test_srcs = [
	'spi_tests.cpp',
	'spi_mock.cpp',
	]

mb_spi_test_srcs = [
	'spi_tests.cpp',
	'mspi_mock.cpp',
	]

core_test_srcs = [
	'core_tests.cpp',
	'uc_tests.cpp',
	'cmd_mock.cpp',
	]

cpputest = dependency('cpputest')

test1 = executable('sb_spi_tests',
	[common_test_srcs, sb_spi_test_srcs],
	include_directories : test_inc,
	dependencies : cpputest,
)
test('single-byte xfer spi tests', test1)

test2 = executable('mb_spi_tests',
	[common_test_srcs, mb_spi_test_srcs],
	include_directories : test_inc,
	dependencies : cpputest,
)
test('multi-byte xfer spi tests', test2)

test3 = executable('core_tests',
	[common_test_srcs, core_test_srcs],
	include_directories : test_inc,
	dependencies : cpputest,
)
test('libnrf24 core tests', test3)
