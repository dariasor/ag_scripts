#! /usr/bin/perl -w
use strict;
use warnings "all";
use Math::Round;

sub Main($$$)
{
	my %hash_n = ();
	my %hash_val = ();
	my $w = $_[2];
	my $col1 = $_[0];
	my $col2 = $_[1];
	while (<STDIN>) {
		my $line = $_;
		chomp($line);
		my @features = split('\t', $line);
		my $value1 = $features[$col1 - 1];
		my $value2 = $features[$col2 - 1];
		my $key = round($value1/$w) + 0.0; #the last part gets rid of -0.0 values
		$hash_n{$key} += 1;
		$hash_val{$key} += $value2;
	}
	my @hashkeys = sort {$a <=> $b} keys %hash_n;
	foreach my $key (@hashkeys) {
		my $hist_center = $key * $w;
		my $hist_value = $hash_val{$key}/$hash_n{$key};
		print "$hist_center\t$hist_value\n";
	} 
}

Main($ARGV[0],$ARGV[1],$ARGV[2]);
