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
  '-D_USE_ZIP_ARCHIVE=1',
  language:'c'
  )


sources=[ 
  'src/jbox.c', 
  'src/diskcheck.c',
  'src/dboxutils.c',
  'src/archive.c',
  'src/ui.c'
  ]
libncurses =dependency('curses', not_found_message:'Missing ncurses  library...', required:false)   
if libncurses.found()  
  add_global_arguments('-DJBOX_TUI_MENU=1',  language:'c')
endif  

libzip =dependency('libzip' , version:'>=1.11.0' ,required:false , 
  not_found_message:'It\'s  highly recommended to have libzip library to proceed.' ) 
if not libzip.found() 
  add_global_arguments('-D_USE_ZIP_ARCHIVE=0' , language:'c') 
endif 

#+ Configuration  
cfg_data = configuration_data() 
cfg_data.set('version' , meson.version()) 
cfg_data.set('progname', meson.project_name())

configure_file(  
  configuration :cfg_data , 
  input : 'jboxconfig.h.in',
  output: 'jboxconfig.h'
  )



inc=include_directories('src') 

executable('JammBox',
           sources,
           dependencies:[libzip , libncurses],
           include_directories:['src'],
           install : true)
