
// Local dump to Error Console
function myDump (aMessage)
{
  var consoleService = Components.classes["@mozilla.org/consoleservice;1"]
                                 .getService (Components.interfaces.nsIConsoleService);
  consoleService.logStringMessage (aMessage);
} // End myDump

