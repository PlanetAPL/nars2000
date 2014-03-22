
window.addEventListener ("load", initPrefsAPL, false);


// Function to initialize the fields in the Preferences window
function initPrefsAPL ()
{
    // Get a ptr to our local preference branch
var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                         .getService (Components.interfaces.nsIPrefService)
                         .getBranch (gAplCharsRoot);
    // Get and select the current settings
var charsEnabled = lclPrefs.getBoolPref (gAplCharsCharsEnabled);
    document.getElementById ("charsenabled").checked = charsEnabled;

var keyboardlayout = lclPrefs.getCharPref (gAplCharsKeyboardLayout);
var selRB = document.getElementById (keyboardlayout);
    document.getElementById ('keyboardlayout').selectedItem = selRB;

var aplfontfamily = lclPrefs.getCharPref (gAplCharsAplFontFamilyId);
    selRB = document.getElementById (aplfontfamily);
    document.getElementById ('aplfontfamily').selectedItem = selRB;
} // End initPrefsAPL


// Function to save the fields in the Preferences window
function savePrefsAPL ()
{
    // Get a ptr to our local preference branch
var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                         .getService (Components.interfaces.nsIPrefService)
                         .getBranch (gAplCharsRoot);
    // Get and save the current settings
var charsEnabled = document.getElementById ('charsenabled').checked;
    lclPrefs.setBoolPref (gAplCharsCharsEnabled, charsEnabled);

var selRB        = document.getElementById ('keyboardlayout').selectedItem;
    lclPrefs.setCharPref (gAplCharsKeyboardLayout, selRB.id);

    selRB        = document.getElementById ('aplfontfamily').selectedItem;
    lclPrefs.setCharPref (gAplCharsAplFontFamilyName, selRB.value);
    lclPrefs.setCharPref (gAplCharsAplFontFamilyId  , selRB.id);
} // End savePrefsAPL


// Display the About Box
function displayAboutAPL ()
{
    window.open ("chrome://aplchars/content/about.xul",
                 "&prefwindow.abouttitle;", "chrome,centerscreen");
} // End displayAboutAPL

