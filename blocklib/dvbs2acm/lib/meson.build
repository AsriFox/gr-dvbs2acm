dvbs2acm_sources += []
dvbs2acm_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, spdlog_dep, pmt_dep, python3_embed_dep, json_dep]

block_cpp_args = ['-DHAVE_CPU']

# if IMPLEMENT_CUDA
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_dvbs2acm_cu = library('gnuradio-blocklib-dvbs2acm-cu', 
#         dvbs2acm_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_dvbs2acm_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_dvbs2acm_cu,
#                         dependencies : cuda_dep)

#     dvbs2acm_deps += [gnuradio_blocklib_dvbs2acm_cu_dep, cuda_dep]

# endif

incdir = include_directories(['../include/gnuradio/dvbs2acm','../include'])
gnuradio_blocklib_dvbs2acm_lib = library('gnuradio-blocklib-dvbs2acm', 
    dvbs2acm_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : dvbs2acm_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_dvbs2acm_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_dvbs2acm_lib,
                       dependencies : dvbs2acm_deps)

pkg = import('pkgconfig')
libs = [gnuradio_blocklib_dvbs2acm_lib]     # the library/libraries users need to link against
h = ['.'] # subdirectories of ${prefix}/${includedir} to add to header path
pkg.generate(libraries : libs,
             subdirs : h,
             version : meson.project_version(),
             name : 'libgnuradio-blocklib-dvbs2acm',
             filebase : 'gnuradio-blocklib-dvbs2acm',
             description : 'GNU Radio dvbs2acm component')
