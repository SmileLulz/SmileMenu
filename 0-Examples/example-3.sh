#!/bin/bash

case $1 in
    list) echo -e "Fooyin\tOpen Fooyin\tfooyin\nSmileMPlayer\tLaunch SmileMPlayer\tsmilemplayer";;

    run) command=$(echo "$2" | cut -f3); exec "$command";;

    *) exit 2;;
esac

# Run:
# smilemenu --provider /path/to/this_script.sh --field name:1 --field description:2
