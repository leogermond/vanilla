project('vanilla', 'C')

conf_data = configuration_data()

if get_option('no_stderr')
    conf_data.set('use_dprintf', 0)
    conf_data.set('printf', get_option('printf'))
else
    conf_data.set('use_dprintf', 1)
    conf_data.set('dprintf', get_option('dprintf'))
endif

cfile = configure_file(input:'config.h.in', output: 'config.h',
                       configuration: conf_data)

log_s = 'log/log.c'

include_directories = [include_directories('.')]
static_library = static_library(meson.project_name(), log_s,
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
