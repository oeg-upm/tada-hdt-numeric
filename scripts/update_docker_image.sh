#!/bin/bash
read -p 'This will update the docker image on docker hub, are you sure? (y/N) ' execflag
if [[ $execflag == y* || $execflag == Y* ]]
then
    echo "Preparing to push the image to docker hub"
    docker image build -t oegupm/tada-hdt-numeric:latest -f Dockerfile.base .
    docker image push oegupm/tada-hdt-numeric:latest
else
    echo The docker image will not be updated in docker hub
fi
#echo Your answer is $execflag
