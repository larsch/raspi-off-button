raspi-off-button: raspi-off-button.c
	gcc -s -Wall -o raspi-off-button -Wall raspi-off-button.c -lbcm2835

install: raspi-off-button
	cp raspi-off-button /usr/local/sbin/raspi-off-button
