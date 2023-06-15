#!/usr/bin/python

import sys
import os
import subprocess
import argparse
import re
import shutil

global_submit_template = """
# standard universe requires checkpointing via condor_compile
# universe = standard
# vanilla universe runs normal code (static compilation often a good idea)
universe = vanilla

executable = gaitsym_tcp
arguments = --quiet --hostList hostlist.txt --runTimeLimit RUNTIME_IN_SECONDS
transfer_input_files = hostlist.txt
when_to_transfer_output = ON_EXIT_OR_EVICT
notification = never

# these control the hosts chosen
requirements = (Target.OpSys == "LINUX" && Target.Arch == "X86_64")
request_memory = 256
rank=kflops
request_disk=100000
"""

def submit_condor():
    parser = argparse.ArgumentParser(description='Generate and submit a Condor job')
    parser.add_argument('-q', '--queue', type=int, required=False, default=1, help='Number of jobs to queue [1]')
    parser.add_argument('-r', '--run_time', type=float, required=False, default=1, help='Maximum runtime in hours [1.0]')
    parser.add_argument('-s', '--submit_file', type=str, required=False, default='condor_submit.txt', help='Batch file name [condor_submit.txt]')
    parser.add_argument('-c', '--submit_command', type=str, required=False, default='condor_submit', help='Condor submit command [condor_submit]')
    parser.add_argument('-t', '--template_file', type=str, required=False, default='', help='Command template file [uses internal template]')
    parser.add_argument('-l', '--log_file_name', type=str, required=False, default='log.txt', help='Name of log file [log.txt]')
    parser.add_argument('-lf', '--log_folder_name', type=str, required=False, default='log', help='Name of log folder [log]')
    parser.add_argument('-f', '--force', action='store_true', help='Force overwrite of files')
    parser.add_argument('-d', '--debug', action='store_true', help='Write output logs for all processes')
    parser.add_argument('-n', '--no_requests', action='store_true', help='Disable the requests')
    parser.add_argument('-v', '--verbose', action='store_true', help='Print progress in more detail')
    args = parser.parse_args()

    if args.verbose:
        pretty_print_sys_argv(sys.argv)
        pretty_print_argparse_args(args)

    # check files
    if args.verbose:
        print('Checking %s' % (args.submit_file))
    if os.path.exists(args.submit_file):
        if args.force == False:
            print('Error: %s exists. Use --force to overwrite.' % (args.submit_file))
            sys.exit(1)
        if os.path.isfile(args.submit_file) == False:
            print('%s exists and is not a file so cannot overwrite. Remove manually.' % (args.submit_file))
            sys.exit(1)

    if args.log_file_name.find(' ') != -1 or args.log_folder_name.find(' ') != -1:
        print('Error: log names and folders cannot contain spaces')
        sys.exit(1)

    global global_submit_template
    if args.template_file != '':
        if args.verbose:
            print('Loading %s' % (args.template_file))
        fin = open(args.template_file, 'r')
        global_submit_template = fin.read()
        fin.close()

    if args.force:
        if os.path.exists(args.log_file_name):
            if os.path.isfile(args.log_file_name) == False:
                print('%s exists and is not a file so cannot overwrite. Remove manually.' % (args.log_file_name))
                sys.exit(1)
            os.unlink(args.log_file_name)
        if os.path.exists(args.log_folder_name):
            if os.path.isdir(args.log_folder_name) == False:
                print('%s exists and is not a folder so cannot overwrite. Remove manually.' % (args.log_folder_name))
                sys.exit(1)
            shutil.rmtree(args.log_folder_name)

    # do substitutions
    global_submit_template_use = global_submit_template
    if args.verbose:
        print('Creating %s' % (args.submit_file))
    if args.debug:
        global_submit_template_use = global_submit_template_use + ('\nlog = %s/log_$(Process).txt\noutput = %s/out_$(Process).txt\nerror = %s/err_$(Process).txt\n'
                                          % (args.log_folder_name, args.log_folder_name, args.log_folder_name))
        global_submit_template_use = global_submit_template_use.replace('--quiet', '')
        if os.path.exists(args.log_folder_name) == False:
            os.mkdir(args.log_folder_name)
        if os.path.isdir(args.log_folder_name) == False:
            print('%s exists and is not a folder so cannot overwrite. Remove manually.' % (args.log_folder_name))
            sys.exit(1)
    else:
        global_submit_template_use = global_submit_template_use + ('\nlog = %s\n' % (args.log_file_name))
    # we can use regular expressions to perform multiple replace operations
    rep = {'RUNTIME_IN_SECONDS': str(int(args.run_time * 60 * 60))} # define desired replacements here
    if args.no_requests:
        rep['requirements'] = '#requirements'
        rep['request_memory'] = '#request_memory'
        rep['rank'] = '#rank'
        rep['request_disk'] = '#request_disk'
    # use these three lines to do the replacement
    rep = dict((re.escape(k), v) for k, v in rep.iteritems())
    pattern = re.compile("|".join(rep.keys()))
    submit_string = pattern.sub(lambda m: rep[re.escape(m.group(0))], global_submit_template_use)
    submit_string = submit_string + ('\nqueue %d\n' % (args.queue)) # queue needs to be the last line

    fout = open(args.submit_file, 'w')
    fout.write(submit_string)
    fout.close()

    # submit the job
    command = [args.submit_command, args.submit_file]
    if args.verbose:
        print('Running command:')
        pretty_print_sys_argv(command)
    subprocess.call(command)


def pretty_print_sys_argv(sys_argv):
    quoted_sys_argv = quoted_if_necessary(sys_argv)
    print(' '.join(quoted_sys_argv))

def pretty_print_argparse_args(argparse_args):
    for arg in vars(argparse_args):
        print('%s: %s' % (arg, getattr(argparse_args, arg)))

def quoted_if_necessary(input_list):
    output_list = []
    for item in input_list:
        if re.match('\w', item):
            item = '"' + item + '"'
        output_list.append(item)
    return output_list

# program starts here
if __name__ == '__main__':
    submit_condor()










