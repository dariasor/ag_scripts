#! /usr/bin/perl -w
use strict;
use warnings "all";

sub Main($$$)
{
	my $fname = $_[0];
	my $bkeyno = $_[1] - 1;
	my $akeyno = $_[2] - 1;
     	open (my $in, "<", $fname) or die $!;
	my @blines = <$in>;
	
	my @substrings = ();
	foreach my $bline (@blines) {
        chomp($bline);
		my @bfeatures = split('\t', $bline);
		push @substrings $bfeatures[$bkeyno];
		
	}	

	while (<STDIN>) {
		my $aline = $_;
		chomp($aline);
		my @afeatures = split('\t', $aline);
		my $akey = $afeatures[$akeyno];
		foreach my $substring (@substrings) {
			if (index($akey, $substring) != -1) {
				print "$aline\n";
				last;
			}
		} 		
    }
}

Main($ARGV[0],$ARGV[1],$ARGV[2]);

