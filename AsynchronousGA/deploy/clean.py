#!/usr/bin/python

import os
import sys
import shutil

contents = os.listdir('.')
skip_list = ['CVS', 'clean.py', 'deploy.bat', 'wsl_deploy.sh', 'move_to_folder.py']
for item in contents:
    if item in skip_list:
        continue
    
    if os.path.isfile(item):
        print('deleting file "%s"' % (item))
        os.remove(item)
        continue
    
    if os.path.isdir(item):
        print('deleting folder "%s"' % (item))
        shutil.rmtree(item)
        continue

