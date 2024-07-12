project('lid_monitor', 'c')

dep_libudev = dependency('libudev')
dep_libinput = dependency('libinput')

executable('lid_monitor', 'lid_monitor.c', 
    dependencies: [dep_libudev, dep_libinput])