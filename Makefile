install:
	sudo kextunload -v /tmp/InsomniaT.kext || true
	sudo cp -R build/Release/InsomniaT.kext /tmp
	sudo kextload -v /tmp/InsomniaT.kext

