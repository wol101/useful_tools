#!/usr/bin/perl -w

# choose the bits I want from a genome

if ($#ARGV <= 2) { die "Usage: $0 startpos endpos file1 file2 ...\n" }

$startPos = $ARGV[0];
$endPos = $ARGV[1];
for ($i = 2; $i <= $#ARGV; $i++)
{
   $inFile[$i - 2] = $ARGV[$i];
}
$newGenomeLength = $endPos - $startPos; 

for ($k = 0; $k <= $#inFile; $k++)
{
   # read whole file into buffer
   open(IN, "<", $inFile[$k]) or die "Error opening file to read: $!\n";
   $buffer = "";
   while (<IN>)
   {
      # concatenate each line onto buffer
      $buffer = $buffer . $_;
   }
   close(IN);
   rename $inFile[$k], $inFile[$k] . ".bak";

   @listOfNumbers = ConvertToListOfNumbers($buffer);

   # write out new genome
   open(OUT, ">", $inFile[$k]) or die "Error opening file to write: $!\n";
   printf(OUT "%d\n", $newGenomeLength);

   $genomeLength = $listOfNumbers[0];
   for ($i = 0; $i < $genomeLength; $i++)
   {
      if ($i >= $startPos && $i < $endPos - 1)
      {
         printf(OUT "%.16g\t", $listOfNumbers[$i + 1]);
      }
      if ($i == $endPos - 1)
      {
         printf(OUT "%.16g\n", $listOfNumbers[$i + 1]);
      }
   }
   
   # output the extra values needed
   for ($i = 0; $i < 6; $i++)
   {
      printf(OUT "%g\t", $listOfNumbers[$i + $genomeLength]);
   }
   printf(OUT "%g\n", $listOfNumbers[$i + $genomeLength]);
   close(OUT);
}

# end of program

sub ConvertToListOfNumbers
{
   my $s = shift;
   my $count = 0;
   
   while ($s =~ m/([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?/)
   {
#     print "Prematch $`\n";
#     print "Match $&\n";
#     print "Postmatch $'\n";

      $s = $';
      $listOfNumbers[$count] = $&;
      $count++;
   }
   
   return @listOfNumbers;
}
   
  
