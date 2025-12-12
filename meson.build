#  Build for Jammbox -- 
project('JammBox', 'c',
  version : '0.1',
  default_options : ['c_std=gnu99', 'warning_level=3'])

cc=meson.get_compiler('c') 

add_project_arguments(
  cc.get_supported_arguments(
    '-Wformat',
    '-Wunused',
    '-Wunused-parameter'
    ),
  '-D_USE_ZIP_ARCHIVE',
  language:'c'
  )

sources=[ 
  'src/jbox.c', 
  'src/diskcheck.c',
  'src/dboxutils.c',
  'src/archive.c'
  ]

libzip =dependency('libzip' , version:'>=1.11.0' ,required:true)   
inc=include_directories('src') 

executable('JammBox',
           sources,
           dependencies:[libzip],
           include_directories:['src'],
           install : true)
