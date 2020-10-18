#!/usr/bin/env bash
IMAGE_NAME=machines-cpp-dev

DOCKER_CMD="docker build -t ${IMAGE_NAME} -f Dockerfile ."

set -e
echo "Building docker image for old games development"
$DOCKER_CMD
