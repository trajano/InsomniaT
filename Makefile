install:
	sudo kextunload -v /tmp/InsomniaT.kext || true
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

