project('vanilla', 'C')

log_s = 'log/log.c'
log_i = include_directories('log')

i = [log_i]
l = static_library(meson.project_name(), log_s,
    c_args: ['-Wall', '-Werror', '-Os'],
    include_directories: i)

subdir('test')
