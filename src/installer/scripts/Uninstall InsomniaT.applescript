try
	tell application "Finder"
		set preflight_file to quoted form of POSIX path of (file "preflight" in container "Resources" in container "Contents" in file "InsomniaT.mpkg" in folder of (path to me) as text)
		try
			display dialog "This will uninstall InsomniaT from the system.  Press 'OK' to continue with uninstallation or 'Cancel' to cancel uninstallation.  This will ask you for administrator priviledges to perform uninstallation." with title "Uninstall InsomniaT" with icon caution
			do shell script preflight_file with administrator privileges
			display alert "InsomniaT has been uninstalled successfully." as informational
		end try
	end tell
on error
	display alert "Unable to locate main uninstaller script code." message "This script needs to be executed within the DMG file as it needs resources that are found in the InsomniaT.mpkg file." as critical
end try