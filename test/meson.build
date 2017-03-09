cc = meson.get_compiler('c')
e = executable('test_log', 'test_log.c',
    c_args: ['-Wall', '-Werror'],
    link_with: vanilla,
    include_directories: vanilla_i)
test('test_logs', e)
