#! /usr/bin/env python
# encoding: utf-8
import os
import platform
import subprocess

APPNAME = 'iem'
VERSION = '0.1.0'

top = '.'
out = 'build'


def options(opt):
    """Add options."""
    opt.load('compiler_cxx')
    opt.add_option('-g',
                   dest='debug',
                   default=False,
                   action='store_true',
                   help='Debug information')
    # http://stackoverflow.com/questions/15008758/parsing-boolean-values-with-argparse
    opt.add_option('--build-static-libs',
                   dest='iem_build_shared_libs',
                   default=True,
                   action='store_false',
                   help='Build IEM with static libraries')
    opt.add_option('--disable-tests',
                   dest='iem_build_tests',
                   default=True,
                   action='store_false',
                   help='Build IEM project tests')

OPENSSL_ROOT_DIR = 'OPENSSL_ROOT_DIR'


def _root_dir(package_name):
    if platform.system() == 'Darwin':  # Apple system
        # if 'OpenSSL_DIR' not in os.environ:
        args = ['which', 'brew']
        brew_cp = subprocess.run(args=args, stdout=subprocess.PIPE)
        if not brew_cp.stdout:
            print("Homebrew not found: not using Homebrew's OpenSSL")
            if OPENSSL_ROOT_DIR not in os.environ:
                print("Use -DOPENSSL_ROOT_DIR for non-Apple OpenSSL")
        else:
            args = ['brew', '--prefix', package_name]
            open_ssl_cp = subprocess.run(args, stdout=subprocess.PIPE)
            return str(open_ssl_cp.stdout, encoding='utf-8').rstrip('\n')
    raise NotImplementedError('Only macOS and Homebrew supported currently')


def definitions(ctx):
    defs = []


def configure(ctx):
    """Configure context."""
    openssl_root_dir = _root_dir('openssl')
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
        '-DBOOST_TEST_DYN_LINK',
        '-DBOOST_NETWORK_ENABLE_HTTPS',
        '-DBOOST_NETWORK_DEBUG',
        ]
    if ctx.options.debug:
        ctx.env.DEFINES.append("DEBUG")
        ctx.env.CXXFLAGS.append('-g')
        ctx.env.CXXFLAGS.append('-ftrapv')
    else:
        ctx.env.CXXFLAGS.append('-O3')
        ctx.env.CXXFLAGS.append('-Oz')
    cppnetlib_root_dir = _root_dir('cpp-netlib')
    ctx.env.INCLUDES = [
        '.',
        '/usr/local/include/',
        '{}/include/'.format(openssl_root_dir),
        '{}/include/'.format(cppnetlib_root_dir),
        ]
    print(ctx.env.INCLUDES)
    ctx.env.LIB = [
        'boost_date_time-mt',
        'boost-filesystem-mt',
        'boost_system-mt',
        'boost_thread-mt',
        'crypto',
        'ssl',
        ]
    ctx.env.LIBPATH = [
        '/usr/local/lib/',
        '{}/lib/'.format(openssl_root_dir),
        '{}/lib/'.format(cppnetlib_root_dir),
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

    if ctx.options.iem_build_tests:
        ctx.recurse('test')
