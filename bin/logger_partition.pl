#!/usr/pkg/bin/perl
$drive = 'ld0';
$dos = 16 * 1024 * 1024;
$dos_start = 63;

open(IN, "fdisk $drive |");
$dummy = <IN>;
$dummy = <IN>;
$line = <IN>;
close(IN);

if ($line =~ /cylinders: (\d+), heads: (\d+), sectors\/track: (\d+)/) {
    ($cyl, $head, $sec) = ($1, $2, $3);
} else {
    die "fdisk incompatible";
}
$whole = $cyl * $head * $sec;

$dos /= 512;
if ($dos % ($head * $sec)) {
    $dos -= ($dos % ($head * $sec));
    $dos += $head * $sec;
}

# -s 'id/start/size'
print "fdisk -f -u -0 -s '12/$dos_start/$dos' $drive\n"; #  FAT32 LBA-mapped
#print "fdisk -f -u -0 -s '6/$dos_start/$dos' $drive\n"; #  FAT16 >=32M
#print "fdisk -f -u -0 -s '4/$dos_start/$dos' $drive\n"; #  FAT16 < 32M
$netbsd_start = $dos_start + $dos;
$netbsd_size  = $whole - $netbsd_start;
print "fdisk -a -f -u -1 -s '169/$netbsd_start/$netbsd_size' $drive\n";

print "disklabel $drive > foo\n";
print "disklabel -r -R $drive foo\n";

system "disklabel $drive > foo";
open(IN, "<foo");
open(OUT, ">new");
while (<IN>) {
    last if /^\d+ partitions:/;
	print OUT;
}
close(IN);

# skip 2sector. (PBR + disklabel)
$a_start = $netbsd_start + 2;
$a_size = $netbsd_size - 2;

print OUT "5 partitions:\n";
print OUT "  a: $a_size $a_start SysVBFS\n";
print OUT "  c: $netbsd_size $netbsd_start unused 0 0\n";
print OUT "  d: $whole 0 unused 0 0\n";
print OUT "  e: $dos $dos_start MSDOS\n";
close(OUT);
print "disklabel -R $drive new\n";

print 'newfs_msdos -F 32 /dev/r' . $drive . "e\n";
print 'newfs_sysvbfs /dev/r' . $drive . "a\n";
exit;
