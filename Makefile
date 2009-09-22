install:
	sudo kextunload -v /tmp/InsomniaT.kext || true
	sudo cp -R build/Release/InsomniaT.kext /tmp
	sudo kextload -v /tmp/InsomniaT.kext

dist:
	cd build && zip -9r InsomniaT.zip Release

