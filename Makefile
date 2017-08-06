clean :
	make -C avr-test clean
	make -C avr clean
	find . -name '*~' | xargs rm -f

