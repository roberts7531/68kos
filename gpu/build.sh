#/bin/bash

m68k-elf-gcc -Wall -m68000 -c *.c

m68k-elf-ld   -T link.ld  *.o  *.a
m68k-elf-objcopy -I elf32-m68k -O symbolsrec a.out demo.run
srec_cat demo.run  -o long.srec -address-length=3
