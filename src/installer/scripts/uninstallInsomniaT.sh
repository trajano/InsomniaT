#!/bin/sh

# uninstallInsomniaT.sh
# InsomniaT
#
# Created by Archimedes Trajano on 2010-06-09.
# Copyright 2010 trajano.net. All rights reserved.

ROOT_UID=0
if [ "$UID" -eq "$ROOT_UID" ] 
then
  echo "Uninstalling InsomniaT..."
  if [ -e /System/Library/Extensions/InsomniaT.kext ]
  then
    kextunload /System/Library/Extensions/InsomniaT.kext
    rm -rf /System/Library/Extensions/InsomniaT.kext
  fi

  if [ -e "/System/Library/Extensions/InsomniaT (10.5).kext" ]
  then
    kextunload "/System/Library/Extensions/InsomniaT (10.5).kext"
    rm -rf "/System/Library/Extensions/InsomniaT (10.5).kext"
  fi

  if [ -e /Application/Utilities/InsomniaT.app ]
  then
    rm -rf /Application/Utilities/InsomniaT.app
  fi

  if [ -e /Library/PreferencePanes/InsomniaTPref.prefPane ]
  then
    rm -rf /Library/PreferencePanes/InsomniaTPref.prefPane
  fi

  if [ -e /usr/local/bin/insomniat ]
  then
    rm -rf /usr/local/bin/insomniat
  fi

  if [ -e /usr/local/bin/uninstallInsomniaT.sh ]
  then
    rm -rf /usr/local/bin/uninstallInsomniaT.sh
  fi

else
  echo "You need to run this program as root, use sudo $0"
fi