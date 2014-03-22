<?php

$wgHooks['OutputPageBeforeHTML'][] = 'fnInsertCSS';

// Insert a reference to a CSS style sheet

// The "main" function
function fnInsertCSS (&$out, &$text)
{
    // Check to see if this style sheet has already been added
    foreach ($out->mLinktags as $tag)
    {
        foreach ($tag as $attr => $val)
            if ($attr == 'href'
             && $val  == '/extensions/apl.css')
                return false;
    } // End FOREACH

    // Add a stylesheet
    $out->addLink (array ('rel'  => 'stylesheet',
                          'type' => 'text/css',
                          'href' => '/extensions/apl.css'
                         ));
    return true;
} // End fnInsertCSS

?>
