#!/usr/bin/python

import os
import sys

usage = '%s destination_path\n' % sys.argv[0]
if len(sys.argv) != 2:
    sys.exit(usage)
    
output_file = 'Generated_Sync_File.txt'
parse_subfolder_string = ' selected'
destination_path = sys.argv[1]

list_of_folders = os.listdir('.')
folders_to_copy = []
folders_to_copy_dest = []

for folder in list_of_folders:

    if os.path.isfile(folder):
        continue
    if folder == '.' or folder == '..':
        continue

    if folder.endswith(parse_subfolder_string) == False:
        folders_to_copy.append(folder)
        folders_to_copy_dest.append(folder)
    else:
        list_of_subfolders = os.listdir(folder)
        for subfolder in list_of_subfolders:
            if os.path.isfile(os.path.join(folder, subfolder)):
                continue
            if subfolder == '.' or subfolder == '..':
                continue
            folders_to_copy.append(os.path.join(folder, subfolder))
            folders_to_copy_dest.append(os.path.join(folder[0:-len(parse_subfolder_string)], subfolder))
    
    # print folders_to_copy

fout = open(output_file, 'w')
for i in range(0, len(folders_to_copy)):

    src = os.path.abspath(os.path.join(os.getcwd(), folders_to_copy[i]))
    dest = os.path.abspath(os.path.join(destination_path, folders_to_copy_dest[i]))
    
    fout.write('%s\t%s\n' % (src, dest))
    
fout.close()

