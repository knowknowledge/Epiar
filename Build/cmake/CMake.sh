#!/bin/bash

if [[ $1 == clean ]]; then
	echo cleaning...
	rm -r "`pwd`/build"
else
	mkdir "`pwd`/build"
	cmake-gui -H"`pwd`" -B"`pwd`/build"
fi
