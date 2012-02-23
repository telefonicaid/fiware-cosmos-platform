#!/usr/bin/perl -w
use strict;

if ($#ARGV < 0) {
    print STDERR <<EOB ;
usage: $0 comscore_file > output_file

Output format:
 * Header: min and max pattern keys on the first two lines
 * Body: pattern key followed by category keys with tabs as separators.
EOB
    exit(1);
}

my ($min, $max);
my %map = ();
while(<>) {
    my ($key, $val) = /^(-?\d+)\s+(-?\d+)/;
    
    if ($map{$key}) {
	push @{$map{$key}}, $val;
    } else {
	$map{$key} = [$val];
    }

    $min = $key unless ($min and $min <= $key);
    $max = $key unless ($max and $max >= $key);
}

print "$min\n$max\n";
for my $key (sort {$a <=> $b} (keys %map)) {
    print "$key\t" . (join "\t", @{$map{$key}}) . "\n";
}
