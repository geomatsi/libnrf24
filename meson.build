project('libnrf24', ['c', 'cpp'],
	version : '0.1',
	default_options : [ 'buildtype=minsize', 'c_std=c99' ],
)

# nRF24 library

src = [ 'src/rf24.c' ]

if get_option('single_byte_spi')
	add_project_arguments('-DSPI_SINGLE_BYTE', language : ['c', 'cpp'])
	src += [ 'src/rf24_sb_cmds.c' ]
endif

if get_option('multi_byte_spi')
	add_project_arguments('-DSPI_MULTI_BYTE', language : ['c', 'cpp'])
	src += [ 'src/rf24_mb_cmds.c' ]
endif

inc = include_directories('include')
lib = static_library('nrf24', src, include_directories : inc, install : 'true')

libnrf24_dep = declare_dependency(link_with : lib, include_directories : inc)
install_headers('include/RF24.h', 'include/nRF24L01.h')

# Tests

common_test_srcs = [
	'tests/main_test.cpp',
	'src/rf24_sb_cmds.c',
	'src/rf24_mb_cmds.c',
	'src/rf24.c',
	]

sb_spi_test_srcs = [
	'tests/spi_tests.cpp',
	'tests/spi_mock.cpp',
	]

mb_spi_test_srcs = [
	'tests/spi_tests.cpp',
	'tests/mspi_mock.cpp',
	]

core_test_srcs = [
	'tests/core_tests.cpp',
	'tests/uc_tests.cpp',
	'tests/cmd_mock.cpp',
	]

test_inc = include_directories('include', 'tests/include')
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
