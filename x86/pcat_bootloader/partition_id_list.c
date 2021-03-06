#include <sys/types.h>
#include <stdio.h>

const struct
{
  int id;
  const char *name;
  int inuse;
} mbr_id[] =
  {
    { 0, "Empty", 1 },
    { 1, "DOS 12-bit FAT", 2 },
    { 2, "XENIX root", 1 },
    { 3, "XENIX /usr", 1 },
    { 4, "DOS 3.0+ 16-bit FAT <32M", 2 },
    { 5, "DOS 3.3+ Extended Partition", 1 },
    { 6, "DOS 3.31+ 16-bit FAT >=32M", 2 },
    { 7, "HPFS/NTFS", 2 },
    { 8, "AIX boot partition", 1 },
    { 9, "AIX data partition", 1 },
    { 10, "OS/2 Boot Manager", 1 },
    { 11, "WIN95 OSR2 FAT32", 2 },
    { 12, "WIN95 OSR2 FAT32, LBA-mapped", 2 },
    { 14, "WIN95: DOS 16-bit FAT, LBA-mapped", 2 },
    { 15, "WIN95: Extended partition, LBA-mapped", 1 },
    { 16, "OPUS", 1 },
    { 17, "Hidden DOS 12-bit FAT", 1 },
    { 18, "Configuration/diagnostics partition", 1 },
    { 20, "Hidden DOS 16-bit FAT <32M", 1 },
    { 22, "Hidden DOS 16-bit FAT >=32M", 1 },
    { 23, "Hidden HPFS/NTFS", 1 },
    { 24, "AST SmartSleep Partition", 1 },
    { 27, "Hidden WIN95 OSR2 FAT32", 1 },
    { 28, "Hidden WIN95 OSR2 FAT32, LBA-mapped", 1 },
    { 30, "Hidden WIN95 16-bit FAT, LBA-mapped", 1 },
    { 36, "NEC DOS 3.x", 1 },
    { 39, "Windows RE hidden partition", 1 },
    { 42, "AtheOS File System (AFS)", 1 },
    { 43, "SyllableSecure (SylStor)", 1 },
    { 50, "NOS", 1 },
    { 53, "JFS on OS/2 or eCS", 1 },
    { 56, "THEOS ver 3.2 2gb partition", 1 },
    { 57, "THEOS ver 4 spanned / Plan 9 partition", 1 },
    { 58, "THEOS ver 4 4gb partition", 1 },
    { 59, "THEOS ver 4 extended partition", 1 },
    { 60, "PartitionMagic recovery partition", 1 },
    { 61, "Hidden NetWare", 1 },
    { 64, "Venix 80286", 1 },
    { 65, "PPC PReP (Power PC Reference Platform) Boot", 1 },
    { 66, "SFS (Secure Filesystem)", 1 },
    { 67, "Linux native (sharing disk with DRDOS)", 1 },
    { 68, "GoBack partition", 1 },
    { 69, "EUMEL/Elan", 1 },
    { 70, "EUMEL/Elan", 1 },
    { 71, "EUMEL/Elan", 1 },
    { 72, "EUMEL/Elan", 1 },
    { 74, "Mark Aitchison's ALFS/THIN lightweight filesystem for DOS", 1 },
    { 76, "Oberon partition", 1 },
    { 77, "QNX4.x", 1 },
    { 78, "QNX4.x 2nd part", 1 },
    { 79, "QNX4.x 3rd part", 1 },
    { 80, "OnTrack Disk Manager (older versions) RO", 1 },
    { 81, "OnTrack Disk Manager RW (DM6 Aux1)", 1 },
    { 82, "CP/M", 1 },
    { 83, "Disk Manager 6.0 Aux3", 1 },
    { 84, "Disk Manager 6.0 Dynamic Drive Overlay (DDO)", 1 },
    { 85, "EZ-Drive", 1 },
    { 86, "Golden Bow VFeature Partitioned Volume.", 1 },
    { 87, "DrivePro", 1 },
    { 92, "Priam EDisk", 1 },
    { 97, "SpeedStor", 1 },
    { 99, "Unix System V (SCO, ISC Unix, UnixWare, ...), Mach, GNU Hurd", 1 },
    { 100, "Novell Netware 286, 2.xx", 1 },
    { 101, "Novell Netware 386, 3.xx or 4.xx", 1 },
    { 102, "Novell Netware SMS Partition", 1 },
    { 103, "Novell", 1 },
    { 104, "Novell", 1 },
    { 105, "Novell Netware 5+, Novell Netware NSS Partition", 1 },
    { 112, "DiskSecure Multi-Boot", 1 },
    { 114, "V7/x86", 1 },
    { 116, "Scramdisk partition", 1 },
    { 117, "IBM PC/IX", 1 },
    { 119, "M2FS/M2CS partition, VNDI Partition", 1 },
    { 120, "XOSL FS", 1 },
    { 128, "MINIX until 1.4a", 1 },
    { 129, "MINIX since 1.4b, early Linux", 1 },
    { 130, "Linux swap", 2 },
    { 131, "Linux native partition", 2 },
    { 132, "OS/2 hidden C: drive", 1 },
    { 133, "Linux extended partition", 2 },
    { 134, "FAT16 volume set", 1 },
    { 135, "NTFS volume set", 1 },
    { 136, "Linux plaintext partition table", 1 },
    { 138, "Linux Kernel Partition (used by AiR-BOOT)", 1 },
    { 139, "Legacy Fault Tolerant FAT32 volume", 1 },
    { 140, "Legacy Fault Tolerant FAT32 volume using BIOS extd INT 13h", 1 },
    { 141, "Free FDISK hidden Primary DOS FAT12 partitition", 1 },
    { 142, "Linux Logical Volume Manager partition", 1 },
    { 144, "Free FDISK hidden Primary DOS FAT16 partitition", 1 },
    { 145, "Free FDISK hidden DOS extended partitition", 1 },
    { 146, "Free FDISK hidden Primary DOS large FAT16 partitition", 1 },
    { 147, "Amoeba", 1 },
    { 148, "Amoeba bad block table", 1 },
    { 149, "MIT EXOPC native partitions", 1 },
    { 151, "Free FDISK hidden Primary DOS FAT32 partitition", 1 },
    { 152, "Datalight ROM-DOS Super-Boot Partition", 1 },
    { 153, "DCE376 logical drive", 1 },
    { 154, "Free FDISK hidden Primary DOS FAT16 partitition (LBA)", 1 },
    { 155, "Free FDISK hidden DOS extended partitition (LBA)", 1 },
    { 159, "BSD/OS", 1 },
    { 160, "Laptop hibernation partition", 2 },
    { 161, "HP Volume Expansion (SpeedStor variant)", 1 },
    { 163, "HP Volume Expansion (SpeedStor variant)", 1 },
    { 164, "HP Volume Expansion (SpeedStor variant)", 1 },
    { 165, "BSD/386, 386BSD, NetBSD, FreeBSD", 2 },
    { 166, "OpenBSD", 2 },
    { 167, "NeXTStep", 1 },
    { 168, "Mac OS-X", 1 },
    { 169, "NetBSD", 2 },
    { 170, "Olivetti Fat 12 1.44MB Service Partition", 1 },
    { 171, "Mac OS-X Boot partition", 1 },
    { 174, "ShagOS filesystem", 1 },
    { 175, "MacOS X HFS", 1 },
    { 176, "BootStar Dummy", 1 },
    { 177, "HP Volume Expansion (SpeedStor variant)", 1 },
    { 179, "HP Volume Expansion (SpeedStor variant)", 1 },
    { 180, "HP Volume Expansion (SpeedStor variant)", 1 },
    { 182, "HP Volume Expansion (SpeedStor variant)", 1 },
    { 183, "BSDI BSD/386 filesystem", 1 },
    { 184, "BSDI BSD/386 swap partition", 1 },
    { 187, "Boot Wizard hidden", 1 },
    { 188, "Acronis backup partition", 1 },
    { 190, "Solaris 8 boot partition", 1 },
    { 191, "New Solaris x86 partition", 1 },
    { 192, "DR-DOS/Novell DOS secured partition", 1 },
    { 193, "DRDOS/secured (FAT-12)", 1 },
    { 194, "Hidden Linux", 1 },
    { 195, "Hidden Linux swap", 1 },
    { 196, "DRDOS/secured (FAT-16, < 32M)", 1 },
    { 197, "DRDOS/secured (extended)", 1 },
    { 198, "DRDOS/secured (FAT-16, >= 32M)", 1 },
    { 199, "Syrinx boot", 1 },
    { 200, "Reserved for DR-DOS 8.0+", 1 },
    { 201, "Reserved for DR-DOS 8.0+", 1 },
    { 202, "Reserved for DR-DOS 8.0+", 1 },
    { 203, "DR-DOS 7.04+ secured FAT32 (CHS)/", 1 },
    { 204, "DR-DOS 7.04+ secured FAT32 (LBA)/", 1 },
    { 205, "CTOS Memdump", 1 },
    { 206, "DR-DOS 7.04+ FAT16X (LBA)/", 1 },
    { 207, "DR-DOS 7.04+ secured EXT DOS (LBA)/", 1 },
    { 208, "Multiuser DOS secured partition", 1 },
    { 209, "Old Multiuser DOS secured FAT12", 1 },
    { 212, "Old Multiuser DOS secured FAT16 <32M", 1 },
    { 213, "Old Multiuser DOS secured extended partition", 1 },
    { 214, "Old Multiuser DOS secured FAT16 >=32M", 1 },
    { 216, "CP/M-86", 1 },
    { 218, "Non-FS Data", 1 },
    { 219, "Digital Research CP/M, Concurrent CP/M, Concurrent DOS", 1 },
    { 221, "Hidden CTOS Memdump", 1 },
    { 222, "Dell PowerEdge Server utilities (FAT fs)", 1 },
    { 223, "BootIt EMBRM", 1 },
    { 224, "Reserved by STMicroelectronics for a filesystem called ST AVFS.", 1 },
    { 225, "DOS access or SpeedStor 12-bit FAT extended partition", 1 },
    { 227, "DOS R/O or SpeedStor", 1 },
    { 228, "SpeedStor 16-bit FAT extended partition < 1024 cyl.", 1 },
    { 229, "Tandy MSDOS with logically sectored FAT", 1 },
    { 230, "Storage Dimensions SpeedStor", 1 },
    { 232, "LUKS", 1 },
    { 235, "BeOS BFS", 1 },
    { 236, "SkyOS SkyFS", 1 },
    { 238, "MBR is followed by an EFI header", 1 },
    { 239, "EFI file system", 1 },
    { 240, "Linux/PA-RISC boot loader", 1 },
    { 241, "Storage Dimensions SpeedStor", 1 },
    { 242, "DOS 3.3+ secondary partition", 1 },
    { 244, "SpeedStor large partition", 1 },
    { 245, "Prologue multi-volume partition", 1 },
    { 246, "Storage Dimensions SpeedStor", 1 },
    { 249, "pCache", 1 },
    { 250, "Bochs", 1 },
    { 251, "VMware File System partition", 1 },
    { 252, "VMware Swap partition", 1 },
    { 253, "Linux raid partition with autodetect using persistent superblock", 1 },
    { 254, "LANstep", 1 },
    { 255, "Xenix Bad Block Table", 1 },
  };

main ()
{
  int i;
  for (i = 0; i < sizeof mbr_id / sizeof (mbr_id[0]);i++)
    if (mbr_id[i].inuse > 1)
      printf ("{ %d, \"%s\" },\n", mbr_id[i].id, mbr_id[i].name);

}
