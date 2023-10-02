#!/usr/bin/env python3

# JFDI is
# Copyright (C) 2016-2021 Frogtoss Games, Inc.
#
# Author Michael Labbe
# See LICENSE in this repo for license terms
#
# latest version, examples and documentation:
# https://github.com/mlabbe/jfdi.git

# todo:
# - handle OSError could not rmdir because a dos prompt is in it

_cfg = {'verbose': False,
        'build_vars': {} }

import sys
if sys.version_info[0] < 3:
    sys.stderr.write('JFDI requires Python 3\n')
    sys.stderr.write('Alternatively: Official github repo offers standalone exes for windows')
    sys.stderr.write('https://github.com/mlabbe/jfdi')
    sys.exit(1)

import os
import sys
import glob
import time
import shutil
import os.path
import argparse
import platform
import subprocess

VERSION=(1,0,6)

g_start_time = time.time()

def _is_jfdi_compatible_with_build_script_version():
    """is this version of jfdi.py compatible with the build format version?
    The value is stored in JFDI_VERSION in the generated template.

    None return value means it's compatible"""
    script_version = int(globals()['JFDI_VERSION'])
    
    if script_version == 1:
        return None
    
    elif script_version > 1:
        return " is too old for build script JFDI_VERSION %d" % (script_version)
    
    elif script_version < 1:
        return " is too new for build script JFDI_VERSION %s" % (script_version)

    
class _ArgDispatch:
    """subcommand-based argument processing.  ie:
    jfdi init  [args, options]
    jfdi clean [args, options]
    jfdi build [args, options]

    Also supports implicit subcommand for build:
    jfdi [args, options]
    """
    def dispatch(self):
        desc = "JFDI Simple Build System version %s" % (_pp_version())
        p = argparse.ArgumentParser(
            description=desc,
            usage='''%(prog)s <subcommand> [options] [buildvar=value ...]

The most commonly used jfdi subcommands are:

    {jfdi} init    # create new build.jfdi from template
    {jfdi} build   # build your project
    {jfdi} clean   # clean your project
    {jfdi} run     # run your built project

More help topics:

    {jfdi} help buildvars          
'''.format(jfdi='%(prog)s'),
            
            epilog="%(prog)s <subcommand> --help for detailed help")

        p.add_argument('subcommand', help='Subcommand to run (omit to build)',
                       nargs='?',
                       default='build')

        subcommand = sys.argv[1:2]

        # build is implicit subcommand; detect buildvar as first arg
        # and provide workaround.
        if len(subcommand) > 0 and '=' in subcommand[0]:
            subcommand = ['build']

        # as above, but for actual arguments
        if len(subcommand) > 0 and subcommand[0][0] == '-':
            if subcommand[0] != '-h' and subcommand[0] != '--help':
                subcommand = ['build']

        top_args = p.parse_args(subcommand)

        # default to 'build' if no subcommand is specified
        if not hasattr(self, 'subcommand_'+top_args.subcommand):
            _fatal_error("unknown subcommand %s.\n" % top_args.subcommand +
                         "Use --help for detailed help")

        # call method named after command.  if it returns at all, return a tuple:
        #
        # [0]: subcommand (implicit build is made explicit)
        # [1]: args namespace for subcommand
        # [2]: build vars dictionary
        #
        # callers are responsible for 1&2
        sub_args, build_vars = getattr(self, 'subcommand_'+top_args.subcommand)()

        # adjust global config from sub args
        global _cfg
        _cfg['verbose'] = sub_args.verbose
        _cfg['build_vars'] = build_vars
        
        return (top_args.subcommand, sub_args, build_vars)

    @staticmethod
    def _subcommand_prog(sub_name):
        return "%s %s" % (sys.argv[0], sub_name)
    
    @staticmethod
    def _add_common_args(p):
        """add args common to all subcommands
        """
        p.add_argument('-f', '--file',
                       help='read FILE as build.jfdi',
                       default='build.jfdi')

        p.add_argument('-v', '--verbose',
                       help="increase log verbosity",
                       action='store_true')

        p.add_argument('--target-os',
                       help='specify TARGET_OS for cross compiling',
                       default=platform.system())
        
        return p
        
    
    @staticmethod
    def _parse_build_vars(unknown_args):
        """build vars are KEY=value variables that are passed on the command
        line.  parse them out of unknown args from parse_known_args
        and return a dict.
        """
        # unknown arg parse sets variables 
        vars = {}
        for v in unknown_args:
            var = v.split('=', 1)

            # if no equals sign exists in the unknown argument, the argument
            # is not a buildvar and is truly unknown
            if len(var) == 1:
                _fatal_error('"%s": unknown argument\n'% v)
            
            if len(var[1]) == 0:
                _fatal_error('"%s": buildvar must have value\n' % v)
            
            # all vars are uppercase
            ukey = var[0].upper()

            if ukey in vars:
                _fatal_error('"%s": buildvar specified multiple times\n' % v)

            vars[ukey] = var[1]

        return vars
        

    def subcommand_init(self):
        p = argparse.ArgumentParser(
            description='init creates a new build.jfdi in the working directory',
            prog=self._subcommand_prog('init'),
        )

        p = self._add_common_args(p)

        sub_args = p.parse_args(sys.argv[2:])

        return sub_args, {} # no build vars
        

    def subcommand_clean(self): 
       
        p = argparse.ArgumentParser(
            description='clean intermediate and build product files by calling clean() in build.jfdi',
            prog=self._subcommand_prog('clean')
        )

        p = self._add_common_args(p)
        
        sub_args, unknown_args = p.parse_known_args(sys.argv[2:])
        build_vars = self._parse_build_vars(unknown_args)

        return sub_args, build_vars

    
    def subcommand_build(self):

        p = argparse.ArgumentParser(
            description="build the program by executing build.jfdi",
            prog=self._subcommand_prog('build'),
        )

        p = self._add_common_args(p)
        
        p.add_argument('-r', '--run', help='call run() after successful build',
                       action='store_true')
        p.add_argument('--version', help='print version and exit',
                       action='store_true')

        # work around implicit build subcommand
        first_arg = 2
        if len(sys.argv) > 1 and sys.argv[1] != 'build':
            first_arg = 1

        sub_args, unknown_args = p.parse_known_args(sys.argv[first_arg:])
        build_vars = self._parse_build_vars(unknown_args)

        return sub_args, build_vars

    
    def subcommand_run(self):

        p = argparse.ArgumentParser(
            description="perform a canonical run of the program by calling run() in build.jfdi",
            prog=self._subcommand_prog('run'),
        )

        p = self._add_common_args(p)

        sub_args, unknown_args = p.parse_known_args(sys.argv[2:])
        build_vars = self._parse_build_vars(unknown_args)

        return sub_args, build_vars

    
    def subcommand_help(self):

        p = argparse.ArgumentParser(
            description="additional help topics",
            prog=self._subcommand_prog('help'),
        )

        p.add_argument('topic', help='Help topic. Use --help to see topics')

        sub_args = p.parse_args(sys.argv[2:])
        sub_args.verbose = False
        

        return sub_args, {}


def _which(file):
    for path in os.environ["PATH"].split(os.pathsep):
        if os.path.exists(os.path.join(path, file)):
            return os.path.join(path, file)
    return None

def _pp_version():
    """pretty print version as a string"""
    return '.'.join(str(i) for i in VERSION)

def _clean(context, target_os):
    globals()['TARGET_OS'] = target_os
    _message(1, "cleaning")
    input_files = context[0]['list_input_files']()
    input_files = _handle_input_files(input_files)
    
    context[0]['clean'](input_files)
    # returning from clean means the calling script did not die(), and so
    # it was a success.
    


def _message(verbosity, in_msg):
    global _cfg
    
    if in_msg.__class__ == list:
        msg = ' '.join(in_msg)
    else:
        msg = in_msg
        
    if verbosity >= 1 and not _cfg['verbose']:
        return
    print("%s %s" % (_log_stamp(), msg))

def _warning(msg):
    sys.stderr.write("%s WARNING: %s" % (_log_stamp(), msg))

def _fatal_error(msg, error_code=1):
    sys.stderr.write(_log_stamp() + ' FATAL: ')
    sys.stderr.write(msg)
    sys.stderr.write("exiting with error code %d\n" % error_code)
    sys.exit(error_code)

def _get_script(args_file):
    """compiled contents of script or error out"""
    DEFAULT_SCRIPT = 'build.jfdi'

    script_path = None
    if args_file != None:
        script_path = args_file
    elif os.path.exists(DEFAULT_SCRIPT):
        script_path = DEFAULT_SCRIPT
        
    script_path = None
    if os.path.exists(DEFAULT_SCRIPT):
        script_path = DEFAULT_SCRIPT

    if args_file != None:
        script_path = args_file

    if script_path == None or not os.path.exists(script_path):
        fatal_msg =  "Build file not found\n"
        fatal_msg += "\nIf this is your first run, use %s init\n" \
                     % sys.argv[0]
        fatal_msg += "%s --help for detailed help.\n\n" \
                     % sys.argv[0]
        _fatal_error(fatal_msg)

    with open(script_path) as f:
        script = f.read()

    try:
        pycode = compile(script, script_path, mode='exec')
    except SyntaxError as ex:
        msg =  "SyntaxError in (%s, line %d):\n\t%s\n" \
               % (ex.filename, ex.lineno, ex.text)
        _fatal_error(msg)
    return pycode

def _swap_slashes(dir):
    if platform.system() == 'Windows':
        return dir.replace('/', '\\')
    else:
        return dir.replace('\\', '/')


def _add_api(g):
    g['ext'] = _api_ext
    g['log'] = _api_log
    g['mkd'] = _api_mkd
    g['cmd'] = _api_cmd
    g['die'] = _api_die
    g['cp']  = _api_cp
    g['rm']  = _api_rm
    g['env'] = _api_env
    g['use'] = _api_use
    g['arg'] = _api_arg
    g['obj'] = _api_obj
    g['var'] = _api_var
    g['yes'] = _api_yes
    g['exe'] = _api_exe
    g['exp'] = _api_exp
    g['pth'] = _api_pth
    g['raw'] = _api_raw
    return g

def _run_script(pycode, target_os):
    globals()['TARGET_OS'] = target_os    
    g = _add_api(globals())

    push_name = globals()['__name__']
    globals()['__name__'] = '__jfdi__'
    exec(pycode, g)
    globals()['__name__'] = push_name

    #
    # validate expected functions
    #
    
    # todo: fill this out
    missing_msg = ""
    if 'list_input_files' not in g:
        missing_msg += "list_input_files() must exist\n"

    if 'JFDI_VERSION' not in g:
        missing_msg += "JFDI_VERSION must exist in build script\n"

    if len(missing_msg) != 0:
        sys.stderr.write("errors were found during execution:\n")
        _fatal_error(missing_msg)

    #
    # validate version compatibility
    #
    error_result = _is_jfdi_compatible_with_build_script_version()
    if error_result != None:
        _fatal_error("JFDI %s%s\n" % (_pp_version(), error_result))
        
    
    context = [globals()]
    return context

def _handle_input_files(input_files):
    if input_files.__class__ == str:
        input_files = [input_files]

    out_paths = []
        
    for entry in input_files:
        if '*' in entry:
            wildcard = glob.glob(entry)
            for path in wildcard:
                out_paths.append(path)
        else:
            out_paths.append(entry)

    return out_paths


def _build(context, target_os):
    globals()['HOST_OS'] = platform.system()
    globals()['TARGET_OS'] = target_os
        
    input_files = context[0]['list_input_files']()
    input_files = _handle_input_files(input_files)

    context[0]['start_build']()
        
    cmd_list = []
    for path in input_files:
        cmd = context[0]['build_this'](path)
        if cmd != None:
            cmd_list.append(cmd)

    _message(1, "building %d/%d file(s)" %
             (len(cmd_list), len(input_files)))
    
    for cmd in cmd_list:
        _run_cmd(cmd)

    context[0]['end_build'](input_files)

    
def _canonical_run(context, target_os):
    # not an error to have this omitted in the build script; run() is optional
    if 'run' not in context[0]:
        return

    globals()['HOST_OS'] = platform.system()
    globals()['TARGET_OS'] = target_os

    _message(1, "performing a canonical run of the build product")
    globals()['TARGET_OS'] = target_os    
    context[0]['run']()

def _run_cmd(cmd):
    _message(0, cmd)
    exit_code = subprocess.call(cmd, shell=True)
    if exit_code != 0:
        _fatal_error("error '%d' running command \"%s\"\n" %
                     (exit_code, cmd))

def _report_success(start_time):
    end_time = time.time()
    delta_time = end_time - start_time
    _message(0, "exiting with success in %.1f seconds." % delta_time)

def _str_to_list(val):
    """If val is str, return list with single entry, else return as-is."""
    l = []
    if val.__class__ == str:
        l.append(val)
        return l
    else:
        return val

def _list_single_to_str(val):
    """If val is len(list) 1, return first entry, else return as-is."""
    if val.__class__ == list and len(val) == 1:
        return val[0]
    else:
        return val


def _log_stamp():
    # strategy is to format in milliseconds until the build time
    # gets long, then format in terms of seconds.
    
    justify_chars = 6
    cur_time = time.time()

    d_ms = int((cur_time - g_start_time) * 1000)

    if d_ms < 1000:
        return "[%s ms]" % str(d_ms).rjust(justify_chars-2)
    else:
        d_s = cur_time - g_start_time

        num = "%.1f" % d_s
        return "[%ss]" % num.rjust(justify_chars)

def _display_help_topic(topic):
    if topic == 'buildvars':
        msg='''Build Variables are KEY=value pairs specified on the commandline.
In a build script, var('KEY') returns the value specified.

Build variables have the following properties:

 REGARDING CASE:
 - They are case insensitive. var('key') and var('KEY') are the same thing.
 
 REGARDING FALSE/UNDEF IN BUILD SCRIPTS:
 - var('KEY') returns string, yes('KEY') returns boolean
 - Boolean KEY=0 on the command line should be evaulated as if yes('KEY')

 REGARDING UNSPECIFIED BUILD VARS:
 - var('OMITTED') == ''
 - yes('OMITTED') == False

Example buildvar usage:
 jfdi DEBUG=1

Corresponding buildvar code:
 if yes('debug'):
    CFLAGS.append('-g')

'''

    else:
        _fatal_error("unknown help topic %s.  " % topic +
                     "Use --help to view all help topics\n")

    print(msg)
    sys.exit(0)
    
        
def generate_tmpl(path):
    if os.path.exists(path):
        _fatal_error("%s already exists.\n" % path)

    f = open(path, "wt")
    f.write("""\
#    _______________ _____ 
#   |_  |  ___|  _  \_   _|
#     | | |_  | | | | | |  
#     | |  _| | | | | | |  
# /\__/ / |   | |/ / _| |_ 
# \____/\_|   |___/  \___/ 
#
# NOTE:
# if you do not have jfdi.py, run this script with python to get it.
# or, git clone https://github.com/mlabbe/jfdi
\"""
jfdi build script

available functions:
  cp(src, dst)  - copy a file or directory
  rm(str|iter)  - remove file or directory
  arg(str)      - convert a /flag into a -flag depending on compiler
  use('?')      - add make-like variables (LD, CC, etc.). gcc, clang, msvc
  cmd(list|str) - run a command on a shell, fatal if error, stdout returns as str
  die(str)      - fail build with a message, errorlevel 3
  env(str)      - return environment variable or None
  exe(str)      - return filename with exe extension based on TARGET_OS
  exp(str)      - expand a $string, searching CLI --vars and then global scope
  ext(str)      - return file extension         (file.c = .c)
  raw(str)      - return file without extension (file.c = file)
  log(str)      - print to stdout
  mkd(str)      - make all subdirs
  obj(str)      - return filename with obj file ext (file.c = file.obj)
  pth(str)      - swap path slashes -- \ on windows, / otherwise
  var(str)      - get buildvar passed in as a string, ie: DEBUG="0"
  yes(str)      - get buildvar passed in as a boolean, ie: DEBUG=False

variables:
  HOST_OS       - compiling machine OS    (str)
  TARGET_OS     - target machine OS       (str)

after use(), variables, where applicable:
  CC            - c compiler
  CXX           - c++ compiler
  LD            - linker
  OBJ           - obj extension (ex: 'obj')
  CCTYPE        - compiler 
  CFLAGS        - list of c flags
  CXXFLAGS      - list of c++ flags
  LDFLAGS       - list of linker flags
  
\"""

JFDI_VERSION = 1

# called at the start of the build
def start_build():
    pass

# return a list of files
def list_input_files():
    return []


# return command to build single file in_path or None to skip
def build_this(in_path):
    return None

# called after every input file has been built
def end_build(in_files):
    pass

# called when the user runs 'jfdi clean'
def clean(in_files):
    pass

# called when the user runs 'jfdi run'
def run():
    pass
    
#
# main -- installs build system if build script is run directly
#
# generated code: do not edit this
#
if __name__ == '__main__':
    import sys
    import os.path
    import urllib.request
    
    print("You have run the build script directly.")
    print("Expected Usage: python jfdi.py -f %s" %
          sys.argv[0])

    DST_FILENAME = 'jfdi.py'
    if os.path.exists(DST_FILENAME):
        sys.exit(0)
    print("Do you want to download the JFDI build script?")
    yesno = input('Y/n -->')
    if yesno == 'n':
        sys.exit(0)

    print("downloading jfdi.py")
    url = "https://raw.githubusercontent.com/mlabbe/jfdi/master/jfdi.py"
    urllib.request.urlretrieve(url, DST_FILENAME)
    
    print("%s downloaded." % DST_FILENAME)
    print("Usage: python %s -f %s" %
          (DST_FILENAME, sys.argv[0]))
    print("To permanently install jfdi, manually copy jfdi.py into your search path.")
    sys.exit(0)

""")
    f.close()
        
#
# api 
#
def _api_ext(x):
    return os.path.splitext(x)[1]

def _api_raw(x):
    return os.path.splitext(x)[0]

def _api_log(msg):
    frame = sys._getframe(1)
    func_name = frame.f_code.co_name
    
    print("%s %s(): %s" % (_log_stamp(), func_name, msg))

def _api_mkd(dirs):
    dirs = _swap_slashes(dirs)
    _message(1, "making dirs %s" % dirs)
    os.makedirs(dirs, exist_ok=True)

def _api_cmd(cmd):
    if cmd.__class__ == list:
        cmd_str = ' '.join(cmd)
    else:
        cmd_str = cmd

    _message(0, cmd_str)

    proc = subprocess.Popen(cmd_str, shell=True,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    out, err = proc.communicate()
    ret = proc.returncode

    if len(err) != 0:
        _warning(err.decode('utf-8'))

    if ret != 0:
        print(out.rstrip().decode('utf-8'), file=sys.stdout)
        print(err.rstrip().decode('utf-8'), file=sys.stderr)
        _fatal_error("\nerror code %d running \"%s\"\n" % (ret, cmd_str),
                     error_code=ret)

    return out.rstrip().decode('utf-8')

def _api_cp(src, dst):
    if os.path.isdir(src):
        _message(0, "recursively copy %s to %s" % (src, dst))
        shutil.copytree(src, dst)
    else:
        _message(0, "cp %s to %s" % (src, dst))
        shutil.copy2(src, dst)

def _api_die(msg):
    sys.stderr.write("die: " + msg + "\n")
    sys.exit(3)

def _api_rm(files):

    file_list = _str_to_list(files)

    for f in file_list:
        if not os.path.exists(f):
            _message(1, "rm nonexistent %s" % f)
            continue

        f = _swap_slashes(f)
    
        if os.path.isdir(f):
            _message(0, "rmdir %s" % f)
            shutil.rmtree(f, ignore_errors=False)
        else:
            _message(0, "rm %s" % f)
            os.remove(f)

def _api_env(e):
    if e not in os.environ:
        return None
    return os.environ[e]

def _api_use(id):
    v = {}
    if id[:4] == 'msvc':
        v['CC'] = 'cl.exe'
        v['CXX'] = v['CC']
        v['OBJ'] = 'obj'
        v['LD'] = 'link.exe'
        v['CCTYPE'] = 'msvc'
        v['CFLAGS'] = []
        v['CXXFLAGS'] = []
        v['LDFLAGS'] = []
        
    elif id == 'clang':
        v['CC'] = 'clang'
        v['CXX'] = 'clang++'
        v['OBJ'] = 'o'
        v['LD'] = 'clang'   # /usr/bin/ld is too low-level
        v['CCTYPE'] = 'gcc' # clang is gcc-like
        v['CFLAGS'] = []
        v['CXXFLAGS'] = []
        v['LDFLAGS'] = []

    elif id == 'gcc':
        v['CC'] = 'gcc'
        v['CXX'] = 'g++'
        v['OBJ'] = 'o'
        v['LD'] = 'gcc'     # /usr/bin/ld is too low-level
        v['CCTYPE'] = 'gcc'
        v['CFLAGS'] = []
        v['CXXFLAGS'] = []
        v['LDFLAGS'] = []
        

    else:
        msg =  "use() unknown ID '%s'\n" % (id)
        msg += "acceptable Ids:\n"
        msg += "\tmsvc, clang, gcc\n"
        _fatal_error(msg)

    # override from environment
    for potential_env in v:
        if potential_env in os.environ:
            v[potential_env] = os.environ[potential_env]

    if _which(v['CC']) == None:
        _warning("use(): compiler '%s' not found in search path.\n" % v['CC'])

    if _which(v['LD']) == None:
        _warning("use(): linker '%s' not found in search path.\n" % v['LD'])

    g = globals()
    for var in v:
        g[var] = v[var]

def _api_arg(flag):
    if 'CCTYPE' not in globals():
        _fatal_error("must call use() before arg()")

    i = 0
    if flag[0] == '-' or flag[0] == '/':
        i = 1

    if globals()['CCTYPE'] == 'msvc':
        symbol = '/'
    else:
        symbol = '-'
        
    return symbol + flag[i:]


def _api_obj(path, in_prefix_path=''):
    prefix_path = _swap_slashes(in_prefix_path)
    if 'CCTYPE' not in globals():
        _fatal_error('you must call use() before calling obj()\n')

    in_paths = _str_to_list(path)
    out_paths = []
    
    ext = ''
    if globals()['CCTYPE'] == 'msvc':
        ext = '.obj'
    elif globals()['CCTYPE'] == 'gcc':
        ext = '.o'

    for p in in_paths:
        split = os.path.splitext(p)

        filename = split[0] + ext
        path = os.path.join(prefix_path, filename)
        
        out_paths.append(path)

    return _list_single_to_str(out_paths)
    

def _api_var(key):
    global _cfg

    ukey = key.upper()
    if ukey not in _cfg['build_vars']:
        return ''
        
    return _cfg['build_vars'][ukey]


def _api_yes(key):
    global _cfg
    
    ukey = key.upper()
    if ukey not in _cfg['build_vars']:
        return False

    if _cfg['build_vars'][ukey] == '0':
        return False
    
    # it is not possible to have a build var with a len(0) value so it
    # is safe to return True now.
    return True
    

def _api_exe(path, append_if_debug=None):
    split = os.path.splitext(path)

    exe = ''
    if globals()['TARGET_OS'] == 'Windows':
        exe = '.exe'

    base_str = str(split[0])
    if append_if_debug != None and _api_yes('DEBUG'):
        base_str += append_if_debug

    return base_str + exe

def _api_exp(in_str):
    _message(1, "expanding \"%s\"" % in_str)
    out = ''

    reading_var = False
    for i in range(0, len(in_str)):
        c = in_str[i]
        if c == ' ':
            reading_var = False
            
        if reading_var:
            continue
            
        if c == '$':
            reading_var = True
            var = in_str[i:].split(' ')[0]
            if len(var) == 1:
                out += c
                reading_var = False
                continue

            var = var[1:]
            if len(var) > 1:
                val = None
                
                # scan calling function first
                frame = sys._getframe(1)
                if var in frame.f_locals:
                    val = frame.f_locals[var]
                    
                # scan vars second (command line override)                
                elif var in _cfg['build_vars']:
                    
                    val = _cfg['build_vars'][var]
                # check environment variables, third
                elif var in os.environ:
                    val = os.environ[var]
                    
                # fall back to global vars
                elif var in globals():
                    val = globals()[var]
                else:
                    _fatal_error("exp(): var %s not found.\n" % var)
                    
            if val.__class__ == list:
                val = ' '.join(str(x) for x in val)

            out += val
        else:
            out += c
                
    return out

def _api_pth(path):
    return _swap_slashes(path)

        
#
# main
#

if __name__ == '__main__':

    dispatch = _ArgDispatch()
    subcommand, args, build_vars = dispatch.dispatch()


    #
    # subcommand init
    #
    if subcommand == 'init':
        generate_tmpl(args.file)
        _message(1, 'wrote %s' % args.file)
        _report_success(g_start_time)
        sys.exit(0)

    #
    # subcommand help
    #
    if subcommand == 'help':
        _display_help_topic(args.topic)
        sys.exit(0)

    #
    # subcommand build, version case 
    # 
    if subcommand == 'build' and args.version:
        print(_pp_version())
        sys.exit(0)
        
    # all subcommands not handled yet require execution of the build script.
    pycode = _get_script(args.file)
    context = _run_script(pycode, args.target_os)

    if subcommand == 'clean':
        #
        # subcommand clean
        #
        _clean(context, args.target_os)

    elif subcommand == 'run':
        #
        # subcommand run
        #
        _canonical_run(context, args.target_os)

    else:
        #
        # subcommand build (default)
        #
        _build(context, args.target_os)
        if args.run:
            _canonical_run(context, args.target_os)

    _report_success(g_start_time)
    sys.exit(0)

