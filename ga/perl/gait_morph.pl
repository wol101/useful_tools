#!/usr/bin/perl -w

$usageString = "Usage:\n$0 [--controlFile file] [--jumpPercent x]";

$controlFile = "gait_morph_control.txt";
$jumpFlag = 0;

for ($i = 0;  $i <= $#ARGV; $i++)
{
  if ($ARGV[$i] eq "--controlFile")
  {
    $i++;
    if ($i > $#ARGV) { die "$usageString\n"; }
    $controlFile = $ARGV[$i];
    next;
  }
    
  if ($ARGV[$i] eq "--jumpPercent")
  {
    $i++;
    if ($i > $#ARGV) { die "$usageString\n"; }
    $jumpFlag = 1;
    $jumpPercent = $ARGV[$i];
    next;
  }
    
  die "$usageString\n";
}

@tokenList = ConvertFileToTokens($controlFile);

if (GetValueFromTokenList("startPercent", $kRangeControlStartPercent, @tokenList)) { die "startPercent undefined"; }
if (GetValueFromTokenList("endPercent", $kRangeControlEndPercent, @tokenList)) { die "endPercent undefined"; }
if (GetValueFromTokenList("incrementPercent", $kRangeControlIncrementPercent, @tokenList)) { die "incrementPercent undefined"; }
if (GetValueFromTokenList("rootConfigFile1", $rootConfigFile1, @tokenList)) { die "rootConfigFile1 undefined"; }
if (GetValueFromTokenList("rootConfigFile2", $rootConfigFile2, @tokenList)) { die "rootConfigFile2 undefined"; }
if (GetValueFromTokenList("rootParameterFile", $rootParameterFile, @tokenList)) { die "rootParameterFile undefined"; }
if (GetValueFromTokenList("rootGenomeFile", $rootGenomeFile, @tokenList)) { die "rootGenomeFile undefined"; }
if (GetValueFromTokenList("rootPopulationFile", $rootPopulationFile, @tokenList)) { die "rootPopulationFile undefined"; }
if (GetValueFromTokenList("rootModelStateFile", $rootModelStateFile, @tokenList)) { die "rootModelStateFile undefined"; }
if (GetValueFromTokenList("workingConfigFile", $workingConfigFile, @tokenList)) { die "workingConfigFile undefined"; }
if (GetValueFromTokenList("workingParameterFile", $workingParameterFile, @tokenList)) { die "workingParameterFile undefined"; }
if (GetValueFromTokenList("workingModelState", $workingModelState, @tokenList)) { die "workingModelState undefined"; }
if (GetValueFromTokenList("gaCommand", $gaCommand, @tokenList)) { die "gaCommand undefined"; }
if (GetValueFromTokenList("objectiveCommand", $objectiveCommand, @tokenList)) { die "objectiveCommand undefined"; }
if (GetValueFromTokenList("mergeCommand", $mergeCommand, @tokenList)) { die "mergeCommand undefined"; }
if (GetValueFromTokenList("zeroThreshold", $zeroThreshold, @tokenList)) { die "zeroThreshold undefined"; }
if (GetValueFromTokenList("workingGenome", $workingGenome, @tokenList)) { die "workingGenome undefined"; }
if (GetValueFromTokenList("workingPopulation", $workingPopulation, @tokenList)) { die "workingPopulation undefined"; }
if (GetValueFromTokenList("usePopulationFlag", $usePopulationFlag, @tokenList)) { die "usePopulationFlag undefined"; }

if ($jumpFlag)
{
  $kRangeControlStartPercent = $jumpPercent;
}
else
{
  if ($usePopulationFlag) 
  {
    print "Copying $rootPopulationFile to $workingPopulation\n";
    open(IN, "<$rootPopulationFile") or die "Error opening file to read: $!\n";
    open(OUT, ">$workingPopulation") or die "Error opening file to read: $!\n";
    while ($line = <IN>)
    {
        printf(OUT "%s", $line);
    }
    close(IN);
    close(OUT);
  }
  else
  {
    print "Copying $rootGenomeFile to $workingGenome\n";
    open(IN, "<$rootGenomeFile") or die "Error opening file to read: $!\n";
    open(OUT, ">$workingGenome") or die "Error opening file to read: $!\n";
    while ($line = <IN>)
    {
        printf(OUT "%s", $line);
    }
    close(IN);
    close(OUT);
  }

  print "Copying $rootModelStateFile to $workingModelState\n";
  open(IN, "<$rootModelStateFile") or die "Error opening file to read: $!\n";
  open(OUT, ">$workingModelState") or die "Error opening file to read: $!\n";
  while ($line = <IN>)
  {
      printf(OUT "%s", $line);
  }
  close(IN);
  close(OUT);
}

for ($kRangeControlPercent = $kRangeControlStartPercent; 
  ($kRangeControlPercent * $kRangeControlIncrementPercent) 
  <= ($kRangeControlEndPercent * $kRangeControlIncrementPercent); 
  $kRangeControlPercent += $kRangeControlIncrementPercent)
{
  $kRangeControl = $kRangeControlPercent / 100;

  # build the working config file

  $command = "$mergeCommand $rootConfigFile1 $rootConfigFile2 $kRangeControl tempConfigFile.txt";
  print "$command\n";
  system $command;
  
  open(OUT, ">$workingConfigFile");
  printf(OUT "kRangeControl %f\n\n", $kRangeControl);
  
  # handle reparsing the Model State file
  
  @tokenList = ConvertFileToTokens($workingModelState);
  for ($i = 0; $i <= $#tokenList; $i++)
  {
     if ($tokenList[$i] eq "kTorsoPosition")
     {
        printf(OUT "\nkTorsoPosition %.16g %.16g %.16g %.16g\n%.16g %.16g %.16g\n",
           $tokenList[$i + 1],   
           $tokenList[$i + 2],   
           $tokenList[$i + 3],   
           $tokenList[$i + 4],   
           0, 
           0, 
           $tokenList[$i + 7]);
        $i += 7;
        next;
     }

     if (IsANumber($tokenList[$i]))
     {
        if (abs($tokenList[$i]) < $zeroThreshold) { printf(OUT " 0"); }
        else { printf(OUT " %.16g", $tokenList[$i]); }
        next;
     }
     
     printf(OUT "\n%s", $tokenList[$i]);
  }
  printf(OUT "\n\n");
  
  open(IN, "<tempConfigFile.txt") or die "Error opening file to read: $!\n";
  while ($line = <IN>)
  {
      printf(OUT "%s", $line);
  }
  printf(OUT "\n");
  close(IN);
  close(OUT);
  
  # build the working parameter file

  open(OUT, ">$workingParameterFile");
  if ($usePopulationFlag) { printf(OUT "startingPopulation %s\n\n", $workingPopulation); }
  else    { printf(OUT "modelGenome %s\n\n", $workingGenome); }
  printf(OUT "extraDataFile %s\n\n", $workingConfigFile);
  open(IN, "<$rootParameterFile") or die "Error opening file to read: $!\n";
  while ($line = <IN>)
  {
      printf(OUT "%s", $line);
  }
  printf(OUT "\n");
  close(IN);
  close(OUT);
  
  # run the ga

  $command = "$gaCommand -p $workingParameterFile -b $workingGenome -l $workingPopulation";
  print "$command\n";
  system $command;  
  
  # run the objective
  
  $command = "$objectiveCommand -c $workingConfigFile -g $workingGenome";
  print "$command\n";
  system $command;  

}

exit;

sub ConvertFileToTokens
{
   my $s = shift;
   my $buffer = "";
   
   # read whole file into buffer
   open(IN, "<$s") or die "Error opening file to read: $!\n";
   $buffer = "";
   while (<IN>)
   {
      # concatenate each line onto buffer
      $buffer = $buffer . $_;
   }
   close(IN);
   
   return ConvertToListOfTokens($buffer);
}
   
sub ConvertToListOfTokens
{
   my $s = shift;
   my $count = 0;
   my @listOfTokens = ();
   my $token;
   my $index;
   
   while ($s =~ m/\S\S*/)
   {
#     print "Prematch $`\n";
#     print "Match $&\n";
#     print "Postmatch $'\n";
      
      $token = $&;
      $s = $';
      if (substr($token, 0, 1) eq '"')
      {
         $token = $token . $s;
         $index = index($token, '"', 1);
         if ($index == -1) { die "Umatched quote after $listOfTokens[$count]\n"; }
         $s = substr($token, $index + 1);
         $token = substr($token, 1, $index - 1);
      }
      $listOfTokens[$count] = $token;
      $count++;
   }
   
   return @listOfTokens;
}
   
sub GetValueFromTokenList
{
   my $i;
   
   for ($i = 2; $i <= $#_ - 1; $i++)
   {
      if ($_[$i] eq $_[0])
      {
         $_[1] = $_[$i + 1];
         return 0;
      }
   }
   return 1;
}

sub IsANumber
{
   return $_[0] =~ m/([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?/;
}
