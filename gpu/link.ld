MEMORY
{
	RAM(RWX) 	: ORIGIN =	0x000000, LENGTH = 900K
	ROM(RX)		: ORIGIN = 	0x0e0000, LENGTH = 64K
} 
 
 SECTIONS
 {
	

 	bss_size = SIZEOF(.bss);

	. = 0x10000;
	.text ALIGN(0x4) : {
 		*(.text.startup);
 		*(.text);
 	}>RAM
	.eh_frame :{
		*(.eh_frame);
	}>RAM
	.rodata ALIGN(0x4):
	{
		*(.rodata*)
	}>RAM
	rom_data = .;
 	
 	
 	.data ALIGN(0x4): {
		data_start = .;
		*(.data)
		data_end = .;
	}>RAM
	data_size = SIZEOF(.data);

	.bss ALIGN(0x4): {
		bss_start = .;
		*(.bss)
		_bss_end = .;	
		*(COMMON)
    	_end =  ALIGN (0x8);
    	__end = _end;	
	}>RAM

 }
 ENTRY(_start)
