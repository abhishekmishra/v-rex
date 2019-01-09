#!/bin/bash

docker run --rm -it --privileged \
    --net host \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v /var/run/docker.sock:/var/run/docker.sock \
    --volume="$HOME/.Xauthority:/root/.Xauthority:rw" \
    --env="DISPLAY" \
    --name v-rex-motif \
    abhishekmishra3/v-rex-motif:latest /root/v-rex/v-rex-motif/bin/release/v-rex "$@"
