#!/bin/bash
gcc ./inf136720_c.c -o ./client
gcc ./inf136720_s.c -o ./server
gnome-terminal -e "./server"
gnome-terminal -e "./client"
gnome-terminal -e "./client"
gnome-terminal -e "./client"