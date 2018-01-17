project('libnrf24', 'c',
	version : '0.1',
	default_options : [ 'buildtype=minsize', 'c_std=c99' ],
)

src = [ 'src/rf24.c' ]

if get_option('single_byte_spi')
	add_project_arguments('-DSPI_SINGLE_BYTE', language : 'c')
	src += [ 'src/rf24_sb_cmds.c' ]
endif

if get_option('multi_byte_spi')
	add_project_arguments('-DSPI_MULTI_BYTE', language : 'c')
	src += [ 'src/rf24_mb_cmds.c' ]
endif

inc = include_directories('include')
lib = static_library('nrf24', src, include_directories : inc, install : 'true')

libnrf24_dep = declare_dependency(link_with : lib, include_directories : inc)
install_headers('include/RF24.h', 'include/nRF24L01.h')

