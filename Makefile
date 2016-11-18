# this makefile uploads the built packages

first: push

#push: mount clean copy sign gen signrel
push: mount clean copy sign

mount:
	mkdir mount
	sshfs alarmpi-debian@msrd0.duckdns.org:/srv/debian mount

clean:
	rm mount/pool/main/all/q/qsl* || true
	rm mount/pool/main/all/libq/libqsl* || true
	rm mount/pool/main/amd64/q/qsl* || true
	rm mount/pool/main/amd64/libq/libqsl* || true

copy:
	cp qsl*all*.deb mount/pool/main/all/q/
	cp libqsl*all*.deb mount/pool/main/all/libq/
	cp qsl*amd64*.deb mount/pool/main/amd64/q/
	cp libqsl*amd64*.deb mount/pool/main/amd64/libq/

sign:
	ssh alarmpi-debian@msrd0.duckdns.org -o BatchMode=yes -v \
		/srv/debian/bin/sign.sh

gen:
	mount/bin/genpackages.sh mount
	mount/bin/genrelease.sh mount

signrel:
	ssh alarmpi-debian@msrd0.duckdns.org -o BatchMode=yes -v \
		/srv/debian/bin/signreleases.sh
