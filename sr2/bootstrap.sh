#!/bin/bash
aclocal && automake --add-missing && libtoolize --force  && autoheader && autoconf
