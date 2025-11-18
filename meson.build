#  Build for Jammbox -- 
project('JammBox', 'c',
  version : '0.1',
  default_options : ['c_std=gnu99', 'warning_level=3'])

cc=meson.get_compiler('c') 


sources=[ 
  'jbox.c', 
  'diskcheck.c'
  ]

executable('JammBox',
           sources,
           install : true)
