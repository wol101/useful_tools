#!/usr/bin/perl -w

# choose from a list of best genomes by generation
# e.g. from 1000 generations, 50, 100 , 150 etc

# check the length
if ($#ARGV < 3) { die "Usage: $0 populationSize maxGenerations [files]\n" }

$populationSize = $ARGV[0];
$maxGenerations = $ARGV[1];

# set the file list
for ($i = 2; $i <= $#ARGV; $i++)
{
  $fileList[$i - 2] = $ARGV[$i];
}

# sort the list
@sortedFileList = sort(@fileList);

# create the required list of files
for ($i = 0; $i < $populationSize; $i++)
{
   $targetGeneration = int($i *  $maxGenerations / ($populationSize - 1));
   
   # find the highest genome <= targetGeneration
   
   for ($j = 0; $j <= $#sortedFileList; $j++)
   {
      if (ExtractNumber($sortedFileList[$j]) > $targetGeneration)
      {
         last;
      }
   }
   if ($j > 0)
   {
      $selectedFileList[$i] = $sortedFileList[$j - 1];
   }
   else    
   {   
      $selectedFileList[$i] = $sortedFileList[0];
   }
   printf("%s selected\n", $selectedFileList[$i]);
}

# now convert the list of genomes to a population
open(OUT, ">", "ChosenPopulation" . $populationSize . "_" . $maxGenerations . ".txt") or die "Error opening file to write: $!\n";
printf(OUT "%d\n", $populationSize);
for ($i = 0; $i < $populationSize; $i++)
{
   open(IN, "<", $selectedFileList[$i]) or die "Error opening file to read: $!\n";
   while (<IN>)
   {
      $line = $_;
      printf(OUT "%s", $line);
   }
   close(IN);
} 
close(OUT); 

sub ExtractNumber
{
   # take a local copy of argument
   my $s = shift;

   # search for a number within the string
   $s =~ m/(?:\d+(?:\.\d*)?|\.\d+)/;
   
   # if found, return the number
   if ($&) 
   { 
      return $&;
   }
   
   # otherwise return 0
   return 0;
}

