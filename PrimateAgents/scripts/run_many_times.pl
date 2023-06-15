#!/usr/bin/perl -w
# run a program a number of times

# check the length
if ($#ARGV != 1) { die "Usage: $0 number_of_repeats program\n" }

$repeats = $ARGV[0];
$program = $ARGV[1];

for ($i = 0; $i < $repeats; $i++)
{
    printf("%s\n", $program);
    system($program);
}
