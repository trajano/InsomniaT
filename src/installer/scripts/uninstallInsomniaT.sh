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

  if [ -e /Library/PreferencePanes/InsomniaT.prefPane ]
  then
    killall "System Preferences"
    rm -rf /Library/PreferencePanes/InsomniaT.prefPane
  fi

  if [ -e /usr/local/bin/insomniat ]
  then
    rm -rf /usr/local/bin/insomniat
  fi

  if [ -e /usr/local/bin/uninstallInsomniaT.sh ]
  then
    rm -rf /usr/local/bin/uninstallInsomniaT.sh
  fi
  
  pkgutil -f --unlink net.trajano.pkg.InsomniaT.kernel
  pkgutil -f --unlink net.trajano.pkg.InsomniaT.prefpane
  pkgutil -f --unlink net.trajano.pkg.InsomniaT.uninstall
  pkgutil -f --unlink net.trajano.pkg.InsomniaT.cli
  pkgutil -f --unlink trajano.net.insomniat.InsomniaT-1.pkg
  pkgutil -f --unlink trajano.net.insomniat.InsomniaT.pkg
  pkgutil -f --unlink trajano.net.insomniat.postflight.pkg
  pkgutil -f --unlink trajano.net.insomniat.preflight.pkg

  pkgutil -f --forget net.trajano.pkg.InsomniaT.kernel
  pkgutil -f --forget net.trajano.pkg.InsomniaT.prefpane
  pkgutil -f --forget net.trajano.pkg.InsomniaT.uninstall
  pkgutil -f --forget net.trajano.pkg.InsomniaT.cli
  pkgutil -f --forget trajano.net.insomniat.InsomniaT-1.pkg
  pkgutil -f --forget trajano.net.insomniat.InsomniaT.pkg
  pkgutil -f --forget trajano.net.insomniat.postflight.pkg
  pkgutil -f --forget trajano.net.insomniat.preflight.pkg
  
  exit 0

else
  echo "You need to run this program as root, use sudo $0"
fi