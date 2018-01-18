sb_spi_test_srcs = [
	'main_test.cpp',
	'spi_tests.cpp',
	'spi_mock.cpp',
	]

mb_spi_test_srcs = [
	'main_test.cpp',
	'spi_tests.cpp',
	'mspi_mock.cpp',
	]

core_test_srcs = [
	'main_test.cpp',
	'core_tests.cpp',
	'uc_tests.cpp',
	'cmd_mock.cpp',
	]

cpputest = dependency('cpputest')

test1 = executable('sb_spi_tests',
	sb_spi_test_srcs,
	include_directories : test_inc,
	link_with : libnrf24,
	dependencies : cpputest,
)
test('single-byte xfer spi tests', test1)

test2 = executable('mb_spi_tests',
	mb_spi_test_srcs,
	include_directories : test_inc,
	link_with : libnrf24,
	dependencies : cpputest,
)
test('multi-byte xfer spi tests', test2)

test3 = executable('core_tests',
	core_test_srcs,
	include_directories : test_inc,
	link_with : libnrf24,
	dependencies : cpputest,
)
test('libnrf24 core tests', test3)
