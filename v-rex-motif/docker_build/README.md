### Docker build of V-Rex(motif)

##### To run the program in docker and connect to your local docker do the following...

```
docker run --rm -it --privileged \
    --net host \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v /var/run/docker.sock:/var/run/docker.sock \
    --volume="$HOME/.Xauthority:/root/.Xauthority:rw" \
    --env="DISPLAY" \
    --name v-rex-motif \
    abhishekmishra3/v-rex-motif:latest /root/v-rex/v-rex-motif/bin/release/v-rex
```

##### To connect to a remote docker via http, do the following...

 (Note: slash at the end of url is essential)

```
docker run --rm -it --privileged \
    --net host \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v /var/run/docker.sock:/var/run/docker.sock \
    --volume="$HOME/.Xauthority:/root/.Xauthority:rw" \
    --env="DISPLAY" \
    --name v-rex-motif \
    abhishekmishra3/v-rex-motif:latest /root/v-rex/v-rex-motif/bin/release/v-rex http://<host>:<port>/
```
