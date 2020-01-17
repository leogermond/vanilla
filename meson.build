project('vanilla', 'C')

conf_data = configuration_data()

conf_data.set10('use_dprintf', get_option('no_stderr'))
conf_data.set('printf', get_option('printf'))
conf_data.set('dprintf', get_option('dprintf'))

cfile = configure_file(input:'config.h.in', output: 'config.h',
                       configuration: conf_data)

sources = []

if get_option('include_guard')
    include_directories = include_directories('.')
else
    include_directories = include_directories('vanilla')
endif
subdir('vanilla')

assert(sources.length() != 0, 'No sources specified')
static_library = static_library(meson.project_name(),
    sources,
    c_args: ['-Wall', '-Werror', '-Os'],
    include_directories: include_directories)

if get_option('tests')
    tests = []
    subdir('test')

    foreach t : tests
        e = executable(t[0], t[1],
        c_args: ['-Wall', '-Werror'],
        link_with: static_library,
        include_directories: include_directories)
        test(t[0], e)
    endforeach
endif
