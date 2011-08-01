installer:
	/usr/local/bin/freeze src/installer/InsomniaT.packproj
	osacompile -o "build/packages/Uninstall InsomniaT.app" "src/installer/scripts/Uninstall InsomniaT.applescript"
	rm -f build/insomniat-${CURRENT_PROJECT_VERSION}.dmg
	hdiutil create build/insomniat-${CURRENT_PROJECT_VERSION}.dmg -volname "InsomniaT" -fs HFS+ -srcfolder build/packages

install:
	sudo kextunload -v /tmp/InsomniaT.kext || sudo kextunload -v /System/Library/Extensions/InsomniaT.kext || true
	sudo chown -R root:wheel build/Release/InsomniaT.kext
	sudo cp -R build/Release/InsomniaT.kext /tmp
	sudo kextload -v /tmp/InsomniaT.kext
	sudo chown -R trajano build/Release/InsomniaT.kext

test:
	sudo kextunload -v /tmp/InsomniaT.kext || true
	sudo cp -R build/Release/InsomniaT.kext /tmp
	sudo kextutil -v -t /tmp/InsomniaT.kext
	build/Release/insomniat -n
	sleep 60
	sudo kextunload -v /tmp/InsomniaT.kext || true

test2:
	sudo kextunload -v /tmp/InsomniaT.kext || true
	sudo cp -R build/Release/InsomniaT.kext /tmp
	sudo kextutil -v -t /tmp/InsomniaT.kext

uninstall:
	sudo kextunload -v /tmp/InsomniaT.kext || sudo kextunload -v /System/Library/Extensions/InsomniaT.kext || true

dist:
	cd build && zip -9r InsomniaT.zip Release

clean:
	true

