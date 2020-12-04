#! /usr/bin/perl -w
use strict;
use warnings "all";

sub Main($$)
{
	my $groupCol = $_[0];
	my $valueCol = $_[1];
    my $isFirst = 1;
	my @curGroupLines = ();
    my @valsInCurGroup = (); 
	my $prevGroupId = "";
    while (<STDIN>) {
        my $line = $_;
        chomp($line);
        if (length $line) {

            my @features = split('\t', $line);
			my $curGroupId = $features[$groupCol - 1];
            
            if ($curGroupId ne $prevGroupId) {
                if (!$isFirst) {
                    my@valsSorted = sort {$a <=> $b} @valsInCurGroup;
                    if ($valsSorted[0] != $valsSorted[-1]) {
                        foreach my $savedLine (@curGroupLines) {
                            print "$savedLine\n";
                        } 
                    }
                    @curGroupLines = ();
                    @valsInCurGroup = ();
                }
                $prevGroupId = $curGroupId;
            }
            push @curGroupLines, $line;
            push @valsInCurGroup, $features[$valueCol - 1];
		}
        $isFirst = 0;
	}
    my @valsSorted = sort {$a <=> $b} @valsInCurGroup;
    if ($valsSorted[0] != $valsSorted[-1]) {
        foreach my $savedLine (@curGroupLines) {
            print "$savedLine\n";
        } 
    }
}

Main($ARGV[0],$ARGV[1]);
