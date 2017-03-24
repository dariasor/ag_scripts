#! /usr/bin/perl -w
use strict;
use warnings "all";

sub Main($)
{
	my $fname = $_[0];
     	open (my $in, "<", $fname) or die $!;
	my @blines = <$in>;
	
	my %hash = ();
	foreach my $bline (@blines) {
        chomp($bline);
		my $bkey = $bline;
        if ($bline =~ /^([^\t]*)\t/) {
            $bkey = $1;
        }
		$hash{$bkey} = 1;
	}	

	while (<STDIN>) {
		my $aline = $_;
		chomp($aline);
		my $akey = $aline;
		if ($aline =~ /^([^\t]*)\t/) {
			$akey = $1;
		}
		chomp($akey);
		if (exists $hash{$akey}) {
			print "$aline\n";
		} 		
     	}
}

Main($ARGV[0]);

