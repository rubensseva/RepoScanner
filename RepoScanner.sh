#!/bin/bash

# saner programming env: these switches turn some bugs into errors
set -o errexit -o pipefail -o noclobber -o nounset

# -allow a command to fail with !’s side effect on errexit
# -use return value from ${PIPESTATUS[0]}, because ! hosed $?
! getopt --test > /dev/null 
if [[ ${PIPESTATUS[0]} -ne 4 ]]; then
    echo 'I’m sorry, `getopt --test` failed in this environment.'
    exit 1
fi

OPTIONS=thp:v
LONGOPTS=text,hidden,path:,verbose

# -regarding ! and PIPESTATUS see above
# -temporarily store output to be able to check for errors
# -activate quoting/enhanced mode (e.g. by writing out “--options”)
# -pass arguments only via   -- "$@"   to separate them correctly
! PARSED=$(getopt --options=$OPTIONS --longoptions=$LONGOPTS --name "$0" -- "$@")
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then
    # e.g. return value is 1
    #  then getopt has complained about wrong arguments to stdout
    exit 2
fi
# read getopt’s output this way to handle the quoting right:
eval set -- "$PARSED"

t="" h="" v="" p=""
# now enjoy the options in order and nicely split until we see --
while true; do
    case "$1" in
        -t|--text)
            t="-t"
            shift
            ;;
        -h|--hidden)
            h="-h"
            shift
            ;;
        -v|--verbose)
            v="-v"
            shift
            ;;
        -p|--path)
            p="$2"
            shift 2
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Programming error"
            exit 3
            ;;
    esac
done


# Get the absolute path of current script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ -n "$v" ]; then
  echo "Command line arguments - text: $t, verbose: $v, hidden: $h, path: $p"
  echo "Executing command: "$DIR/SearchLib/RepoScannerProgram $p $h $v
fi

# Execute C program to search for files
# REPOS=$($DIR/SearchLib/RepoScannerProgram $p $v $h)

# Execute C program to search for files
REPOS=$(/opt/repoScanner/SearchLib/RepoScannerProgram $p $v $h)

if [ -n "$t" ] || [ -n "$v" ]; then
  echo "$REPOS"
else
  # Index is stored in $REPLY variable
  select repo in $(echo "$REPOS"); do
      if [ -z $repo ]; then
        echo "Could not understand choice"$repo
        echo "Exiting"
        break
      fi
      echo "changing directory to"${repo/.git/}
      cd ${repo/.git/}
      break
    done

  exec $SHELL
fi

