project('libnrf24', ['c', 'cpp'],
	version : '0.1',
	default_options : [ 'buildtype=minsize', 'c_std=c99' ],
)

main_inc = include_directories('include')
test_inc = include_directories('include', 'tests/include')

subdir('include')
subdir('src')
subdir('tests')
