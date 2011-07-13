installer:
	/usr/local/bin/freeze src/iceberg/InsomniaT/InsomniaT.packproj
	cp src/installer/scripts/uninstall.sh build/packages/uninstall
	chmod a+rx build/packages/uninstall
	rm -f build/insomniat-${CURRENT_PROJECT_VERSION}.dmg
	hdiutil create build/insomniat-${CURRENT_PROJECT_VERSION}.dmg -volname "InsomniaT" -fs HFS+ -srcfolder build/packages

install:
	sudo kextunload -v /tmp/InsomniaT.kext || sudo kextunload -v /System/Library/Extensions/InsomniaT.kext || true
	sudo chown -R root:wheel build/Release/InsomniaT.kext
	sudo cp -R build/Release/InsomniaT.kext /tmp
	sudo kextload -v /tmp/InsomniaT.kext

test:
	sudo kextunload -v /tmp/InsomniaT.kext || true
	sudo cp -R build/Release/InsomniaT.kext /tmp
	sudo kextutil -v -t /tmp/InsomniaT.kext

uninstall:
	sudo kextunload -v /tmp/InsomniaT.kext || true

dist:
	cd build && zip -9r InsomniaT.zip Release

clean:
	true

