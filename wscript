#! /usr/bin/env python
# encoding: utf-8

APPNAME = 'ciem'
VERSION = '1.0'

top = '.'
out = 'build'

def options(opt):
    """Add options."""
    opt.load('compiler_cxx')
    opt.add_option('-t',
                   '--test',
                   dest='test',
                   default=False,
                   action='store_true',
                   help='Compile unit test')
    opt.add_option('-g',
               dest='debug',
               default=False,
               action='store_true',
               help='Debug information')


def configure(ctx):
    """Configure context."""
    openssl_ver = '1.0.2g'
    boost_ver = '1.61.0_1'
    cpp_netlib_ver = '0.12.0_1'
    ctx.env.CXXFLAGS = [
        '-std=c++1z',
        '-emit-llvm',
        '-Wall',
        '-Wextra',
        '-Weffc++',
        #'-Wconversion',
        # Clang 3.4
        '-Wheader-guard',
        '-Wlogical-not-parentheses',
        '-Wloop-analysis',
        '-Wuninitialized',
        # Clang 3.5
        '-Wabsolute-value',
        '-Wtautological-pointer-compare',
        '-Wtautological-undefined-compare',
        '-Qunused-arguments',
        '-Wno-missing-braces',
        ]
    if ctx.options.debug:
        ctx.env.DEFINES.append("DEBUG")
        ctx.env.CXXFLAGS.append('-g')
        ctx.env.CXXFLAGS.append('-ftrapv')
    else:
        ctx.env.CXXFLAGS.append('-O3')
        ctx.env.CXXFLAGS.append('-Oz')
    ctx.env.INCLUDES = [
        '.',
        '/usr/local/include/',
        '/usr/local/opt/openssl/include',
        '/usr/local/Cellar/cpp-netlib/{}/include/'.format(cpp_netlib_ver),
        ]
    ctx.env.LIB = [
        'boost_date_time-mt',
        'boost_system-mt',
        'boost_thread-mt',
        'crypto',
        'ssl',
        ]
    ctx.env.LIBPATH = [
        '/usr/local/lib/',
        '/usr/local/opt/openssl/lib',
        '/usr/local/Cellar/cpp-netlib/{}/lib/'.format(cpp_netlib_ver),
        ]
    ctx.env.STLIB = [
        'cppnetlib-client-connections',
        'cppnetlib-uri',
        ]
    ctx.env.LINKFLAGS = [
        '-flto',
        #'-v',
        ]
    if ctx.options.debug:
        ctx.env.LINKFLAGS.append('-g')
    ctx.load('compiler_cxx')

def build(ctx):
    """Build project."""
    ctx.recurse('src')

    if ctx.options.test:
        ctx.recurse('test')
