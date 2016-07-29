#!/bin/sh

# Simple script for automatic Preparation of source.
BASEDIR=$(dirname $0)

cd $BASEDIR

echo "Running aclocal"
aclocal --install -I m4 || exit 1

echo "Running autoheader"
autoheader || exit 1

echo "Running autoconf"
autoconf || exit 1

echo "Running automake"
automake --add-missing || exit 1

cd -

echo "Running configure $@"
./$BASEDIR/configure $@
