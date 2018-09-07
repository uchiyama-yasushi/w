#!/usr/pkg/bin/perl
$drive = 'vnd0';
$dos = 2 * 1024 * 1024;
$dos_start = 63;
$bfs = 1 * 1024 * 1024;
$swap = 1 * 1024 * 1024;

open(IN, "fdisk $drive |");
$dummy = <IN>; $dummy = <IN>;
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
#print "fdisk -f -u -0 -s '12/$dos_start/$dos' $drive\n"; #  FAT32 LBA-mapped
#print "fdisk -f -u -0 -s '6/$dos_start/$dos' $drive\n"; #  FAT16 >=32M
print "fdisk -f -u -0 -s '4/$dos_start/$dos' $drive\n"; #  FAT16 < 32M
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

$a_start = $netbsd_start;
$a_size = $netbsd_size - ($swap + $bfs) / 512;
$swap_start = $a_start + $a_size;
$swap_size = $swap / 512;
$bfs_start = $swap_start + $swap_size;
$bfs_size = $bfs / 512;

print OUT "5 partitions:\n";
print OUT "  a: $a_size $a_start 4.2BSD 1024 8192 16\n";
print OUT "  b: $swap_size $swap_start swap\n";
print OUT "  c: $netbsd_size $netbsd_start unused 0 0\n";
print OUT "  d: $whole 0 unused 0 0\n";
print OUT "  e: $dos $dos_start MSDOS\n";
print OUT "  f: $bfs_size $bfs_start SysVBFS\n";
close(OUT);
print "disklabel -R $drive new\n";

print 'newfs -O 2 /dev/r' . $drive . "a\n";
#print 'newfs_msdos -F 16 /dev/r' . $drive . "e\n";
print 'newfs_sysvbfs /dev/r' . $drive . "f\n";
exit;
