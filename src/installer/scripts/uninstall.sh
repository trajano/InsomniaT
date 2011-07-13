#!/bin/sh
__REAL_SCRIPTDIR=$( cd -P -- "$(dirname -- "$(command -v -- "$0")")" && pwd -P )
PREFLIGHT_SCRIPT="$__REAL_SCRIPTDIR/InsomniaT.mpkg/Contents/Resources/preflight"
if [ -x "$PREFLIGHT_SCRIPT" ]
then
  osascript <<EOT
    tell app "System Events"
      display dialog "This will uninstall InsomniaT from the system.  " & \
                     "Press 'OK' to continue with uninstallation or " & \
                     "'Cancel' to cancel uninstallation.  This will ask " & \
                     "you for administrator priviledges to perform " & \
                     "uninstallation." \
        with title "Uninstall InsomniaT" \
        with icon caution
      do shell script "'$PREFLIGHT_SCRIPT'" with administrator privileges
    end tell
EOT
else
  osascript <<EOT
    tell app "System Events"
      display alert "Unable to locate main uninstaller script code." \
        message "This script needs to be executed within the DMG file as " & \
                "it needs resources that are found in the InsomniaT.mpkg " & \
                "file." \
        as critical 
    end tell
EOT
fi

