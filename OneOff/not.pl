#! /usr/bin/perl -w
use strict;
use warnings "all";

sub Main()
{
	my $thresh = $_[0];
	while (<STDIN>) {
		my $line = $_;
		chomp($line);

		if ($line eq '?') {
			print "?\n";
		}elsif ($line == "0") {
			print "1\n";         
		} else {
			print "0\n";
		}
	}
}

Main();
