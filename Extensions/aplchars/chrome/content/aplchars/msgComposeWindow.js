    // Insert as a listener to be called after the window has loaded and unloaded
    window.addEventListener ("load"  , onLoadAPL  , false);
    window.addEventListener ("unload", onUnloadAPL, false);

// Function called when the user clicks on the button
function onClickAPL (bToggle, bFocus)
{
    // Get the enabled state
var charsEnabled = getCharsEnabledAPL (bToggle);

    // Get the button ID and element
var aplButtonId = "APLCharsNormal";
var aplButton = document.getElementById (aplButtonId);
    if (aplButton == null)
    {
        aplButtonId = "APLCharsLarge";
        aplButton = document.getElementById (aplButtonId);
    } // End IF

    // Set the button label to ON/OFF
    aplButton.label =
      charsEnabled ? gAplCharsLabelON
                   : gAplCharsLabelOFF;
    // Set the button tooltip to ON/OFF
    aplButton.tooltipText =
      charsEnabled ? gAplCharsTipON
                   : gAplCharsTipOFF;
    // Set the button image to ON/OFF
    onFocusAPL (bFocus);
} // End onClickAPL


// Function called to retrieve the charsEnabled state
function getCharsEnabledAPL (bToggle)
{
    // Get a ptr to our local preference branch
var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                         .getService (Components.interfaces.nsIPrefService)
                         .getBranch (gAplCharsRoot);
    // Get the current enabled state
var charsEnabled = lclPrefs.getBoolPref (gAplCharsCharsEnabled);

    // If we're to toggle the state, ...
    if (bToggle)
    {
        // Toggle the state
        charsEnabled = !charsEnabled;

        // Save back as the currently selected enabled state
        lclPrefs.setBoolPref (gAplCharsCharsEnabled, charsEnabled);
    } // End IF

    return charsEnabled;
} // End getCharsEnabledAPL


// Function called for onfocus (true) and onblur (false)
function onFocusAPL (bFocus)
{
var iconsize;

    // Get the enabled state
var charsEnabled = getCharsEnabledAPL (false);

    // Get the button ID and element
var aplButtonId = "APLCharsNormal";
var aplButton = document.getElementById (aplButtonId);
    if (aplButton == null)
    {
        aplButtonId = "APLCharsLarge";
        aplButton = document.getElementById (aplButtonId);
    } // End IF

    // Get the toolbar iconsize
    if (aplButton.parentNode.attributes["iconsize"])
        iconsize = aplButton.parentNode.attributes["iconsize"].value;

    // Set the image to display depending upon the focus and enabled states,
    //   the button ID and the iconsize
var image = "url(chrome://aplchars/content/aplchars";

    // Append the enabled state
    image = image + (charsEnabled ? "ON" : "OFF");

    // Append the icon size which depends upon
    //  * the button ID ("APLCharsNormal" or "APLCharsLarge")
    //  * the iconsize ( or "small")

    if (aplButtonId == "APLCharsNormal" && iconsize == "small")
        image = image + "16";
    else
    if (aplButtonId == "APLCharsNormal" && iconsize != "small")
        image = image + "24";
    else
    if (aplButtonId != "APLCharsNormal" && iconsize == "small")
        image = image + "24";
    else
    if (aplButtonId != "APLCharsNormal" && iconsize != "small")
        image = image + "30";

    // Append the selected or unselected state
    image += (bFocus ? "-sel" : "-unsel");

    // Lastly, append the image tail
    image += ".png)";

    // Set the image
    aplButton.style.listStyleImage = image;
} // End onFocusAPL


var gAPLObserver =
{
    observe: function (aSubject, aTopic, aData)
    {
        if (aTopic == "nsPref:changed")
        {
            // aData is "extensions.aplchars.???"
            if (aData == (gAplCharsRoot + gAplCharsCharsEnabled))
                // Set the button image to ON/OFF
                onFocusAPL (false);
        } // End IF
    } // End observe
} // End gAPLObserver


// Function called for onload
function onLoadAPL ()
{
var gPref = Components.classes["@mozilla.org/preferences-service;1"]
                      .getService (Components.interfaces.nsIPrefBranch2);
    gPref.addObserver (gAplCharsRoot, gAPLObserver, false);

    // Simulate a click to initialize the fields
    onClickAPL (false, false);
} // End onLoadAPL


// Function called for onunload
function onUnloadAPL ()
{
var gPref = Components.classes["@mozilla.org/preferences-service;1"]
                      .getService (Components.interfaces.nsIPrefBranch2);
    gPref.removeObserver (gAplCharsRoot, gAPLObserver);
} // End onUnloadAPL


