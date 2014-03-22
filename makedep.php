<?php
// Script to strip out all but prototype text from a .c file
// Date:  15 December 2008
// Author:  Bob Smith

$Debug = false;


//****************************************************//
// Parse the argument line and process switches

// Get the arg count
$argc = $_SERVER['argc'];

// Start with the normal substitution of self for self
$PathSub = array ("." => "");

// Loop through the args, skipping over the invoking filename ($_SERVER['argv'][0]).
for ($i = 1; $i < $argc; $i++)
switch ($_SERVER['argv'][$i][0])
{
    case '-':
        $argv = $_SERVER['argv'][$i];
        if ($argv[1] == 's')
        {
            // Separate out multiple path substitutions separated by commas
            $PathMul = explode (",", substr ($argv, 2));

            // Loop through the parts
            foreach ($PathMul as $Path)
            {
                // Separate the two pieces
                $Path = explode ("=", $Path);

                // If the key has a trailing slash, remove it
                if (substr ($Path[0], -1) == "\\")
                    $Path[0] = substr ($Path[0], 0, -1);

                // Append to the path substitution array
                $PathSub[$Path[0]] = $Path[1];
            } // End FOREACH
        } else
        {
            fprintf (STDERR, "depscan.php:  ***Invalid switch:  $argv\r\n");
            exit (1);
        } // End IF/ELSE

        break;

    default:
        $FileName = $_SERVER['argv'][$i];   // Get the input file

        break;
} // End FOR

if ($Debug)
    print_r ($PathSub);

// Pass the set of includes to a recursive function
ProcessIncludes ($FileName, ".", true);

exit (0);

// Function to prepend a global directory to another dirname
function PrependDir ($FileDir, $GlbDir)
{
    if (substr ($FileDir, 0, 1) != "\\"
     && $GlbDir != ".")
    {
        if ($FileDir == ".")
            $FileDir = $GlbDir;
        else
        if (substr ($FileDir, 0, 2) == ".\\")
            $FileDir = $GlbDir . substr ($FileDir, 1);
        else
            $FileDir = $GlbDir . "\\" . $FileDir;
    } // End IF

    return $FileDir;
} // End PrependDir


// Recursive function to process includes
function ProcessIncludes ($FileName, $GlbDir, $bFirstTime)
{
    global $PathSub, $Debug;

    // Get the file directory
    $FileDir = PrependDir (dirname ($FileName), $GlbDir);

    // Get the file extension
    $path_parts = pathinfo ($FileName);
    $FileExt = "." . $path_parts['extension'];

    // Get the base of the filename (without the extension)
    $FileBase = basename ($FileName, $FileExt);

    // Use common case for comparisons
    $FileExt = strtolower ($FileExt);

    if ($Debug)
        fprintf (STDERR, "GlbDir=$GlbDir, Filename=$FileName, FileDir=$FileDir, FileBase=$FileBase, FileExt=$FileExt\r\n");

    // If this is the first time, ...
    if ($bFirstTime)
    {
        // Get the suffix
        if ($FileExt == ".c")
            $Suffix = "_DEP";
        else
        if ($FileExt == ".cc")
            $Suffix = "_DEP";
        else
        if ($FileExt == ".rc")
            $Suffix = "_RCDEP";
        else
        {
            fprintf (STDERR, "depscan.php:  ***Unknown extension:  $FileName\r\n");
            exit (1);
        } // End IF/ELSE/...

        // Form the filename followed by "_DEP" (for .c files) or "_DEV" (for .rc files)
        $Equate = strtoupper ($FileBase . $Suffix);
    } else
        $Equate = $FileName;

    if ($Debug)
        fprintf (STDERR, "Equate = $Equate\r\n");

    // Glue together the real name
    $RealName = $FileDir . "\\" . $FileBase . $FileExt;

    if ($Debug)
        fprintf (STDERR, "RealName = $RealName\r\n");

    // If the file exists, ...
    if (file_exists ($RealName))
        // Read in the file contents
        $FileNew = file_get_contents ($RealName);
    else
        $FileNew = false;

    // If the file is not found, ...
    if ($FileNew == false)
    {
        // If this is the first time, ...
        if ($bFirstTime)
        {
            echo "Unable to read the filename <$RealName>\r\n";
            exit (2);
        } else
            // Otherwise, just ignore it (e.g. "stdio.h")
            return;
    } // End IF/ELSE

    // Get a list of #include "..." stmts
    $Num = preg_match_all ("/^#\s*include\s*\"(.*?)\"/m", $FileNew, $Matches, PREG_SET_ORDER);

    if ($Debug)
        fprintf (STDERR, "Num = $Num\r\n");

    if ($bFirstTime)
        echo $Equate . " =";

    // If there are any #include stmts, ...
    if ($Num)
    {
        // Initialize the # valid #include stmts
        $GlbInc = 0;

        // Loop through the #include stmts
        for ($i = 0; $i < $Num; $i++)
        {
            if ($Debug)
                fprintf (STDERR, "Matches[0] = " . $Matches[$i][0] . "; Matches[1] = " . $Matches[$i][1] . "\r\n");

            // Get the file's real name
            $RealName = GetRealName ($Matches[$i][1], $FileDir);
            $IncDir = dirname ($RealName);

            if ($Debug)
                fprintf (STDERR, "RealName = $RealName\r\n");

            // Count the # valid #include stmts
            $GlbInc += array_key_exists ($IncDir, $PathSub)
                    && (file_exists ($RealName));
        } // End FOR

        if ($Debug)
            fprintf (STDERR, "GlbInc = $GlbInc\r\n");

        if ($GlbInc == 0)
            return;

        if ($bFirstTime)
            echo            "\\\r\n";
        else
            echo $Equate . ":\\\r\n";

        // Initialize the local count of valid #include stmts
        $LclInc = 0;

        // Loop through the #include stmts
        for ($i = 0; $i < $Num; $i++)
        {
            // Get the file's real name
            $RealName = GetRealName ($Matches[$i][1], $FileDir);
            $IncDir = dirname ($RealName);

            if (!array_key_exists ($IncDir, $PathSub)
             || !file_exists ($RealName))
                continue;

            // Count in another valid #include stmt
            $LclInc++;

            // Get the substitution prefix
            $FileSub = $PathSub[$IncDir];

            echo "\t" . $FileSub . basename ($RealName) . (($LclInc == $GlbInc) ? "\r\n" : "\\") . "\r\n";
        } // End FOR

        // Call ourselves recursively
        for ($i = 0; $i < $Num; $i++)
        {
            // Get the file's real name
            $RealName = GetRealName ($Matches[$i][1], $FileDir);
            $IncDir = dirname ($RealName);

            if (!array_key_exists ($IncDir, $PathSub)
             || !file_exists ($RealName))
                continue;

            ProcessIncludes (basename ($RealName), $IncDir, false);
        } // End FOR
    } // End IF
} // End ProcessIncludes


// Function to get a file's real name
function GetRealName ($FileName, $FileDir)
{
    // Get the #include directory
    $IncDir = PrependDir (dirname ($FileName), $FileDir);

    // Get the #include base
    $IncBase = basename ($FileName);

    // Glue together the real name
    $RealName = $IncDir . "\\" . $IncBase;

    if (!file_exists ($RealName)
     && file_exists ($IncBase))
        return $IncBase;
    else
        return $RealName;
} // End GetRealName

?>
