
    // Insert a listener to be called after the window has unloaded
    window.addEventListener ("unload", onUnloadAPL, false);


// Function called for onunload
function onUnloadAPL ()
{
    // Get a ptr to our local preference branch
var lclPrefs = Components.classes["@mozilla.org/preferences-service;1"]
                         .getService (Components.interfaces.nsIPrefService)
                         .getBranch (gAplCharsRoot);
    // Change the "customize" preference to trigger an image change
    lclPrefs.setBoolPref (gAplCharsCustomize, !lclPrefs.getBoolPref (gAplCharsCustomize));
} // End onLoadAPL


