#!/usr/bin/env bash

pushd "$(pwd)" > /dev/null   # push current dir onto the stack

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}/.." 

vendor/premake/bin/premake5 gmake
