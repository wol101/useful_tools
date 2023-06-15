#!/usr/bin/perl
# bogus warnings under perl 5.0
##!/usr/bin/perl -w

$usageString = "Usage:\n$0 [--inputFile file] [--fitnessThreshold x] [--outputIPFiles]"; 

$outputFilesFlag = 0;
$fitnessThreshold = 5;
$fileName = "socket_log.txt";

for ($i = 0;  $i <= $#ARGV; $i++)
{
  if ($ARGV[$i] eq "--inputFile")
  {
    $i++;
    if ($i > $#ARGV) { die "$usageString\n"; }
    $fileName = $ARGV[$i];
    next;
  }
  
  if ($ARGV[$i] eq "--fitnessThreshold")
  {
    $i++;
    if ($i > $#ARGV) { die "$usageString\n"; }
    $fitnessThreshold = $ARGV[$i];
    next;
  }
  
  if ($ARGV[$i] eq "--outputIPFiles")
  {
    $outputFilesFlag = 1;
    next;
  }
  
  die "$usageString\n";
}

$count = 0;

open(IN, "<$fileName") or die "Error opening file to read: $!\n";

while ($line = <IN>)
{
  @listOfTokens = ConvertToListOfTokens($line);

  # collect finish statistics
  if ($listOfTokens[1] eq "finish")
  {
    for ($i = 0; $i <= $#listOfIPAddressess; $i++)
     {
       if ($listOfIPAddressess[$i] eq $listOfTokens[0]) { last; }
     }
     if ($i > $#listOfIPAddressess) { $listOfIPAddressess[$i] = $listOfTokens[0]; }
     $listOfIPIndices[$count] = $i;
     $listOfFitnessScores[$count] = $listOfTokens[4];
     $listOfRunTimes[$count] = $listOfTokens[5];
     $count++;
  }
}
close(IN);

if ($outputFilesFlag)
{
  # write out files for each IP address

  for ($i = 0; $i <= $#listOfIPAddressess; $i++)
  {
    open(OUT, ">$listOfIPAddressess[$i]");
    for ($j = 0; $j <= $#listOfIPIndices; $j++)
    {
      if ($listOfIPIndices[$j] == $i && $listOfFitnessScores[$j] > $fitnessThreshold && $listOfRunTimes[$j] > 0)
      {
        printf(OUT "%f\t%f\t%f\n", $listOfFitnessScores[$j], $listOfRunTimes[$j], $listOfFitnessScores[$j] / $listOfRunTimes[$j]);
      }
    }
    close(OUT);
  }
}
# calculate some summary information

print "Fitness Threshold > $fitnessThreshold\n";
for ($i = 0; $i <= $#listOfIPAddressess; $i++)
{
  $listOfSums[$i] = 0;
  $listOfCounts[$i] = 0;
  @medianList = ();
  $medianCount = 0;
  for ($j = 0; $j <= $#listOfIPIndices; $j++)
  {
    if ($listOfIPIndices[$j] == $i && $listOfFitnessScores[$j] > $fitnessThreshold && $listOfRunTimes[$j] > 0)
    {
      $listOfSums[$i] += $listOfFitnessScores[$j] / $listOfRunTimes[$j];
      $listOfCounts[$i] += 1;
      $medianList[$medianCount] = $listOfFitnessScores[$j] / $listOfRunTimes[$j];
      $medianCount++;
    }
  }
  if ($listOfCounts[$i] > 0)
  {
    $listOfMeans[$i] = $listOfSums[$i] / $listOfCounts[$i];
    @medianList = sort(@medianList);
    $listOfMedians[$i] = $medianList[int($#medianList / 2)];
    print "$listOfIPAddressess[$i] N = $listOfCounts[$i] Mean = $listOfMeans[$i] Median = $listOfMedians[$i]\n";
  }
  else
  {
    print "$listOfIPAddressess[$i] N = $listOfCounts[$i]\n";
  }
}

$minMedian = $listOfMedians[0];
for ($i = 1; $i <= $#listOfMedians; $i++)
{
  if ($listOfMedians[$i] < $minMedian) { $minMedian = $listOfMedians[$i]; }
}

for ($i = 0; $i <= $#listOfIPAddressess; $i++)
{

  if ($listOfCounts[$i] > 0)
  {
    $relativeSpeed[$i] = $listOfMedians[$i] / $minMedian;
    print "$listOfIPAddressess[$i] Relative Speed = $relativeSpeed[$i]\n";
  }
}


# end of program
exit;

sub ConvertToListOfNumbers
{
   my $s = shift;
   my $count = 0;
   my @listOfNumbers = ();
   
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

sub ConvertToListOfTokens
{
   my $s = shift;
   my $count = 0;
   my @listOfTokens = ();
   
   while ($s =~ m/\S\S*/)
   {
#     print "Prematch $`\n";
#     print "Match $&\n";
#     print "Postmatch $'\n";

      $s = $';
      $listOfTokens[$count] = $&;
      $count++;
   }
   
   return @listOfTokens;
}
