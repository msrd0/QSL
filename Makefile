# this makefile uploads the built packages

first: push

#push: mount clean copy sign gen signrel
push: mount clean copy sign

mount:
	mkdir mount
	sshfs alarmpi-debian@msrd0.duckdns.org:/srv/debian mount

clean:
	rm mount/pool/main/all/s/spis* || true
	rm mount/pool/main/all/libs/libspis* || true
	rm mount/pool/main/amd64/s/spis* || true
	rm mount/pool/main/amd64/libs/libspis* || true

copy:
	cp spis*all*.deb mount/pool/main/all/s/
	cp libspis*all*.deb mount/pool/main/all/libs/
	cp spis*amd64*.deb mount/pool/main/amd64/s/
	cp libspis*amd64*.deb mount/pool/main/amd64/libs/

sign:
	ssh alarmpi-debian@msrd0.duckdns.org -o BatchMode=yes -v \
		/srv/debian/bin/sign.sh

gen:
	mount/bin/genpackages.sh mount
	mount/bin/genrelease.sh mount

signrel:
	ssh alarmpi-debian@msrd0.duckdns.org -o BatchMode=yes -v \
		/srv/debian/bin/signreleases.sh
