project('gr-dvbs2acm', 'cpp',
    version: '0.0.0',
    license: 'GPLv3',
    default_options: [
        'cpp_std=c++20',
        'default_library=shared',
        'buildtype=release',
        'libdir=lib'
    ])

# Import python3 meson module which is used to find the Python dependencies.
# Python3 is required for code generation
py3 = import('python').find_installation('python3')
py3_version = py3.language_version()
if py3_version.version_compare('< 3.6')
    error('Invalid python version! Must have >=3.6')
endif

GR_ENABLE_PYTHON = get_option('enable_python')
if GR_ENABLE_PYTHON
    python3_dep = dependency('python3')
    python3_embed_dep = dependency('python3-embed')
    pybind11_dep = dependency('pybind11')
    incdir_numpy = run_command(py3, 
        ['-c', 'import os; os.chdir(".."); import numpy; print(numpy.get_include())'],
        check: true
    ).stdout().strip()
endif

json_dep = dependency('nlohmann_json')

if get_option('enable_testing')
    TEST_ENV = environment()
    TEST_ENV.prepend('LD_LIBRARY_PATH',
        join_paths(meson.build_root(), 'blocklib', 'dvbs2acm', get_option('libdir')))
    TEST_ENV.prepend('PYTHONPATH', 
        join_paths(meson.build_root(), 'blocklib', 'dvbs2acm', 'python'))
endif

prefix = get_option('prefix')
# Escape spaces
prefix_parts = prefix.split(' ')
prefix = '\ '.join(prefix_parts)

cuda_available = add_languages('cuda', required: false)
IMPLEMENT_CUDA = cuda_available and get_option('enable_cuda')
IMPLEMENT_CPU = true

###############################################################################
# Dependencies
###############################################################################
gnuradio_gr_dep = dependency('gnuradio-runtime', version: '>=3.10')
volk_dep = dependency('volk', version: '>=2.2')
fmt_dep = dependency('fmt', method: 'cmake', modules: ['fmt::fmt'])
spdlog_dep = dependency('spdlog')
pmt_dep = dependency('pmt')

SCRIPTS_DIR = join_paths(meson.project_source_root(), 'utils', 'blockbuilder', 'scripts')

if GR_ENABLE_PYTHON
    run_command('python3', join_paths(SCRIPTS_DIR, 'gen_meson.py'),
        join_paths(meson.project_source_root(), 'blocklib'),
        '--enable_python',
        check: true)
else
    run_command('python3', join_paths(SCRIPTS_DIR, 'gen_meson.py'),
        join_paths(meson.project_source_root(), 'blocklib'),
        check: true)
endif

subdir('blocklib/dvbs2acm')
