#!/bin/bash
aclocal && autoheader && libtoolize --force && automake --add-missing && autoconf
