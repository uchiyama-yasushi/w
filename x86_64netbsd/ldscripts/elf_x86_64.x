OUTPUT_FORMAT ("elf64-x86-64")
OUTPUT_ARCH (i386:x86-64)

SECTIONS
{
  .note.netbsd.ident :
  {
	*(.note.netbsd.ident)
	. = ALIGN (8);
  }

  .text :
  {
	*(.text*)
	*(.rodata.*)
	. = ALIGN (8);
  }

  .data :
  {
	data_start = .;
	*(.data.*)
	. = ALIGN (8);
  }
  data_end = .;

  .bss :
  {
	bss_start = .;
	*(.bss.*)
	. = ALIGN (8);
  }
  bss_end = .;

  .heap :
  {
	heap_start = .;
	. =  . + 0x1000000; /* 16MB */
  }
  heap_end = .;


/*
	/DISCARD/ : { *(*plt*) }
	/DISCARD/ : { *(.eh_frame) }
	/DISCARD/ : { *(.comment) }
*/
}
