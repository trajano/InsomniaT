installer:
	sudo chown -R root:wheel build/Release/InsomniaT.kext
	sudo chown -R root:wheel "build/Release/InsomniaT (10.5).kext"
	cd src/installer && /Developer/usr/bin/packageMaker --doc ${PRODUCT_NAME}.pmdoc --out ../../build/${PRODUCT_NAME}-${CURRENT_PRODUCT_VERSION}.pkg
	sudo chown -R trajano "build/Release/InsomniaT (10.5).kext"
	sudo chown -R trajano build/Release/InsomniaT.kext

install:
	sudo kextunload -v /tmp/InsomniaT.kext || true
	sudo kextunload -v /System/Library/Extensions/InsomniaT.kext || true
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

