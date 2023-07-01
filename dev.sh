#!/bin/bash
docker build . -t qfield-dev
docker run -it -v ./build:/opt/builder/build qfield-dev