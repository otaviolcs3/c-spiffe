#!/usr/bin/env bash
#arguments: $1 = hostname
ssh root@$1 << "EOL" 
pkill go-server
EOL
