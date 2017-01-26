<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
         "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
  <title>NARS2000 Downloads</title>
  <base target="_top" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <meta name="Braces2APL" content="font" />
  <link rel="stylesheet"
        href="http://www.sudleyplace.com/styles/common.css"
        type="text/css" />
  <link rel="stylesheet"
        href="http://www.nars2000.org/styles/common.css"
        type="text/css" />
  <style type="text/css">
#reltable
{
  margin-left: 1em;
  /* border-collapse: collapse; */
}
#reltable th, #reltable tr, #reltable td
{
  color: rgb(0, 0, 164);
  padding: 0 1em 0 1em;
}
#reltable td
{
  border: solid blue 1px;
  font-weight: bold;
  color: rgb(46, 139, 87); /* seagreen */
}
.dnlbutton
{
  background-color: rgb(255, 255, 224); /* Lightyellow */
}
.notes
{
  display: none;
}
  </style>
</head>

<body><!--#config timefmt="%d %B %Y %H:%M:%S" -->

<h1 class="header"
    title="Last Changed <?php echo date ('d F Y H:i:s', filemtime (__FILE__));?>">
Downloads</h1>

<div class="section">
  <h2 id="Latest_File_Releases">Latest File Releases</h2>

  <p>There are several types of files available for downloading:
    <b>NARS2000-<i>version</i>.exe</b> (or the earlier format files
    <b>NARS2000-<i>version</i>.zip</b>),
    <b>NARS2000-<i>version</i>.map</b>,
    <b>NARS2000-<i>version</i>.pdb</b>,
    <b>Release Notes</b>, and <b>Special Files</b>.  The <b>.exe</b> (or earlier
    format <b>.zip</b>) files contain everything you
    need to run the program and are the recommended downloads for end
    users.  The <b>.map</b> and <b>.pdb</b> files contain debugging information and are used
    by developers.  The <b>Release Notes</b> contain information about the
    corresponding release and are, generally, of most use to developers.  The
    <b>Special Files</b> such as
    <b>gsldir.zip</b>,
    <b>misc.zip</b>,
    <b>mplibs.zip</b>,
    <b>mpfns-src.zip</b>,
    and
    <b>qdebug.zip</b>
    contain support files needed by developers.</p>

  <ol>
    <li><p>Download the latest version (marked <b>Recommended</b>) and save it
      in your choice of directory (preferably its own).</p></li>

    <li><p>This file consists of a small setup program which contains the program file
      (<b>NARS2000.exe</b>), an APL font file (<b>SImPL-Medium.ttf</b>), and a license
      agreement (<b>LICENSE</b>).</p></li>

    <li><p>Run the setup program (<b>NARS2000-<i>version</i>.exe</b>) and enter a directory where you'd
      like to save the contents, or unzip the earlier format file (<b>NARS2000-<i>version</i>.zip</b>)
      into a directory of your choice.</p></li>

    <li><p>The program executable (<b>NARS2000.exe</b>) and the font file must be in the
      same directory for the first (and only the first) time you run the program executable.</p></li>

    <li><p>If you are running under Vista or Win7 (but not WinXP), either run the program executable
      the first time (and only the first time) as Administrator
      so as to have the proper privileges to install a font under program
      control, or install the font manually.  More specifically,</p>

      <ul>
        <li><p>To run an executable as Administrator, right-click the <b>.exe</b>
          file and choose "Run as administrator".  This step is necessary only
          once in order to allow the program to install the APL font; after that,
          run the executable as normal by (say) double-clicking on the <b>.exe</b>
           file.</p>

          <p><b>OR</b></p>
        </li>

        <li><p>To install a font manually, right-click the <b>.ttf</b> file and
          choose "Install".</p>
        </li>
      </ul>

      <p>If you are running under WinXP, neither of the above steps are
        necessary.</p>
    </li>
  </ol>
</div>

<div class="section">
  <h2 id="Changes">Changes</h2>

  <table border="0" cellspacing="2" cellpadding="2" summary="">
    <tr>
      <td valign="top">
        <p>Rather than gather email addresses, maintain a mailing list, deal with
          privacy issues, etc., I'd prefer that if you are interested in being
          notified when a new version is released that you sign up with a (free)
          service that does this such as the one linked to on the right of this
          section (for new <b>releases</b>) and/or the <a class="linkleft" href="/">main</a>
          page (for new <b>features</b>).</p>
      </td>

      <td valign="top">
        <table style="display:inline; width:100px; font-family: arial, helvetica;" border="0" cellpadding="0" cellspacing="0" summary="">
          <tr>
            <td bgcolor="#999999" align="center">
              <span style="color:white; font-size: 12px; font-weight:normal; letter-spacing: 1px;">Be notified of<br />page updates</span>
            </td>
          </tr>
          <tr>
            <td>
              <form style="margin-bottom:0;" action="http://www.changedetection.com/detect.html" method="get" target="ChangeDetectionWiz">
                <input style="width:100px;"
                       type="text"
                       name="email"
                       value="enter email" />
                <table style="font-family: arial, helvetica;" width="100%" border="0" cellspacing="0" cellpadding="0" summary="">
                  <tr>
                    <td align="left" valign="top">
                      <small>
                      <input type="submit"
                             name="enter"
                             value=" OK "
                             onclick="somewin=window.open('http://www.changedetection.com/detect.html', 'ChangeDetectionWiz','resizable=yes,scrollbars=yes,width=624,height=460');somewin.focus()" />
                      </small>
                    </td>
                    <td align="right" valign="top">
                      <a href="http://www.changedetection.com/privacy.html"
                         target="ChangeDetectionPrivacy"
                         onclick="somewin=window.open('http://www.changedetection.com/privacy.html', 'ChangeDetectionPrivacy','resizable=yes,scrollbars=yes,width=624,height=390');somewin.focus()">
                      <span style="font-size: 10px">it's private</span></a>
                      <br />
                    </td>
                  </tr>
                </table>
              </form>
            </td>
          </tr>
          <tr style="height:28px;">
            <td style="font-size: 10px; " align="center" valign="bottom">
              powered by
              <br />
              <a  href="http://www.changedetection.com/">ChangeDetection</a>
            </td>
          </tr>
        </table>
      </td>
    </tr>
  </table>
</div>

<div class="section">
  <h2 id="Beta_Versions">Beta Versions</h2>

  <p>For the latest (<b>untested</b>) versions, goto the <a class="linkleft" href="binaries/beta">beta</a>
    directory and download either the 32- or 64-bit file.  This is a .zip file (as opposed to the released
    version which is an executable Setup file) which you might want to unzip into a directory separate from
    the released installation so as to leave the latter unaffected.  As with the released version, please
    report any bugs to the Bug Report section of the NARS2000
    <a class="linkleft" href="http://nars2000.forumatic.com/viewforum.php?f=12">Forum</a>.</p>
</div>

<div class="section">
  <h2 id="Alpha_Versions">Alpha Versions</h2>

  <p>For the latest (<b>untested</b>) version that implements
    <a class="pdfLeft" href="http://www.sudleyplace.com/APL/HyperComplex%20Numbers%20in%20APL.pdf">Hypercomplex
    Numbers</a>, goto the <a class="linkleft" href="binaries/alpha">alpha</a>
    directory and download either the 32- or 64-bit file.  This is a .zip file (as opposed to the released
    version which is an executable Setup file) which you might want to unzip into a directory separate from
    the released installation so as to leave the latter unaffected.  As with the released version, please
    report any bugs to the Bug Report section of the NARS2000
    <a class="linkleft" href="http://nars2000.forumatic.com/viewforum.php?f=12">Forum</a>.</p>
</div>

<div class="section">
  <h2 id="Download_Files">Download Files</h2>

  <form id="DisplayForm" action="">&nbsp;
    <input type="button" name="displaytype" id="IDtoggle" value="Display all files" onclick="return DisplayTab ();" />
  </form>

  <table border="0" cellspacing="0" summary="">
    <tr>
      <td>
        <table id="reltable" border="0" cellspacing="0" summary="">
        <tr>
          <th>File Name</th>
          <th>Version</th>
          <th>Date/Time (GMT)</th>
          <th>Type</th>
          <th class="notes">Notes</th>
          <th>32-bit (bytes)</th>
          <th>64-bit (bytes)</th>
        </tr>

        <?php
        ob_implicit_flush (true);
        echo "\n";

        $DirName = '/v/nars2000.org/htdocs/download/binaries/';

        $dh = opendir ($DirName);

        // This is the correct way to loop over the directory.
        while (false !== ($File = readdir ($dh)))
        {
            if (!is_dir ($DirName . $File)
             && strcmp  ($File, "linestat.txt") != 0
             && strcmp  ($File, "nars2000.ver") != 0
             && strncmp ($File, "Notes-", 6)    != 0)
            {
                if (strncmp ($File, "Version-", 8) == 0)
                {
                    $Pos  = strpos ($File, '-');
                    $FileName = "NARS2000" . substr ($File, $Pos, -3);

                    // Append the appropriate file (.exe or .zip) to the list
                    if (file_exists ($DirName . "w32/" . $FileName . "exe")
                     || file_exists ($DirName . "w64/" . $FileName . "exe"))
                        $Files[] = $FileName . "exe";
                    else
                    if (file_exists ($DirName . "w32/" . $FileName . "zip"))
                        $Files[] = $FileName . "zip";

                    // Append the appropriate file (.map) to the list, if it exists
                    if (file_exists ($DirName . "w32/" . $FileName . "map")
                     || file_exists ($DirName . "w64/" . $FileName . "map"))
                        $Files[] = $FileName . "map";

                    // Append the appropriate file (.pdb) to the list, if it exists
                    if (file_exists ($DirName . "w32/" . $FileName . "pdb")
                     || file_exists ($DirName . "w64/" . $FileName . "pdb"))
                        $Files[] = $FileName . "pdb";
                } else
                    $Files[] = $File;
            } // End IF
        } // End WHILE

        // Sort files descendingly by version #
        //   (which also sorts the Special Files to the front)
        natsort ($Files);
        $Files = array_reverse ($Files);

        foreach ($Files as $File)
        {
            // Ignore "robots.txt"
            if (strcmp ($File, "robots.txt") == 0)
                continue;

            // Handle special files separately
            $IsSpec = (0
                    || strcmp ($File, "gsldir.zip"     ) == 0
                    || strcmp ($File, "misc.zip"       ) == 0
                    || strcmp ($File, "mpfns-src.zip"  ) == 0
                    || strcmp ($File, "mplibs.zip"     ) == 0
                    || strcmp ($File, "qdebug.zip"     ) == 0
                      );
            if ($IsSpec)
            {
                $Name   = substr ($File, 0, strpos ($File, '.'));
                $Rel    = "";
                $ExtPos = strpos (strrev ($File), '.');
                $Ext    = substr ($File, -$ExtPos);         // Extract the extension
                $Class  = "map";
            } else
            {
                $Pos    = strpos ($File, '-');
                $Name   = substr ($File, 0, $Pos);
                $Rel    = substr ($File, $Pos + 1);
                $ExtPos = strpos (strrev ($File), '.');
                $Ext    = substr ($File, -$ExtPos);         // Extract the extension
                $Rel    = substr ($Rel , 0, -$ExtPos-1);    // Remove trailing extension
                $Class  = $Ext;
                $Notes  = "Version-$Rel.txt";
            } // End IF/ELSE

            $Is32 = is_file ($DirName . "w32/$File");
            $Is64 = is_file ($DirName . "w64/$File");
            $D32  = ($IsSpec ? "" : "w32/" );
            $Date = gmdate ("Y F d H:i:s", filemtime ("$DirName$D32$File"));
            $Sz32 = number_format (filesize ("$DirName$D32/$File"));
            if ($Is64)
                $Sz64 = number_format (filesize ($DirName . "w64/$File"));
            else
                $Sz64 = '';

            echo   "      <tr class=\"$Class\">\n"
               .   "        <td>$Name</td>\n"
               .   "        <td>$Rel</td>\n"
               .   "        <td>$Date</td>\n"
               .   "        <td>$Ext</td>\n"
               . (($Class == 'exe' || $Class == 'zip')
               ?   "        <td class=\"notes\"><a target=\"bodyFrame\" class=\"linkleft\" href=\"binaries/$Notes\" onclick=\"return PageTrack ('binaries/$Notes');\">$Rel</a></td>\n"
               :   "        <td class=\"notes\"></td>\n")
               . (($Is32 || $IsSpec)
               ?   "        <td class=\"dnlbutton\"><a class=\"linkleft\" href=\"binaries/$D32$File\" onclick=\"return PageTrack ('binaries/$D32$File');\">Download ($Sz32)</a></td>\n"
               :   "        <td></td>\n")
               . ($Is64
               ?   "        <td class=\"dnlbutton\"><a class=\"linkleft\" href=\"binaries/w64/$File\" onclick=\"return PageTrack ('binaries/w64/$File');\">Download ($Sz64)</a></td>\n"
               :   "        <td></td>\n")
               .   "      </tr>\n";
        } // End FOREACH

        closedir ($dh);
        ?>
        </table>
      </td>
      <td class="recommended" valign="top"><br /><b>&nbsp;&lArr;&nbsp;Recommended</b></td>
    </tr>
  </table>

  <script type="text/javascript">
  <!--
    function DisplayTab ()
    {
    var rows = document.getElementsByTagName ('tr');
    var i;
        for (i = 0; i < rows.length; i++)
        {
        var tr = rows[i];
////////var attr = tr.getAttribute ('class');   // Doesn't work in IE
        var attr = tr.attributes['class'];
            if (attr && (attr.value == 'map'
                      || attr.value == 'pdb'))
                tr.style.display = gAllFiles ? gTableRow : 'none';
        } // End FOR

    var el = document.getElementById ('IDtoggle');

        // Toggle the button text
        el.value = gAllFiles ? 'Display end user files only'
                             : 'Display all files';
        // Toggle the display style of elements with class "notes"
        ToggleClassNotes (document.getElementsByTagName ('td'));
        ToggleClassNotes (document.getElementsByTagName ('th'));

        // Toggle the state
        gAllFiles = !gAllFiles;

        return false;
    } // End DisplayTab

    function ToggleClassNotes (rows)
    {
    var i;
    var DispNotes = gAllFiles ? gTableCell : 'none';
    var DispRecom = gAllFiles ? 'none' : gTableCell;

        for (i = 0; i < rows.length; i++)
        {
        var td = rows[i];
        var attr = td.attributes['class'];
            if (attr)
            {
                if (attr.value == 'notes')
                    td.style.display = DispNotes;
                else
                if (attr.value == 'recommended')
                    td.style.display = DispRecom;
            } // End IF
        } // End FOR
    } // End ToggleClassNotes

    // We need to overcome various problems with IE
var gIE = (navigator.appName == "Microsoft Internet Explorer");
    if (!gIE)
    {
        // IE screws up border-collapse, so we enable
        //   it for non-IE browsers.  On IE, the inner
        //   lines will be a little thicker than necessary,
        //   but if we allow border-collapse on IE then the
        //   bottom line for .exe- or .zip-only display is missing.
    var el = document.getElementById ('reltable');
        el.style.borderCollapse = 'collapse';
    } // End IF

    // Overcome mis-interpretation/limitation/bug in IE
var gTableRow  = gIE ? 'block' : 'table-row';
var gTableCell = gIE ? 'block' : 'table-cell';

    // Start with just .exe and .zip files displayed
var gAllFiles = false;
    document.getElementById ("IDtoggle").click ();
  -->
  </script>
</div>

<div class="section">
  <h2 id="Statistics">Statistics On The Most Current File Release</h2>

  <pre style="margin-left: 1em;"><?php readfile ($DirName . "linestat.txt");?></pre>
</div>

<?php include "/www/htdocs/ValidXHTML-CSS.html" ?>
<?php include "../footer-mon.html" ?>
</body></html>
