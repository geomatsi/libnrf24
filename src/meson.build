src = [ 'rf24.c' ]

if get_option('single_byte_spi')
	add_project_arguments('-DSPI_SINGLE_BYTE', language : ['c', 'cpp'])
	src += [ 'rf24_sb_cmds.c' ]
endif

if get_option('multi_byte_spi')
	add_project_arguments('-DSPI_MULTI_BYTE', language : ['c', 'cpp'])
	src += [ 'rf24_mb_cmds.c' ]
endif


libnrf24 = static_library('nrf24', src, include_directories : main_inc, install : 'true')
libnrf24_dep = declare_dependency(link_with : libnrf24, include_directories : main_inc)
