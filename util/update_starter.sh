#!/bin/bash

sed -i "s,Icon=.*,Icon=$PWD/textures/planether.png,g" planether.desktop
sed -i "s,Exec=.*,Exec=$PWD/bin/planether,g" planether.desktop
sed -i "s,Path=.*,Path=$PWD,g" planether.desktop
