#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
mkdir -p ios/Generated
printf '#define VF3_VERSION "ios-port"\n#define VF3_DATE ""\n' > ios/Generated/version.h
