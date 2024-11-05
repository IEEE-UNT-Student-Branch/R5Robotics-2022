#!/bin/bash

#trap fucnction SIGINT	# Possibly need?

## Command needs to be run before startup:
# docker rm -f tftello ; xhost + ; docker run -it --name tftello --runtime nvidia --network host --privileged -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v /tmp/argus_socket:/tmp/argus_socket kevinfham/jetsonr5:tensorflowtello

## Bootup container
# Note: eventually the container might need to run in detached mode, running in the background (-d flag and requires getting rid of the -it flag)
docker start tftello
docker exec tftello /bin/bash -c "python3 /home/tests-integration/telloGPIO.py"

