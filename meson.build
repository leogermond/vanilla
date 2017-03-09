project('vanilla', 'C')

conf_data = configuration_data()

if get_option('no_stderr')
  conf_data.set('use_dprintf', 0)
  conf_data.set('printf', get_option('printf'))
else
  conf_data.set('use_dprintf', 1)
  conf_data.set('dprintf', get_option('dprintf'))
endif

cfile = configure_file(input:'config.h.in', output: 'config.h', configuration: conf_data)

log_s = 'log/log.c'

vanilla_i = [include_directories('.')]
vanilla = static_library(meson.project_name(), log_s,
    c_args: ['-Wall', '-Werror', '-Os'],
    include_directories: vanilla_i)

if get_option('tests')
  subdir('test')
endif
