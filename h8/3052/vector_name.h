static char reserved[] = "reserved";
static struct
{
  int addr;
  const char *name;
} vector[] = {
  { 0x00, "RESET" },
  { 0x04, reserved },
  { 0x08, reserved },
  { 0x0c, reserved },
  { 0x10, reserved },
  { 0x14, reserved },
  { 0x18, reserved },
  { 0x1c, "NMI" },
  { 0x20, "TRAPA#0" },
  { 0x24, "TRAPA#1" },
  { 0x28, "TRAPA#2" },
  { 0x2c, "TRAPA#3" },
  { 0x30, "IRQ0" },
  { 0x34, "IRQ1" },
  { 0x38, "IRQ2" },
  { 0x3c, "IRQ3" },
  { 0x40, "IRQ4" },
  { 0x44, "IRQ5" },
  { 0x48, reserved },
  { 0x4c, reserved },
  { 0x50, "WOVI" },
  { 0x54, "CMI" },
  { 0x58, reserved },
  { 0x5c, reserved },
  { 0x60, "IMIA0" },
  { 0x64, "IMIB0" },
  { 0x68, "OVI0" },
  { 0x6c, reserved },
  { 0x70, "IMIA1" },
  { 0x74, "IMIB1" },
  { 0x78, "OVI1" },
  { 0x7c, reserved },
  { 0x80, "IMIA2" },
  { 0x84, "IMIB2" },
  { 0x88, "OVI2" },
  { 0x8c, reserved },
  { 0x90, "IMIA3" },
  { 0x94, "IMIB3" },
  { 0x98, "OVI3" },
  { 0x9c, reserved },
  { 0xa0, "IMIA4" },
  { 0xa4, "IMIB4" },
  { 0xa8, "OVI4" },
  { 0xac, reserved },
  { 0xb0, "DEND0A" },
  { 0xb4, "DEND0B" },
  { 0xb8, "DEND1A" },
  { 0xbc, "DEND1B" },
  { 0xc0, reserved },
  { 0xc4, reserved },
  { 0xc8, reserved },
  { 0xcc, reserved },
  { 0xd0, "ERI0" },
  { 0xd4, "RXI0" },
  { 0xd8, "TXI0" },
  { 0xdc, "TEI0" },
  { 0xe0, "ERI1" },
  { 0xe4, "RXI1" },
  { 0xe8, "TXI1" },
  { 0xec, "TEI1" },
  { 0xf0, "ADI" },
};
