#/bin/bash
rm a.out
cd fatfs
m68k-elf-gcc -Wall -m68000 -c *.c
m68k-elf-ar rcs fatfs.a *.o
rm -rf *.o
cp fatfs.a ../lib/
cd ..
cd src
m68k-elf-gcc -Wall  -m68000  -c *.c
ls -1 *.o | xargs -L1 -I{} mv {} ../obj/{} 
cd ../asm
m68k-elf-as traps.s -mcpu=68000 -o traps.o
m68k-elf-as startup.s -mcpu=68000 -o startup.o
ls -1 *.o | xargs -L1 -I{} mv {} ../obj/{} 
cd ..
m68k-elf-ld   -T link.ld  obj/*.o  lib/*.a
m68k-elf-objcopy -I elf32-m68k -O symbolsrec a.out demo.run
srec_cat demo.run  -o long.srec -address-length=3
