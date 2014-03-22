<?php
// Script to increment version # in an <version.rc> file
// Date:  1 January 2007
// Author:  Bob Smith



// Parse the argument line
if ($_SERVER['argc'] != 2)
{
    // Display a help message
    echo "Help!  Need one argument:  typically, version.rc\n";
    exit (1);
} // End IF

//****************************************************//
// First command line argument (e.g., version.rc).

$FileName = $_SERVER['argv'][1];

// Read in the file contents
$File = file_get_contents ($FileName);
if ($File == false)
{
    echo "Unable to read the filename <$FileName>\n";
    exit (2);
} // End IF

// Find the version #
preg_match ('/#define VER_MODULE (\d+)/', $File, $matches);

// Increment and save the version #
$VerNum = ++$matches[1];
$VerNumStr = "000$VerNum";
$VerNumStr = substr ($VerNumStr, -4, 4);
$VerNumStr = "$VerNum";

// Replace the old version # with the new one
$File = preg_replace ('/VER_MODULE \d+/',
                      "VER_MODULE $VerNum",
                      $File);
$File = preg_replace ('/VER_MODULE_STR \"\d+\"/',
                      "VER_MODULE_STR \"$VerNumStr\"",
                      $File);

// Write out the file contents
file_put_contents ($FileName, $File);

echo "Version incremented to $VerNum\n";
exit (0);
?>
