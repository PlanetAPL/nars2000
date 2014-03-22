<?php

/*
1.  Use this script to convert a web page using APL brace notation
    WSIS (Workspace Interchange Standard) to APL385 or SImPL Unicode
    characters.

2.  The web page is expected to already mark the APL characters
    as belonging to the font family of either SImPL or "APL385 Unicode"
    in one way or another.  For example, you might use a Style Sheet
    which contains a class (say, "apl") as follows:

    <style type="text/css">
      .apl {font-family: "APL385 Unicode", monospace;}
    </style>

    and then surround inline APL characters with

    <span class="apl"> ... </span>

    or a block of APL characters with

    <div class="apl"> ... </div>

    etc.

    These fonts may be downloaded from

    http://www.vector.org.uk/resource/apl385.zip
    http://www.vector.org.uk/resource/simpl02.zip

3.  Use WSIS notation as in {rho} to represent the corresponding
    APL character.  This script will convert those five characters to
    &#9076; which is rho in any APL unicode font.

    To display a left brace on a web page, use &#123; -- there is
    no need to encode specially a right brace as the algorithm below
    initially searches for a left brace only.

    Ensure that there are no spaces within the braces surrounding
    a keyword you want translated to an APL character.  For example

    { rho} and {rho } and { rho } will NOT be translated, but
    {rho} will.

4.  The web page content is searched for a special statement of the form
    <meta name="Braces2APL" content="..." /> where the content portion
    contains a comma-separated list of keywords which describe special
    handling of the conversions.

    Keyword     Meaning
    ----------------------
    title       Surround the symbol with <span title="..."> </span>
                  where "..." is the symbol name.
    font=name   Surround the symbol with <span style="font-family: name">...</span>
    ***FIXME** -- isn't sensitve to "=name"

5.  This script was written by Bob Smith <bsmith@sudleyplace.com>,
    and is released under the GPL (GNU Public License).
*/

$wgHooks['ParserAfterTidy'][] = 'fnBracesToUnicode';

$Home      = '/rul/mediawiki';      // Do not end with a backslash
$PathToExt = 'extensions';          // Do not start or end with a backslash
$FileName  = 'b2a.inc.php';         // Do not start with a backslash

// Get the array of mappings of names to numbers
require_once ("$Home/$PathToExt/$FileName");

// Initialize various flags
$bTitle = false;    // TRUE iff surround Unicode symbol with <span title="..."> </span>
$bFont  = false;    // TRUE iff surround Unicode symbol with <span style="..."> </span>
$FontName = "SImPL";


//*************************** FUNCTIONS ***********************//

// The "main" function
function fnBracesToUnicode (&$parser, &$out)
{
    $out = ProcessFile ($out);

    return true;
} // End fnBracesToUnicode


// Process an entire file
function ProcessFile ($File)
{
    global $br2a, $MaxKLen, $bTitle, $bFont, $FontName;

    // Initialize last Left Brace position
    $LastLBpos = 0;

    // Initialize output string
    $Out = '';

    // Search through the file for left braces
    while (false !== ($LBpos = strpos ($File, "{", $LastLBpos)))
    {
        // Output whatever preceded this left brace
        $Out .= substr ($File, $LastLBpos, $LBpos - $LastLBpos);

        // Find the matching right brace within a reasonable
        //   distance (length of longest keyword) and return the
        //   Character Entity number.  The "+2" after $MaxKLen
        //   accounts for the leading and trailing braces.
        if (false !== ($RBpos = strpos (substr ($File, $LBpos, $MaxKLen + 2), "}"))
         && !empty ($br2a[substr ($File, $LBpos + 1, $RBpos - 1)]))
        {
            $Name = substr ($File, $LBpos + 1, $RBpos - 1);
            $CENum = $br2a[$Name];

            // If both title and font keywords are specified,
            //   use one <span>
            if ($bTitle && $bFont)
                $Out .= "<span title=\"$Name\" style=\"font-family: '$FontName';\">";
            else
            // If we're to surround with title only, do so now
            if ($bTitle)
                $Out .= "<span title=\"$Name\">";
            else
            // If we're to surround with font only, do so now
            if ($bFont)
                $Out .= "<span style=\"font-family: '$FontName'; font-size: large;\">";

            // Output the Character Entity
            $Out .= '&#' . $CENum . ';';

            if ($bTitle || $bFont)
                $Out .= "</span>";

            // Skip over the keyword and right brace
            $LastLBpos = $LBpos + $RBpos + 1;
        } else
        {
            // Print the left brace and skip over it
            $Out .= '{';
            $LastLBpos = $LBpos + 1;
        } // End IF/ELSE
    } // End WHILE

    // No more (or perhaps not any) left braces -- output the rest of the file
    $Out .= substr ($File, $LastLBpos);

    return $Out;
} // End ProcessFile ()


?>
