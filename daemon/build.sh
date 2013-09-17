#!/bin/bash
cd "$(dirname "$0")"
./bootstrap.sh 2 Locals/Build/doBuild.sh "$@"
