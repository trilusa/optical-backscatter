TEST_TYPE := dist

include /var/lib/cloud9/common/Makefile
adc: all
	gcc userspace_adc_read.c -o adc.out
	sudo ./adc.out -t $(TEST_TYPE)
	
demo: all
	gcc demo.c -o demo.out
	sudo ./demo.out