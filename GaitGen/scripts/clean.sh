#!/bin/sh
find . -name socket_log.txt -print -exec rm -f {} \;
find . -name genealogy.txt -print -exec rm -f {} \;
find . -name '*~' -print -exec rm -f {} \;
find . -name '*.bak' -print -exec rm -f {} \;
find . -name '*.bck' -print -exec rm -f {} \;
find . -name '*.tmp' -print -exec rm -f {} \;
find . -name '*sh.[oe][0-9][0-9][0-9][0-9]' -print -exec rm -f {} \;
find . -name '*sh.p[oe][0-9][0-9][0-9][0-9]' -print -exec rm -f {} \;
