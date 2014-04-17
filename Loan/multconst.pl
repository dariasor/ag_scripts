#! /usr/bin/perl -w
use strict;
use warnings "all";

sub Main
{
     	while (<>) {
        	my $line = $_;
        	chomp($line);

		if ($line eq '?') {
			print "?\n";
        	} else {
			my $retval = -1 * $line;
			print "$retval\n";
		}
	}
}

Main();
