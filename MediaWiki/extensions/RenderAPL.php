<?php

// MediaWiki extension to apply a consistent style to APL expressions
//   by defining a new tag name <apl>...</apl>.

$wgHooks['ParserFirstCallInit'][] = 'wfAPLExtension';

$FontName = 'SImPL';

function wfAPLExtension (&$parser)
{
    // Register the extension with the WikiText parser
    // the first parameter is the name of the new tag.
    // In this case it defines the tag <apl> ... </apl>
    // the second parameter is the callback function for
    // processing the text between the tags
    $parser->setHook( "apl",   "renderAPL" );
    $parser->setHook( "apll",  "renderAPLlarge" );
    $parser->sethook( "aplx",  "renderAPLxlarge" );
    $parser->sethook( "aplxx", "renderAPLxxlarge" );

    return true;
} // End wfAPLExtension


// The callback function for converting the input text to HTML output
function renderAPL ($input, $args, $parser, $frame)
{
    return renderAPLcom ($input, $args, $parser, 0, false);
} // End renderAPL


// The callback function for converting the input text to HTML output
function renderAPLlarge ($input, $args, $parser, $frame)
{
    return renderAPLcom ($input, $args, $parser, 1, true);
} // End renderAPLlarge


// The callback function for converting the input text to HTML output
function renderAPLxlarge ($input, $args, $parser, $frame)
{
    return renderAPLcom ($input, $args, $parser, 2, true);
} // End renderAPLxlarge


// The callback function for converting the input text to HTML output
function renderAPLxxlarge ($input, $args, $parser, $frame)
{
    return renderAPLcom ($input, $args, $parser, 3, true);
} // End renderAPLxxlarge


// The callback function for converting the input text to HTML output
function renderAPLcom ($input, $argv, $parser, $iLargeSize, $bBoldWeight)
{
    global $FontName;

    // Disable caching for these parser tags
    $parser->disableCache ();

    // $argv is an array containing any arguments passed to the
    // extension like <example argument="foo" bar>..
    // Put this on the sandbox page:  (works in MediaWiki 1.5.5)
    //   <example argument="foo" argument2="bar">Testing text **example** in between the new tags</example>
    //    $output = "Text passed into example extension: <br/>$input";
    //    $output .= " <br/> and the value for the arg 'argument' is " . $argv["argument"];
    //    $output .= " <br/> and the value for the arg 'argument2' is: " . $argv["argument2"];
    //    return $output;

    $style = 'white-space: nowrap; ';

    if ($argv['size'])
        $style .= 'font-size: '   . $argv['size']   . '; ';
    else
    switch ($iLargeSize)
    {
        case 0:
            break;

        case 1:
            $style .= 'font-size: large; ';

            break;

        case 2:
            $style .= 'font-size: x-large; ';

            break;

        case 3:
            $style .= 'font-size: xx-large; ';

            break;
    } // End SWITCH

    if ($argv['font'])
        $style .= 'font-family: ' . $argv['font']   . '; ';
    else
        $style .= 'font-family: "' . $FontName     . '"; ';

    if ($argv['weight'])
        $style .= 'font-weight: ' . $argv['weight'] . '; ';
    else
    if ($bBoldWeight)
        $style .= 'font-weight: bold; ';

    if ($argv['class'])
        $class = 'class="' . $argv['class'] . '" ';
    else
        $class = '';

    return '<span ' . $class . 'style="' . $style . '">' . $input . '</span>';
} // End renderAPLcom

?>
