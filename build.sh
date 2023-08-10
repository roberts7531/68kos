#/bin/bash
rm a.out
cd fatfs
m68k-elf-gcc -Wall -m68000 -c *.c
cp *.o ../
cd ..
m68k-elf-gcc -Wall  -m68000  -c *.c
m68k-elf-as startup.S -mcpu=68000 -o startup.o
m68k-elf-as traps.s -mcpu=68000 -o traps.o
m68k-elf-ld   -T link.ld  *.o  *.a
m68k-elf-objcopy -I elf32-m68k -O symbolsrec a.out demo.run
srec_cat demo.run  -o long.srec -address-length=3
