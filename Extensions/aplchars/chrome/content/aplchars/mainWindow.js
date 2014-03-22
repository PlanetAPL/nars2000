
    // Insert a listener to be called after the window has loaded
    window.addEventListener ("load"  , onLoadAPL  , false);


// Function called when the user clicks on the button
function onClickAPL (bToggle, bFocus)
{
    // Get the enabled state
var fontsEnabled = getFontsEnabledAPL (bToggle);

    // Get the button ID and element
var aplButtonId = "APLFontsNormal";
var aplButton = document.getElementById (aplButtonId);
    if (aplButton == null)
    {
        aplButtonId = "APLFontsLarge";
        aplButton = document.getElementById (aplButtonId);
    } // End IF

    // If the button is on the toolbar, ...
    if (aplButton)
    {
        // Set the button label to ON/OFF
        aplButton.label =
          fontsEnabled ? gAplFontsLabelON
                       : gAplFontsLabelOFF;
        // Set the button tooltip to ON/OFF
        aplButton.tooltipText =
          fontsEnabled ? gAplFontsTipON
                       : gAplFontsTipOFF;
        // Set the button image to ON/OFF
        onFocusAPL (bFocus);
    } // End IF

    // If we're changing fonts, ...
    if (bToggle)
    {
        // Get a ptr to our local preference branch
    var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                             .getService (Components.interfaces.nsIPrefService)
                             .getBranch (gAplCharsRoot);
        // Get our font name and type preferences
    var fontNameRoot  = lclPrefs.getCharPref (gAplCharsFontNameRoot);
    var fontNameType  = lclPrefs.getCharPref (gAplCharsFontNameType);
    var oldFontFamilyName;

        // Get a ptr to the font name preference branch
    var glbPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                             .getService (Components.interfaces.nsIPrefService)
                             .getBranch (fontNameRoot);
        // If fonts are now enabled, ...
        if (fontsEnabled)   // Other to APL
        {
            // Get the other font family
            oldFontFamilyName = glbPrefs.getCharPref (fontNameType);

            // Save the other font family
            lclPrefs.setCharPref (gAplCharsOldFontFamilyName, oldFontFamilyName);

            // Get the APL font family name
            oldFontFamilyName = lclPrefs.getCharPref (gAplCharsAplFontFamilyName);
        } else              // APL to Other
            // Get the other font family name
            oldFontFamilyName = lclPrefs.getCharPref (gAplCharsOldFontFamilyName);

        // Change the font to the appropriate family
        glbPrefs.setCharPref (fontNameType, oldFontFamilyName);

        // Re-display the message
        ReloadMessage ();
    } // End IF
} // End onClickAPL


// Function called to retrieve the fontsEnabled state
function getFontsEnabledAPL (bToggle)
{
    // Get a ptr to our local preference branch
var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                         .getService (Components.interfaces.nsIPrefService)
                         .getBranch (gAplCharsRoot);
    // Get the current enabled state
var fontsEnabled = lclPrefs.getBoolPref (gAplCharsFontsEnabled);

    // If we're to toggle the state, ...
    if (bToggle)
    {
        // Toggle the state
        fontsEnabled = !fontsEnabled;

        // Save back as the currently selected enabled state
        lclPrefs.setBoolPref (gAplCharsFontsEnabled, fontsEnabled);
    } // End IF

    return fontsEnabled;
} // End getFontsEnabledAPL


// Function called for onfocus (true) and onblur (false)
function onFocusAPL (bFocus)
{
var iconsize;

    // Get the enabled state
var fontsEnabled = getFontsEnabledAPL (false);

    // Get the button ID and element
var aplButtonId = "APLFontsNormal";
var aplButton = document.getElementById (aplButtonId);
    if (aplButton == null)
    {
        aplButtonId = "APLFontsLarge";
        aplButton = document.getElementById (aplButtonId);
    } // End IF

    try
    {
        // Get the toolbar iconsize
    var attr = aplButton.parentNode.attributes["iconsize"];
        if (attr)
            iconsize = attr.value;
        else
            iconsize = "large";
    } catch (e) {}

    // Set the image to display depending upon the focus and enabled states,
    //   the button ID and the iconsize
var image = "url(chrome://aplchars/content/aplfonts";

    // Append the enabled state
    image = image + (fontsEnabled ? "ON" : "OFF");

    // Append the icon size which depends upon
    //  * the button ID ("APLFontsNormal" or "APLFontsLarge")
    //  * the iconsize ( or "small")

    if (aplButtonId == "APLFontsNormal" && iconsize == "small")
        image = image + "16";
    else
    if (aplButtonId == "APLFontsNormal" && iconsize != "small")
        image = image + "24";
    else
    if (aplButtonId != "APLFontsNormal" && iconsize == "small")
        image = image + "24";
    else
    if (aplButtonId != "APLFontsNormal" && iconsize != "small")
        image = image + "30";

    // Append the selected or unselected state
    image += (bFocus ? "-sel" : "-unsel");

    // Lastly, append the image tail
    image += ".png)";

    // Set the image
    aplButton.style.listStyleImage = image;
} // End onFocusAPL


var gPrefObserverAPL =
{
    observe: function (aSubject, aTopic, aData)
    {
        if (aTopic == "nsPref:changed")
        {
            // aData is "extensions.aplchars.???"
            if (aData == (gAplCharsRoot + gAplCharsCustomize))
                // Simulate a click to initialize the fields
                onClickAPL (false, false);
            else
            if (aData == (gAplCharsRoot + gAplCharsAplFontFamilyId))
            {
                // If fonts are now enabled, ...
                if (getFontsEnabledAPL (false))     // APL to APL
                {
                    // Get a ptr to our local preference branch
                var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                                         .getService (Components.interfaces.nsIPrefService)
                                         .getBranch (gAplCharsRoot);
                    // Get our font name and type preferences
                var fontNameRoot  = lclPrefs.getCharPref (gAplCharsFontNameRoot);
                var fontNameType  = lclPrefs.getCharPref (gAplCharsFontNameType);

                    // Get a ptr to the font name preference branch
                var glbPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                                         .getService (Components.interfaces.nsIPrefService)
                                         .getBranch (fontNameRoot);
                    // Get the APL font family name
                var aplFontFamilyName = lclPrefs.getCharPref (gAplCharsAplFontFamilyName);

                    // Change the font to the appropriate family
                    glbPrefs.setCharPref (fontNameType, aplFontFamilyName);

                    // Re-display the message
                    ReloadMessage ();
                } // End IF
////        } else
////        {
////            // Get a ptr to our local preference branch
////        var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
////                                 .getService (Components.interfaces.nsIPrefService)
////                                 .getBranch (gAplCharsRoot);
////            // Get our font name and type preferences
////        var fontNameRoot  = lclPrefs.getCharPref (gAplCharsFontNameRoot);
////        var fontNameType  = lclPrefs.getCharPref (gAplCharsFontNameType);
////
////            if (aData == (fontNameRoot + fontNameType))
////            {
////                myDump ("aData = " + aData);
////
////                // Get a ptr to the font name preference branch
////            var glbPrefs = Components.classes["@mozilla.org/preferences-service;1"]
////                                     .getService (Components.interfaces.nsIPrefService)
////                                     .getBranch (fontNameRoot);
////                // If the APL font is in effect, ...
////                if (getFontsEnabledAPL (false))
////                {
////                    // Save the font family name back into the local preferences
////                    lclPrefs.setCharPref (fontNameType, glbPrefs.getCharPref (fontNameType));
////                } else
////                {
////
////
////
////                } // End IF/ELSE
////            } // End IF
            } // End IF/ELSE
        } // End IF
    } // End observe
} // End gPrefObserverAPL


// Function called for onload
function onLoadAPL ()
{
    // Install an observer to handle changes to our preferences
var gPref = Components.classes["@mozilla.org/preferences-service;1"]
                      .getService (Components.interfaces.nsIPrefBranch2);
    gPref.addObserver (gAplCharsRoot, gPrefObserverAPL, false);

////     // Get a ptr to our local preference branch
//// var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
////                          .getService (Components.interfaces.nsIPrefService)
////                          .getBranch (gAplCharsRoot);
////     // Get our font name and type preferences
//// var fontNameRoot  = lclPrefs.getCharPref (gAplCharsFontNameRoot);
//// var fontNameType  = lclPrefs.getCharPref (gAplCharsFontNameType);
////     gPref.addObserver (fontNameRoot + fontNameType, gPrefObserverAPL, false);

    // Simulate a click to initialize the fields
    onClickAPL (false, false);
} // End onLoadAPL


