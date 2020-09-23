#!/bin/bash

NAME="vismatrix"
VERSION="latest"

echo
echo
echo "... docker .................................................."
echo "............................................................."
echo

docker build --no-cache -t gosl/$NAME:$VERSION .
docker images -q -f "dangling=true" | xargs docker rmi
