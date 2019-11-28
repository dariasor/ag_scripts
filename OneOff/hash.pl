#! /usr/bin/perl -w
use strict;
use warnings "all";

sub Main
{
	my %hash = ();
	my $counter = 1;
	while (<>) {
		my $line = $_;
		chomp($line);
		if (!exists $hash{$line}) {
			$hash{$line} = $counter++;
		}
		print "$hash{$line}\n";
	}
}

Main();
