project('stormbird', 'cpp',
	license: 'MPL-2.0',
	version: '0.0.1',
	default_options: [
		'prefix=/usr',
		'warning_level=3',
		'cpp_std=c++20'
	],
    meson_version: '>=1.2.1')

compiler = meson.get_compiler('cpp')

if get_option('buildtype') == 'custom'
    opt_args = compiler.get_supported_arguments(
        '-march=knl',
        '-mtune=knl',
        '-fopenmp'
    )

    if compiler.get_id() == 'clang'
        add_project_arguments('-Ofast', language: 'cpp')

        if compiler.compiles('void test() { }', name: 'has polly', args: ['-mllvm', '-polly'])
            opt_args += ['-mllvm', '-polly']

            if compiler.compiles('void test() { }', name: 'has polly vectorizer', args: ['-mllvm', '-polly-vectorizer=stripmine'])
                opt_args += ['-mllvm', '-polly-vectorizer=stripmine']
            endif

            if compiler.compiles('void test() { }', name: 'has polly parallel', args: ['-mllvm', '-polly-parallel'])
                opt_args += ['-mllvm', '-polly-parallel']
            endif

            if compiler.compiles('void test() { }', name: 'has polly omp backend', args: ['-mllvm', '-polly-omp-backend=LLVM'])
                opt_args += ['-mllvm', '-polly-omp-backend=LLVM']
            endif

            if compiler.compiles('void test() { }', name: 'has polly num threads', args: ['-mllvm', '-polly-num-threads=24'])
                opt_args += ['-mllvm', '-polly-num-threads=24']
            endif

            if compiler.compiles('void test() { }', name: 'has polly scheduling', args: ['-mllvm', '-polly-scheduling=dynamic'])
                opt_args += ['-mllvm', '-polly-scheduling=dynamic']
            endif

            if compiler.compiles('void test() { }', name: 'has polly scheduling chunksize', args: ['-mllvm', '-polly-scheduling-chunksize=1'])
                opt_args += ['-mllvm', '-polly-scheduling-chunksize=1']
            endif
        endif
    else
        if compiler.get_id() == 'msvc'
            opt_args += ['/Ox']
        else
            opt_args += ['-O2']
        endif
    endif

    add_global_arguments(opt_args, language: 'cpp')
endif

# public dependencies
deps = []
if not compiler.check_header('ankerl/unordered_dense.h')
    deps += [dependency('unordered_dense', version: '>=4.1.1')]
endif

# cross dependencies between cli and stormbird
nlohmann_json_dep = dependency('', required: false)
if not compiler.check_header('nlohmann/json.hpp')
    nlohmann_json_dep = dependency('nlohmann_json', version: '>=3.11.2')
endif

# cli dependencies
cli_deps = [] + deps
#if not compiler.check_header('clipp.h')
#    cli_deps += [dependency('clipp', version: '>=1.2.4')]
#endif
cli_deps += [nlohmann_json_dep]

# stormbird dependencies
stormbird_deps = [] + deps
stormbird_deps += [nlohmann_json_dep]

libdeps = []

cpp_args = []

cpp_args = compiler.get_supported_arguments(
    '/wd4251', # do i need to define my own template exports? kinda cringe! do better msvc
    '/wd4913',
    '/utf-8',
    '/bigobj',
    '/EHsc',
    '-fPIC',
    '-finput-charset=utf-8',
    '-fexec-charset=utf-8',
    '-faddrsig',
    '-faligned-new',
    '-fcoroutines'
)

add_project_arguments(cpp_args, language: 'cpp')

icon_sources = []
if host_machine.system() == 'windows'
    if import('fs').is_file('resources/icon.ico')
        icon_sources += import('windows').compile_resources(
            'resources/stormbird.rc',
            include_directories : [meson.current_build_dir(), 'resources'],
        )
    endif
endif

# subdir('src')

# if not get_option('lib_only')
#    subdir('cli')
# endif

subdir('stormbird_hook')

install_subdir('include/',
	install_dir: 'include/',
	strip_directory: true)
