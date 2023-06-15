#!/usr/bin/python

import os
import sys
import shutil
import argparse

def move_to_folder():
    parser = argparse.ArgumentParser(description='Move the generated deployment files to a named folder')
    parser.add_argument('folder', type=str, nargs=1, help='the folder to copy the files to')
    parser.add_argument('-f', '--force_overwrite', action="store_true", help='overwrite file/folder if it exists')
    args = parser.parse_args()
    
    print args

    output_folder = args.folder[0]
    if os.path.exists(output_folder):
        if not args.force_overwrite:
            print('"%s" exists. Use --force_overwrite to overwrite.' % (output_folder))
            sys.exit(1)
        if os.path.isfile(output_folder):
            print('deleting file "%s"' % (output_folder))
            os.remove(output_folder)
        if os.path.isdir(output_folder):
            print('deleting folder "%s"' % (output_folder))
            shutil.rmtree(output_folder)
    
    contents = os.listdir('.')
    os.mkdir(output_folder)
    skip_list = ['CVS', 'clean.py', 'deploy.bat', 'wsl_deploy.sh', 'move_to_folder.py']
    for item in contents:
        if item in skip_list:
            continue
        
        print('moving item "%s"' % (item))
        shutil.move(item, os.path.join(output_folder, item))

# program starts here

if __name__ == '__main__':
    move_to_folder()
    
