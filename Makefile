UNAME	:= $(shell uname -r)
KDIR	:= /lib/modules/$(UNAME)/build
PWD	:= $(shell pwd)

obj-m	:= sim_dev.o

all::	test_sim_dev
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	
test_sim_dev: test_sim_dev.c
	gcc -o $@ $< -lrt

clean::
	$(RM) test_sim_dev 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
