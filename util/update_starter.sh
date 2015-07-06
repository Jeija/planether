#!/bin/bash

if [ "$(uname)" = "Darwin" ]; then
	exit 0
fi

sed -i "s,Icon=.*,Icon=$PWD/textures/planether.png,g" planether.desktop
sed -i "s,Exec=.*,Exec=$PWD/bin/planether,g" planether.desktop
sed -i "s,Path=.*,Path=$PWD,g" planether.desktop
