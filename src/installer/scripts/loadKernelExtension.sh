#!/bin/sh

# loadKernelExtension.sh
# InsomniaT
#
# Created by Archimedes Trajano on 11-07-12.
# Copyright 2011 trajano.net. All rights reserved.

  if [ -e /System/Library/Extensions/InsomniaT.kext ]
  then
    kextload /System/Library/Extensions/InsomniaT.kext
  fi

  if [ -e "/System/Library/Extensions/InsomniaT (10.5).kext" ]
  then
    kextload "/System/Library/Extensions/InsomniaT (10.5).kext"
  fi
