#!/usr/bin/env bash
if [[ ! -d .git ]]
then
  echo "Script should be called from project root directory!"
fi

IMAGE_NAME=machines-cpp-dev
WORKDIR=/opt/dev
MOUNTS="$(pwd):${WORKDIR}"

DOCKER_CMD="docker run -it \
            -v $MOUNTS \
            -w ${WORKDIR} \
            ${IMAGE_NAME} \
            bash"

set -e
echo "Logging into docker shell"
$DOCKER_CMD
