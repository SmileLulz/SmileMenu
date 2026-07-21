#!/usr/bin/env bash

case "$1" in

list)
    echo "Firefox"
    echo "Steam"
    echo "Vesktop"
;;

run)
    case "$2" in

    Firefox)
        firefox
    ;;
    
    Steam)
        steam
    ;;
    
    Vesktop)
        vesktop
    ;;
    
    esac
;;

esac

# smilemenu --provider path/to/this/example-1.sh