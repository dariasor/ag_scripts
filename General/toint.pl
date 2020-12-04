#! /usr/bin/perl -w
use strict;
use warnings "all";

use Math::Round;

sub Main()
{
	my $thresh = $_[0];
	while (<STDIN>) {
		my $line = $_;
		chomp($line);

		if ($line eq '?') {
			print "?\n";
		} else {
			my $intval = round($line);
			print "$intval\n";
		}
	}
}

Main();
