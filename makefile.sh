#!/bin/bash
gcc ./client.c -o ./client
gcc ./server.c -o ./server
gnome-terminal -e "./server"
gnome-terminal -e "./client"
gnome-terminal -e "./client"
gnome-terminal -e "./client"
