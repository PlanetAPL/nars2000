//***************************************************************************
//  NARS2000 -- Update Check Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include "headers.h"

#ifdef DEBUG
    #define BoolTest(a)                 {if (!(a)) {UINT uRet = GetLastError (); DbgBrk ();}}
#else
    #define BoolTest(a)                 (a);
#endif
#define SetPBMState(hDlg,pbmState)  SendDlgItemMessageW (hDlg, IDC_DNL_PB, PBM_SETSTATE, pbmState, 0)
#define GetPBMState(hDlg)           SendDlgItemMessageW (hDlg, IDC_DNL_PB, PBM_GETSTATE,        0, 0)

typedef struct tagUPDATESDLGSTR
{
    LPWCHAR lpFilVer,                   // Ptr to file version string
            lpWebVer;                   // Ptr to web  ...
    APLUINT aplFilVer,                  // File version # as single 64-bit #
            aplWebVer;                  // Web ...
    UBOOL   bForceDownload;             // TRUE iff we should force a download
} UPDATESDLGSTR, *LPUPDATESDLGSTR;

typedef struct tagDNLDLGSTR
{
    UINT      uNumBytesOrig,            // # bytes in the file to download
              uNumBytesAlloc,           // ...     allocated for the buffer
              uNumBytesTotal;           // ...     total downloaded
    HINTERNET hNetOpen,                 // Net handle
              hNetOpenUrl,              // ...
              hTempFile;                // Handle to temporary file
    LPVOID    lpMem;                    // Ptr to temporary buffer
    HWND      hDlgUI;                   // Window handle of "Check for Updates" dialog
    LPWCHAR   lpwszTempPathBuffer,      // Ptr to temp path buffer
              lpwszTempFileName;        // ...         filename
    LPUBOOL   lpbDeleteFile;            // Ptr to TRUE iff we should delete the downloaded file after running it
} DNLDLGSTR, *LPDNLDLGSTR;

typedef struct tagDNLTHRSTR
{
    LPDNLDLGSTR lpDnlDlgStr;            // Ptr to the outer DNLDLGSTR
    HANDLE      hThreadWork,            // Handle of the worker thread
                hThreadUI,              // ...           UI     ...
                hSuspend,               // ...           suspend event
                WaitHandle;             // Wait handle
    UBOOL       bThreadActive,          // TRUE iff the thread is active (not paused)
                bThreadStop,            // TRUE iff we want the thread to stop
                bThreadSuspend,         // TRUE iff we want the thread to suspend
                bThreadDone;            // TRUE iff we've processed WaitForDownload.
    DWORD       dwThreadId;             // Thread ID
} DNLTHRSTR, *LPDNLTHRSTR;

UINT guCnt;                             // ODS message count


//***************************************************************************
//  $CheckForUpdates
//
//  See if we have the latest version
//***************************************************************************

void CheckForUpdates
    (UBOOL bForceDialog,                // TRUE iff we should force the display of the dialog
     UBOOL bForceDownload)              // TRUE iff we should force a download

{
    HINTERNET hNetOpen    = NULL,       // Net handle
              hNetOpenUrl = NULL;       // ...
    char      szTemp[64];               // Temporary buffer
    WCHAR     wszTemp[64];              // ...
    UINT      uNumBytes;                // # bytes in the file

    // Get the file "nars2000.ver" from http://www.NARS2000.ORG/download/binaries/

    // Stop the timer
    KillTimer (hWndMF, ID_TIMER_UPDCHK);

    // Make the connection
    hNetOpen =
      InternetOpenW (NULL,                                  // Ptr to User Agent
                     INTERNET_OPEN_TYPE_PRECONFIG,          // Access type
                     NULL,                                  // Ptr to proxy name (may be NULL)
                     NULL,                                  // Ptr to proxy bypass (may be NULL)
                     0);                                    // Flags
    if (!hNetOpen)
    {
        FormatNetErrorMessage (L"InterNetOpen");

        goto ERROR_EXIT;
    } // End IF

    // Open the URL
    hNetOpenUrl =
      InternetOpenUrlW (hNetOpen,                           // Net handle
                        L"http://www.nars2000.org"          // Ptr to server name
                        L"/download/binaries/nars2000.ver", // Ptr to /path/filename
                        NULL,                               // Ptr to headers (if any)
                        0,                                  // Size of headers
                        0                                   // Flags
                      | INTERNET_FLAG_NO_CACHE_WRITE        // Does not add the returned entity to the cache
                      | INTERNET_FLAG_RELOAD,               // Forces a download of the requested file, object, or directory
                                                            //   listing from the origin server, not from the cache.
                        0);                                 // Context value
    if (!hNetOpenUrl)
    {
        FormatNetErrorMessage (L"InterNetOpenUrl");

        goto ERROR_EXIT;
    } // End IF

    // Read the file
    if (InternetReadFile (hNetOpenUrl,
                          szTemp,
                          sizeof (szTemp),
                         &uNumBytes))
    {
        UPDATESDLGSTR updatesDlgStr;
        UINT          uFilVer[4],
                      uWebVer[4];

        // Free these resources
        if (hNetOpenUrl)
        {
            // We no longer need this resource
            BoolTest (InternetCloseHandle (hNetOpenUrl)); hNetOpenUrl = NULL;
        } // End IF

        if (hNetOpen)
        {
            // We no longer need this resource
            BoolTest (InternetCloseHandle (hNetOpen)); hNetOpen = NULL;
        } // End IF

        // Ensure properly terminated
        szTemp[uNumBytes] = WC_EOS;

        // Convert the file contents to wide so we can compare it against the file version
        A2W (wszTemp, szTemp, sizeof (szTemp) - uNumBytes);

        // Save data in struc to pass to the dialog box
        updatesDlgStr.lpFilVer       = wszFileVer;
        updatesDlgStr.lpWebVer       = wszTemp;
        updatesDlgStr.bForceDownload = bForceDownload;

        // Split apart the File version information
        sscanfW (wszFileVer,
                 L"%u.%u.%u.%u",
                &uFilVer[0],
                &uFilVer[1],
                &uFilVer[2],
                &uFilVer[3]);
        // Split apart the Web version information
        sscanfW (wszTemp,
                 L"%u.%u.%u.%u",
                &uWebVer[0],
                &uWebVer[1],
                &uWebVer[2],
                &uWebVer[3]);
        // Combine the version info into a single number (note this limits us to four digits per field)
        updatesDlgStr.aplFilVer = (uFilVer[3] + 10000 * (uFilVer[2] + 10000 * (uFilVer[1] + 10000 * uFilVer[0])));
        updatesDlgStr.aplWebVer = (uWebVer[3] + 10000 * (uWebVer[2] + 10000 * (uWebVer[1] + 10000 * uWebVer[0])));

        // If there's a new version, ...
        if (bForceDownload || updatesDlgStr.aplFilVer < updatesDlgStr.aplWebVer)
            bForceDialog = TRUE;
        else
            // Set the update check to today
            SetUpdChkCur ();

        // If we're forcing, or there's a new version,
        //   and not already open...
        if (bForceDialog
         && ghDlgUpdates EQ NULL)
            // Display the dialog with the choices
////////////ghDlgUpdates =              // Set in WM_INITDIALOG
              CreateDialogParamW (_hInstance,
                                  MAKEINTRESOURCEW (IDD_UPDATES),
                                  hWndMF,
                       (DLGPROC) &UpdatesDlgProc,
                        (LPARAM) &updatesDlgStr);
        goto NORMAL_EXIT;
    } else
    {
        FormatSystemErrorMessage (L"InternetReadFile");

        goto ERROR_EXIT;
    } // End IF/ELSE
ERROR_EXIT:
NORMAL_EXIT:
    if (hNetOpenUrl)
    {
        // We no longer need this resource
        BoolTest (InternetCloseHandle (hNetOpenUrl)); hNetOpenUrl = NULL;
    } // End IF

    if (hNetOpen)
    {
        // We no longer need this resource
        BoolTest (InternetCloseHandle (hNetOpen)); hNetOpen = NULL;
    } // End IF

    // Start the timer
    SetUpdChkTimer ();
} // End CheckForUpdates


//***************************************************************************
//  $UpdatesDlgProc
//
//  Modeless Dialog box to handle "Check For Updates"
//***************************************************************************

INT_PTR CALLBACK UpdatesDlgProc
    (HWND   hDlg,       // Window handle
     UINT   message,    // Type of message
     WPARAM wParam,     // Additional information
     LPARAM lParam)     // ...

{
    static WCHAR     wszWebVer[64];                 // Web version #
    static UBOOL     bSuccess;                      // TRUE iff the download succeeded
    static DNLDLGSTR dnlDlgStr;                     // DownloadRun struct
    static DNLTHRSTR dnlThrStr;                     // DownloadInThread parameter
    static UBOOL     bDeleteFile = TRUE;            // TRUE iff we should delete the downloaded file after running it
           UINT      uState;                        // Previous PB state

////LCLODSAPI ("UD: ", hDlg, message, wParam, lParam);
    // Split cases based upon the message #
    switch (message)
    {
        case WM_INITDIALOG:         // hwndFocus = (HWND) wParam; // handle of control to receive focus
                                    // lInitParam = lParam;       // initialization parameter
        {
            WCHAR  wszTemp[512],        // Temp save area
                  *lpwStr;              // Ptr to version action string

            // Save the dialog handle for message loop, etc.
            ghDlgUpdates = hDlg;

            // Check the box
            CheckDlgButton (hDlg, IDC_DNL_XB, bDeleteFile);

#define lpUpdatesDlgStr     ((LPUPDATESDLGSTR) lParam)
            // Clear and initialize DLNDLGSTR
            ZeroMemory (&dnlDlgStr, sizeof (dnlDlgStr));
            dnlDlgStr.hDlgUI        = hDlg;
            dnlDlgStr.hNetOpen      = NULL;
            dnlDlgStr.hNetOpenUrl   = NULL;
            dnlDlgStr.hTempFile     = INVALID_HANDLE_VALUE;
            dnlDlgStr.lpbDeleteFile = &bDeleteFile;

            // Clear and initialize DNLTHRSTR
            ZeroMemory (&dnlThrStr, sizeof (dnlThrStr));
            dnlThrStr.lpDnlDlgStr = &dnlDlgStr;
            dnlThrStr.bThreadDone = TRUE;

            // Save the web version ptr for later use
            lstrcpyW (wszWebVer, lpUpdatesDlgStr->lpWebVer);

            // Compare the two versions
            if (lpUpdatesDlgStr->bForceDownload
             || lpUpdatesDlgStr->aplFilVer < lpUpdatesDlgStr->aplWebVer)    // New version is available for download
            {
                lpwStr = L"The version you are running (%s) is older than the most current version (%s) on the website -- click the \"Download and Run\" button to obtain the latest version.";
                EnableWindow (GetDlgItem (hDlg, IDC_DNLRUN_BN), TRUE);
            } else
            if (lpUpdatesDlgStr->aplFilVer EQ lpUpdatesDlgStr->aplWebVer)   // Same version (nothing to do)
                lpwStr = L"The version you are running (%s) is the same as the most current version (%s) on the website and there is no need to update your version.";
            else                        // Actual version is later (must be running an unreleased version)
                lpwStr = L"The version you are running (%s) is newer than the most current version (%s) on the website -- there is no need to update your version.";

            // Format the text
            wsprintfW (wszTemp,
                       lpwStr,
                       lpUpdatesDlgStr->lpFilVer,
                       lpUpdatesDlgStr->lpWebVer);
#undef  lpUpdatesDlgStr

            // Write out the file & web version strings
            SetDlgItemTextW (hDlg, IDC_VERACTION, wszTemp);

            // Tell the Prgress Bar about the buddy window for the percentage completion
            SendDlgItemMessageW (hDlg, IDC_DNL_PB, PBM_SETBUDDY, (WPARAM) GetDlgItem (hDlg, IDC_DNLPCT_LT), 0);

            CenterWindow (hDlg);    // Reposition the window to the center of the screen
#ifdef DEBUG
            guCnt = 0;
#endif
            return DLG_MSGDEFFOCUS; // Use the focus in wParam, DWLP_MSGRESULT is ignored
        } // End WM_INITDIALOG

        case MYWM_DNL_SUCCESS:      // bSuccess = (UBOOL) wParam
            bSuccess = (UBOOL) wParam;  // Save for later use

            Assert (IsBooleanValue (bSuccess));

            // Tell the dialog about the download status
            SetDlgItemTextW (hDlg,
                             IDC_DNL_LT,
                            (bSuccess ? L"Download complete!" : L"Download cancelled!"));
            // Mark the Progress Bar as Normal or Error
            SetPBMState (hDlg, bSuccess ? PBST_NORMAL : PBST_ERROR);

            // Return dialog result
            DlgMsgDone (hDlg);          // We handled the msg

        case WM_CTLCOLORSTATIC:     // hdc = (HDC) wParam;   // Handle of display context
                                    // hwnd = (HWND) lParam; // Handle of static control
#define hDC     ((HDC)  wParam)
#define hWnd    ((HWND) lParam)

            // We handle IDC_DNL_LT static window only
            if (hWnd EQ GetDlgItem (hDlg, IDC_DNL_LT))
            {
                // Set the text color
                SetTextColor (hDC, bSuccess ? DEF_SCN_BLUE : DEF_SCN_RED);

                // Set the background text color
                SetBkColor (hDC, GetSysColor (COLOR_BTNFACE));

                // Return handle of brush for background, DWLP_MSGRESULT is ignored
                return (APLU3264) (HANDLE_PTR) GetSysColorBrush (COLOR_BTNFACE);
            } // End IF

            break;
#undef  hWnd
#undef  hDC

        case WM_CLOSE:
            // Simulate a click of the Exit button
            SendMessageW (hDlg, WM_COMMAND, GET_WM_COMMAND_MPS (IDOK, NULL, BN_CLICKED));

            // Return dialog result
            DlgMsgDone (hDlg);              // We handled the msg

#define idCtl               GET_WM_COMMAND_ID   (wParam, lParam)
#define cmdCtl              GET_WM_COMMAND_CMD  (wParam, lParam)
#define hwndCtl             GET_WM_COMMAND_HWND (wParam, lParam)
        case WM_COMMAND:
            // If the user pressed one of our buttons, ...
            switch (idCtl)
            {
                case IDC_DNL_XB:                // Delete file
                    // Get the current state
                    bDeleteFile = IsDlgButtonChecked (hDlg, IDC_DNL_XB);

                    // Return dialog result
                    DlgMsgDone (hDlg);          // We handled the msg

                case IDC_DNLPAUSE_BN:           // Pause
////////////////////oprintfW (L"%3d:  IDC_DNLPAUSE_BN\n", guCnt++);

                    dnlThrStr.bThreadActive = !dnlThrStr.bThreadActive;

                    // Change the text on the button
                    SetDlgItemTextW (hDlg,
                                     IDC_DNLPAUSE_BN,
                                     dnlThrStr.bThreadActive ? L"Pause" : L"Resume");
                    // Change the text on the static
                    SetDlgItemTextW (hDlg,
                                      IDC_DNL_LT,
                                      dnlThrStr.bThreadActive ? L"" : L"Download paused");
                    // Mark the Progress Bar state as Normal or Paused
                    SetPBMState (hDlg, dnlThrStr.bThreadActive ? PBST_NORMAL : PBST_PAUSED);

                    // Resume or suspend the thread
                    if (dnlThrStr.bThreadActive)
                    {
                        // Tell the thread to restart
                        dnlThrStr.bThreadSuspend = FALSE;

                        // Set to signalled so the WaitForSingleObject returns
////////////////////////oprintfW (L"%3d:  SetEvent on %p (IDC_DNLPAUSE_BN -- hSuspend)\n", guCnt++, dnlThrStr.hSuspend);
                        SetEvent (dnlThrStr.hSuspend);
                    } else
                    {
                        // Set to non-signalled so the WaitForSingleObject suspends
////////////////////////oprintfW (L"%3d:  ResetEvent on %p (IDC_DNLPAUSE_BN -- hSuspend)\n", guCnt++, dnlThrStr.hSuspend);
                        ResetEvent (dnlThrStr.hSuspend);

                        // Tell the thread to suspend
                        dnlThrStr.bThreadSuspend = TRUE;
                    } // End IF/ELSE

                    // Return dialog result
                    DlgMsgDone (hDlg);          // We handled the msg

                case IDOK:                      // Exit
////////////////////oprintfW (L"%3d:  IDOK\n", guCnt++);

                    // If we're not done, ...
                    if (!dnlThrStr.bThreadDone)
                    {
                        // Mark the Progress Bar state as Paused
                        //   and save the previous state to restore later
                        uState = (UINT) SetPBMState (hDlg, PBST_PAUSED);

                        // Pause until we get an answer
                        //   unless already paused
                        if (uState EQ PBST_NORMAL)
                        {
                            // Set to non-signalled so the WaitForSingleObject suspends
////////////////////////////oprintfW (L"%3d:  ResetEvent on %p (IDOK -- hSuspend)\n", guCnt++, dnlThrStr.hSuspend);
                            ResetEvent (dnlThrStr.hSuspend);

                            // Tell the thread to suspend
                            dnlThrStr.bThreadSuspend = TRUE;
                        } // End IF

                        // Confirm the Cancel and Exit
                        if (IDYES EQ MessageBoxW (hWndMF, L"Download in progress -- cancel it and exit?", lpwszAppName, MB_YESNO | MB_ICONWARNING))
                        {
                            // Mark as to be stopped
                            dnlThrStr.bThreadStop = TRUE;

                            // Tell the thread to restart
                            dnlThrStr.bThreadSuspend = FALSE;

                            // Set to signalled so the WaitForSingleObject returns
////////////////////////////oprintfW (L"%3d:  SetEvent on %p (IDOK -- hSuspend)\n", guCnt++, dnlThrStr.hSuspend);
                            SetEvent (dnlThrStr.hSuspend);

                            // Wait for the thread to terminate
////////////////////////////oprintfW (L"%3d:  WaitForSingleObject start on %p (IDOK -- hThreadWork)\n", guCnt++, dnlThrStr.hThreadWork);
                            WaitForSingleObject (dnlThrStr.hThreadWork, INFINITE);
////////////////////////////oprintfW (L"%3d:  WaitForSingleObject end   on %p (IDOK -- hThreadWork)\n", guCnt++, dnlThrStr.hThreadWork);
                        } else
                        {
                            // Restore the Progress Bar state
                            SetPBMState (hDlg, uState);

                            // Keep on truckin'
                            //   unless previously paused
                            if (uState EQ PBST_NORMAL)
                            {
                                // Tell the thread to restart
                                dnlThrStr.bThreadSuspend = FALSE;

                                // Set to signalled so the WaitForSingleObject returns
/////////////////////////////   oprintfW (L"%3d:  SetEvent on %p (IDOK -- hSuspend)\n", guCnt++, dnlThrStr.hSuspend);
                                SetEvent (dnlThrStr.hSuspend);
                            } // End IF

                            // Return dialog result
                            DlgMsgDone (hDlg);          // We handled the msg
                        } // End IF/ELSE
                    } // End IF

                    // We're done here
                    DestroyWindow (hDlg);

                    // Return dialog result
                    DlgMsgDone (hDlg);          // We handled the msg

                case IDCANCEL:                  // Cancel
////////////////////oprintfW/(L"%3d:  IDCANCEL\n", guCnt++);

                    // If we're not done, ...
                    if (!dnlThrStr.bThreadDone)
                    {
                        // Mark the Progress Bar state as Paused
                        //   and save the previous state to restore later
                        uState = (UINT) SetPBMState (hDlg, PBST_PAUSED);

                        // Pause until we get an answer
                        //   unless already paused
                        if (uState EQ PBST_NORMAL)
                        {
                            // Set to non-signalled so the WaitForSingleObject suspends
////////////////////////////oprintfW (L"%3d:  ResetEvent on %p (IDOK -- hSuspend)\n", guCnt++, dnlThrStr.hSuspend);
                            ResetEvent (dnlThrStr.hSuspend);

                            // Tell the thread to suspend
                            dnlThrStr.bThreadSuspend = TRUE;
                        } // End IF

                        // Confirm the Cancel
                        if (IDYES EQ MessageBoxW (hWndMF, L"Really cancel this download?", lpwszAppName, MB_YESNO | MB_ICONWARNING))
                        {
                            // Mark as to be stopped
                            dnlThrStr.bThreadStop = TRUE;

                            // Tell the thread to restart
                            dnlThrStr.bThreadSuspend = FALSE;

                            // Set to signalled so the WaitForSingleObject returns
////////////////////////////oprintfW (L"%3d:  SetEvent on %p (IDCANCEL -- hSuspend)\n", guCnt++, dnlThrStr.hSuspend);
                            SetEvent (dnlThrStr.hSuspend);

                            // Wait for the thread to terminate
////////////////////////////oprintfW (L"%3d:  WaitForSingleObject start on %p (IDCANCEL -- hThreadWork)\n", guCnt++, dnlThrStr.hThreadWork);
                            WaitForSingleObject (dnlThrStr.hThreadWork, INFINITE);
////////////////////////////oprintfW (L"%3d:  WaitForSingleObject end   on %p (IDCANCEL -- hThreadWork)\n", guCnt++, dnlThrStr.hThreadWork);

                            // Disable the Pause & Cancel buttons
                            EnableWindow (GetDlgItem (hDlg, IDC_DNLPAUSE_BN), FALSE);
                            EnableWindow (GetDlgItem (hDlg, IDCANCEL       ), FALSE);
                        } // End IF
                    } // End IF

                    // Return dialog result
                    DlgMsgDone (hDlg);          // We handled the msg

                case IDC_DNLRUN_BN:             // Download and run
                    // Initialize the success flag
                    bSuccess = FALSE;

                    // Download and run the setup program
                    DownloadRun (wszWebVer, &dnlThrStr);

                    // Return dialog result
                    DlgMsgDone (hDlg);          // We handled the msg

                default:
                    break;
            } // End SWITCH

            break;
#undef  hwndCtl
#undef  cmdCtl
#undef  idCtl

        case WM_DESTROY:
            // Mark as no longer active
            ghDlgUpdates = NULL;

            // Return dialog result
            DlgMsgDone (hDlg);          // We handled the msg
    } // End SWITCH

    //Return dialog result
    DlgMsgPass (hDlg);      // We didn't handle the msg
} // End UpdatesDlgProc


//***************************************************************************
//  $DownloadRun
//
//  Download and run the setup program
//***************************************************************************

void DownloadRun
    (LPWCHAR     lpWebVer,                      // Ptr to web version string
     LPDNLTHRSTR lpDnlThrStr)                   // Ptr to DownloadInThread parameter

{
    WCHAR        wszTemp[128];                  // Temporary buffer
    UINT         uNumBytesRead,                 // # bytes read
                 uRet;                          // Return value
    static WCHAR wszTempPathBuffer[MAX_PATH],   // Buffer for temp path
                 wszTempFileName[MAX_PATH];     // ...             filename

////oprintfW (L"%3d:  -----------------\n", guCnt++);
////oprintfW (L"%3d:  DownloadRun start\n", guCnt++);

    // Clear the previous status text
    SetDlgItemTextW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNL_LT   , L"");
    SetDlgItemTextW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLPCT_LT, L"0%");

    // Enable the Pause & Cancel buttons
    EnableWindow (GetDlgItem (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLPAUSE_BN), TRUE);
    EnableWindow (GetDlgItem (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDCANCEL       ), TRUE);

    // Change the text on the Pause button
    SetDlgItemTextW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLPAUSE_BN, L"Pause");

    // Reset the Progress bar
    SetPBMState (lpDnlThrStr->lpDnlDlgStr->hDlgUI, PBST_NORMAL);
    SendDlgItemMessageW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNL_PB, PBM_SETPOS  , 0          , 0);

    // Initialize the various dialog structs
    lpDnlThrStr->lpDnlDlgStr->uNumBytesTotal = 0;
    lpDnlThrStr->lpDnlDlgStr->lpwszTempPathBuffer = wszTempPathBuffer;
    lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName   = wszTempFileName;
    lpDnlThrStr->bThreadStop                      = FALSE;
    wszTempPathBuffer[0] = WC_EOS;
    wszTempFileName  [0] = WC_EOS;

    // Make the connection
    lpDnlThrStr->lpDnlDlgStr->hNetOpen =
      InternetOpenW (NULL,                                  // Ptr to User Agent
                     INTERNET_OPEN_TYPE_PRECONFIG,          // Access type
                     NULL,                                  // Ptr to proxy name (may be NULL)
                     NULL,                                  // Ptr to proxy bypass (may be NULL)
                     0);                                    // Flags
    if (!lpDnlThrStr->lpDnlDlgStr->hNetOpen)
    {
        FormatNetErrorMessage (L"InterNetOpen");

        goto ERROR_EXIT;
    } // End IF

    // Fill in the URL
    wsprintfW (wszTemp,
#ifdef _WIN64
               L"http://www.nars2000.org/download/binaries/w64/" WS_APPNAME L"-%s.exe",
#else
               L"http://www.nars2000.org/download/binaries/w32/" WS_APPNAME L"-%s.exe",
#endif
               lpWebVer);
    // Open the URL
    lpDnlThrStr->lpDnlDlgStr->hNetOpenUrl =
      InternetOpenUrlW (lpDnlThrStr->lpDnlDlgStr->hNetOpen, // Net handle
                        wszTemp,                            // Ptr to URL
                        NULL,                               // Ptr to headers (if any)
                        0,                                  // Size of headers
                        0                                   // Flags
                      | INTERNET_FLAG_NO_CACHE_WRITE        // Does not add the returned entity to the cache
                      | INTERNET_FLAG_RELOAD,               // Forces a download of the requested file, object, or directory
                                                            //   listing from the origin server, not from the cache.
                        0);                                 // Context value
    if (!lpDnlThrStr->lpDnlDlgStr->hNetOpenUrl)
    {
        FormatNetErrorMessage (L"InterNetOpenUrl");

        goto ERROR_EXIT;
    } // End IF

    // Initialize the buffer length
    uNumBytesRead = sizeof (lpDnlThrStr->lpDnlDlgStr->uNumBytesOrig);

    // Query the content length
    if (!HttpQueryInfo (lpDnlThrStr->lpDnlDlgStr->hNetOpenUrl,      // Net handle
                        0                                           // Flags
                      | HTTP_QUERY_CONTENT_LENGTH
                      | HTTP_QUERY_FLAG_NUMBER
                        ,
                       &lpDnlThrStr->lpDnlDlgStr->uNumBytesOrig,    // Ptr to output buffer
                       &uNumBytesRead,                              // Size of buffer
                        0))
    {
        FormatNetErrorMessage (L"HttpQueryInfo");

        goto ERROR_EXIT;
    } // End IF

    // Get the temp path env string (no guarantee it's a valid path)
    uRet =
      GetTempPathW (countof (wszTempPathBuffer), // Length of the buffer
                    wszTempPathBuffer);          // Buffer for path
    if (uRet > MAX_PATH
     || uRet EQ 0)
    {
        FormatSystemErrorMessage (L"GetTempPath");

        goto ERROR_EXIT;
    } // End IF

    // Generate a temporary file name
    uRet =
      GetTempFileNameW (wszTempPathBuffer,      // Directory for tmp files
                        L"NARS2000",            // Temp file name prefix
                        0,                      // Create unique name
                        wszTempFileName);       // Buffer for name
    // Check for error
    if (uRet EQ 0)
    {
        FormatSystemErrorMessage (L"GetTempFileName");

        goto ERROR_EXIT;
    } // End IF

    // Create the output file
    lpDnlThrStr->lpDnlDlgStr->hTempFile =
      CreateFileW (wszTempFileName,             // File name
                   GENERIC_WRITE,               // Open for write
                   0,                           // Do not share
                   NULL,                        // Default security
                   CREATE_ALWAYS,               // Overwrite existing
                   FILE_ATTRIBUTE_NORMAL,       // Normal file
                   NULL);                       // No template
    if (lpDnlThrStr->lpDnlDlgStr->hTempFile EQ INVALID_HANDLE_VALUE)
    {
        FormatSystemErrorMessage (L"CreateFile");

        goto ERROR_EXIT;
    } // End IF

////oprintfW (L"%3d:  CreateFile on %p (DownloadRun -- hTempFile)\n", guCnt++, lpDnlThrStr->lpDnlDlgStr->hTempFile);

    // Divide the # bytes in the file by an amount that will show some progress
    lpDnlThrStr->lpDnlDlgStr->uNumBytesAlloc = lpDnlThrStr->lpDnlDlgStr->uNumBytesOrig / 20;

    // Allocate global memory to read the file
    while (lpDnlThrStr->lpDnlDlgStr->uNumBytesAlloc
        && (lpDnlThrStr->lpDnlDlgStr->lpMem = MyGlobalAlloc (GPTR, lpDnlThrStr->lpDnlDlgStr->uNumBytesAlloc)) EQ NULL)
        lpDnlThrStr->lpDnlDlgStr->uNumBytesAlloc /= 2;
    if (lpDnlThrStr->lpDnlDlgStr->uNumBytesAlloc EQ 0)
        goto WSFULL_EXIT;

    // Set the range for the Progress Bar
    SendDlgItemMessageW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNL_PB, PBM_SETRANGE32, 0, lpDnlThrStr->lpDnlDlgStr->uNumBytesOrig);

    // Create a suspend event
    lpDnlThrStr->hSuspend =
      CreateEventW (NULL,                       // Security attributes
                    TRUE,                       // TRUE iff manual-reset event
                    FALSE,                      // TRUE iff initial state is signalled
                    WS_APPNAME L"-suspend");    // Event name
    if (!lpDnlThrStr->hSuspend)
    {
        FormatSystemErrorMessage (L"CreateEvent");

        goto ERROR_EXIT;
    } // End IF

////oprintfW (L"%3d:  CreateEvent on %p (DownloadRun -- hSuspend)\n", guCnt++, lpDnlThrStr->hSuspend);

    // Create a worker thread
    lpDnlThrStr->hThreadWork =
      CreateThread (NULL,                       // No security attrs
                    0,                          // Use default stack size
                   &DownloadInThread,           // Starting routine
                    lpDnlThrStr,                // Param to thread func
                    CREATE_SUSPENDED,           // Creation flag
                   &lpDnlThrStr->dwThreadId);   // Returns thread id
    if (!lpDnlThrStr->hThreadWork)
    {
        FormatSystemErrorMessage (L"CreateThread");

        goto ERROR_EXIT;
    } // End IF

////oprintfW (L"%3d:  CreateThread on %p (DownloadRun -- hThreadWork)\n", guCnt++, lpDnlThrStr->hThreadWork);

    // Tell W to callback when this thread terminates
    RegisterWaitForSingleObject (&lpDnlThrStr->WaitHandle,  // Return wait handle
                                  lpDnlThrStr->hThreadWork, // Handle to wait on
                                 &WaitForDownload,          // Callback function
                                  lpDnlThrStr,              // Callback function parameter
                                  INFINITE,                 // Wait time in milliseconds
                                  WT_EXECUTEONLYONCE);      // Options
    // Start 'er up
    lpDnlThrStr->bThreadActive = TRUE;
    lpDnlThrStr->bThreadDone   = FALSE;
    ResumeThread (lpDnlThrStr->hThreadWork);

    // Disable the "Download and Run" button
    EnableWindow (GetDlgItem (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLRUN_BN), FALSE);

    goto NORMAL_EXIT;

WSFULL_EXIT:
    MessageBoxW (hWndMF, L"GlobalAlloc failed with WS FULL", lpwszAppName, MB_OK | MB_ICONERROR);

    goto ERROR_EXIT;

ERROR_EXIT:
    // Set the status text
    SetDlgItemTextW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNL_LT, L"Download not initiated");

    // Reset the Progress bar
    SetPBMState (lpDnlThrStr->lpDnlDlgStr->hDlgUI, PBST_NORMAL);
    SendDlgItemMessageW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNL_PB, PBM_SETPOS  , 0          , 0);

    // Restore the text on the Pause button
    SetDlgItemTextW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLPAUSE_BN, L"Pause");

    // Disable the Pause & Cancel buttons
    EnableWindow (GetDlgItem (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLPAUSE_BN), FALSE);
    EnableWindow (GetDlgItem (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDCANCEL       ), FALSE);

    // Free and/or close internet resources
    FreeAndClose (lpDnlThrStr, TRUE);
NORMAL_EXIT:
////oprintfW (L"%3d:  DownloadRun end\n", guCnt++);

    return;
} // End DownloadRun


//***************************************************************************
//  $WaitForDownload
//
//  Wait callback for DownloadInThread
//***************************************************************************

void CALLBACK WaitForDownload
    (LPDNLTHRSTR lpDnlThrStr,           // Thread data
     BOOLEAN     TimerOrWaitFired)      // Reason

{
    UBOOL             bSuccess;         // TRUE iff the file was successfully downloaded
    UINT              uRet;             // Return value from WaitForSingleObject
    SHELLEXECUTEINFOW shellExecuteInfo; // Used by ShellExecuteEx
////OVERLAPPED        overLapped = {0}; // Used by CancelIoEx

////oprintfW (L"%3d:  WaitForDownload start\n", guCnt++);

    Assert (!TimerOrWaitFired);

    // Cancel any pending I/O
    if (lpDnlThrStr->lpDnlDlgStr->hTempFile NE INVALID_HANDLE_VALUE)
    {
////////overLapped.hEvent = lpDnlThrStr->hThreadWork;
////////BoolTest (CancelIoEx (lpDnlThrStr->lpDnlDlgStr->hTempFile, &overLapped));
////////oprintfW (L"%3d:  CancelIo on %p (WaitForDownload -- hTempFile)\n", guCnt++, lpDnlThrStr->lpDnlDlgStr->hTempFile);
        BoolTest (CancelIo   (lpDnlThrStr->lpDnlDlgStr->hTempFile             ));
    } // End IF

    // Free and/or close internet resources
    FreeAndClose (lpDnlThrStr, FALSE);

    // Get the thread's exit code
    GetExitCodeThread (lpDnlThrStr->hThreadWork, &bSuccess); // lpDnlThrStr->hThreadWork = NULL;
    lpDnlThrStr->bThreadDone = TRUE;
////oprintfW (L"%3d:  GetExitCodeThread %p (%d) (WaitForDownload -- hThreadWork\n", guCnt++, lpDnlThrStr->hThreadWork, bSuccess);

    Assert (IsBooleanValue (bSuccess));

    // Cancel the wait operation
    if (lpDnlThrStr->WaitHandle)
    {
////////oprintfW (L"%3d:  UnregisterWait on %p (WaitForDownload -- WaitHandle)\n", guCnt++, lpDnlThrStr->WaitHandle);
        uRet = UnregisterWait (lpDnlThrStr->WaitHandle); lpDnlThrStr->WaitHandle = NULL;
        if (!uRet)
        {
            uRet = GetLastError ();

            // If it's not I/O pending, ...
            if (uRet NE ERROR_IO_PENDING)
            {
                FormatSystemErrorMessage (L"UnregisterWait");

                goto ERROR_EXIT;
            } else
            {
////////////////oprintfW (L"%3d:  ERROR_IO_PENDING on %p (WaitForDownload -- WaitHandle)\n", guCnt++, lpDnlThrStr->WaitHandle);
            } // End IF/ELSE
        } // End IF
    } // End IF

    // Tell the "Check for Updates" dialog about this flag
    SendMessageW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, MYWM_DNL_SUCCESS, bSuccess, 0);

    // Restore the text on the Pause button
    SetDlgItemTextW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLPAUSE_BN, L"Pause");

    // Disable the Pause & Cancel buttons
    EnableWindow (GetDlgItem (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLPAUSE_BN), FALSE);
    EnableWindow (GetDlgItem (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDCANCEL       ), FALSE);

    // Enable the "Download and Run" button
    EnableWindow (GetDlgItem (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNLRUN_BN), TRUE);

    if (bSuccess)
    {
        // Copy the temp file name and rename it to an executable
        lstrcpyW ( lpDnlThrStr->lpDnlDlgStr->lpwszTempPathBuffer, lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName);
        lstrcpyW (&lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName[lstrlenW (lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName) - 4], L".exe");

        // Rename the file to an .exe so we can run it vis ShellExecuteEx
        if (!MoveFileExW (lpDnlThrStr->lpDnlDlgStr->lpwszTempPathBuffer,    // Source filename
                          lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName,      // Destin ...
                          0                                                 // Flags
                        | MOVEFILE_REPLACE_EXISTING
                        | MOVEFILE_COPY_ALLOWED))
        {
            FormatSystemErrorMessage (L"MoveFileEx");

            goto ERROR_EXIT;
        } // End IF

        // In case we need COM, ...
        CoInitializeEx (NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        // Fill in the SHELLEXECUTEINFO struct
        ZeroMemory (&shellExecuteInfo, sizeof (shellExecuteInfo));
        shellExecuteInfo.cbSize       = sizeof (shellExecuteInfo);                      // Size
        shellExecuteInfo.fMask        = 0                                               // Mask
                                      | SEE_MASK_NOCLOSEPROCESS
                                      | SEE_MASK_NOASYNC
                                        ;
        shellExecuteInfo.hwnd         = lpDnlThrStr->lpDnlDlgStr->hDlgUI;               // Parent window handle
        shellExecuteInfo.lpVerb       = NULL;                                           // Operation (NULL = use default verb)
        shellExecuteInfo.lpFile       = lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName;    // The file to "open"
////////shellExecuteInfo.lpParameters = NULL;                                           // Unused (already zero from ZeroMemory)
////////shellExecuteInfo.lpDirectory  = NULL;                                           // Unused (already zero from ZeroMemory)
        shellExecuteInfo.nShow        = SW_SHOWNORMAL;                                  // Show command
////////shellExecuteInfo.hInstApp     = NULL;                                           // Output from ShellExecuteEx
////////shellExecuteInfo.lpIDList     = NULL;                                           // Unused (already zero from ZeroMemory)
////////shellExecuteInfo.lpClass      = NULL;                                           // Unused (already zero from ZeroMemory)
////////shellExecuteInfo.hKeyClass    = NULL;                                           // Unused (already zero from ZeroMemory)
////////shellExecuteInfo.dwHotKey     = 0;                                              // Unused (already zero from ZeroMemory)
////////shellExecuteInfo.hIcon        = NULL;                                           // Unused (already zero from ZeroMemory)
////////shellExecuteInfo.hMonitor     = NULL;                                           // Unused (already zero from ZeroMemory)
////////shellExecuteInfo.hProcess     = NULL;                                           // Output from ShellExecuteEx

        // Run the downloaded program
        ShellExecuteExW (&shellExecuteInfo);

        // If we launched a process, ...
        if (shellExecuteInfo.hProcess)
        {
            // Wait for the process to terminate
////////////oprintfW (L"%3d:  WaitForSingleObject start on %p (WaitForDownload -- hProcess)\n", guCnt++, shellExecuteInfo.hProcess);
            uRet = WaitForSingleObject (shellExecuteInfo.hProcess, INFINITE);
////////////oprintfW (L"%3d:  WaitForSingleObject end   on %p (WaitForDownload -- hProcess)\n", guCnt++, shellExecuteInfo.hProcess);

            // We no longer need this resource
            BoolTest (0 NE CloseHandle (shellExecuteInfo.hProcess)); shellExecuteInfo.hProcess = NULL;
        } else
        {
            FormatSystemErrorMessage (L"ShellExecuteEx");

            goto ERROR_EXIT;
        } // End IF

        // If we should delete the file after running it, ...
        if (*lpDnlThrStr->lpDnlDlgStr->lpbDeleteFile)
        {
////////////oprintfW (L"%3d:  DeleteFile on %s (WaitForDownload -- lpwszTempFileName)\n", guCnt++, lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName);

            // We no longer need this storage
            DeleteFileW (lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName);
        } // End IF

        // Set the update check to today
        SetUpdChkCur ();

        goto NORMAL_EXIT;
    } // End IF
ERROR_EXIT:
    // Mark the Progress bar state as Error
    SetPBMState (lpDnlThrStr->lpDnlDlgStr->hDlgUI, PBST_ERROR);
NORMAL_EXIT:
////oprintfW (L"%3d:  WaitForDownload exit\n", guCnt++);
////oprintfW (L"%3d:  --------------------\n", guCnt++);

    return;
} // End WaitForDownload


//***************************************************************************
//  $DownloadInThread
//
//  Download worker thread
//***************************************************************************

DWORD WINAPI DownloadInThread
    (LPDNLTHRSTR lpDnlThrStr)           // Thread data

{
    UINT        uNumBytesRead;          // # bytes read by InternetReadFile
    UBOOL       bRet;                   // TRUE iff all I/O has completed

////oprintfW (L"%3d:  DownloadInThread start\n", guCnt++);

    while (!lpDnlThrStr->bThreadStop)
    {
        // Check for suspend
        if (lpDnlThrStr->bThreadSuspend)
        {
////////////oprintfW (L"%3d:  WaitForSingleObject start on %p (DownloadInThread -- hSuspend)\n", guCnt++, lpDnlThrStr->hSuspend);
            WaitForSingleObject (lpDnlThrStr->hSuspend, INFINITE);
////////////oprintfW (L"%3d:  WaitForSingleObject end   on %p (DownloadInThread -- hSuspend)\n", guCnt++, lpDnlThrStr->hSuspend);

            if (lpDnlThrStr->bThreadStop)
                break;
        } // End IF

        // Read the file contents in a loop
////////oprintfW (L"%3d:  InternetReadFile on %p (DownloadInThread -- hNetOpenUrl)\n", guCnt++, lpDnlThrStr->lpDnlDlgStr->hNetOpenUrl);
        if (!InternetReadFile (lpDnlThrStr->lpDnlDlgStr->hNetOpenUrl,
                               lpDnlThrStr->lpDnlDlgStr->lpMem,
                               lpDnlThrStr->lpDnlDlgStr->uNumBytesAlloc,
                              &uNumBytesRead))
        {
            FormatNetErrorMessage (L"InternetReadFile");

            break;
        } // End IF

        // If we read something, ...
        if (uNumBytesRead)
        {
            // Check for suspend
            if (lpDnlThrStr->bThreadSuspend)
            {
////////////////oprintfW (L"%3d:  WaitForSingleObject start on %p (DownloadInThread -- hSuspend)\n", guCnt++, lpDnlThrStr->hSuspend);
                WaitForSingleObject (lpDnlThrStr->hSuspend, INFINITE);
////////////////oprintfW (L"%3d:  WaitForSingleObject end   on %p (DownloadInThread -- hSuspend)\n", guCnt++, lpDnlThrStr->hSuspend);

                if (lpDnlThrStr->bThreadStop)
                    break;
            } // End IF

            // Accumulate the # bytes read
            lpDnlThrStr->lpDnlDlgStr->uNumBytesTotal += uNumBytesRead;

            // Update the Progress Bar
            SendDlgItemMessageW (lpDnlThrStr->lpDnlDlgStr->hDlgUI, IDC_DNL_PB, PBM_SETPOS, lpDnlThrStr->lpDnlDlgStr->uNumBytesTotal, 0);

            // Write it out to the file
////////////oprintfW (L"%3d:  WriteFile on %p (DownloadInThread -- hTempFile)\n", guCnt++, lpDnlThrStr->lpDnlDlgStr->hTempFile);
            if (!WriteFile (lpDnlThrStr->lpDnlDlgStr->hTempFile,
                            lpDnlThrStr->lpDnlDlgStr->lpMem,
                            uNumBytesRead,
                           &uNumBytesRead,
                            NULL))
            {
                FormatSystemErrorMessage (L"WriteFile");

                break;
            } // End IF
        } else
            break;
    } // End WHILE

    // Reset the flag
    lpDnlThrStr->bThreadStop = FALSE;

    // Did we get the entire file?
    bRet = (lpDnlThrStr->lpDnlDlgStr->uNumBytesTotal EQ lpDnlThrStr->lpDnlDlgStr->uNumBytesOrig);
    if (!bRet)
    {
////////oprintfW (L"%3d:  CancelIo on %p (DownloadInThread -- hTempFile)\n", guCnt++, lpDnlThrStr->lpDnlDlgStr->hTempFile);
        BoolTest (CancelIo (lpDnlThrStr->lpDnlDlgStr->hTempFile))
    } // End IF

////oprintfW (L"%3d:  DownloadInThread end\n", guCnt++);
////oprintfW (L"%3d: ExitThread %p (%d) (DownloadInThread -- hThreadWork)\n", guCnt++, lpDnlThrStr->hThreadWork, bRet);

    ExitThread (bRet);

    return bRet;                // Keep the compiler happy
} // End DownloadInThread


//***************************************************************************
//  $SetUpdChkCur
//
//  Set the update check date to today
//***************************************************************************

void SetUpdChkCur
    (void)

{
    SYSTEMTIME stCur;

    // Get the current system date
    GetSystemDate (&stCur);

    // Mark as having checked today
    gstUpdChk.wYear  = stCur.wYear;
    gstUpdChk.wMonth = stCur.wMonth;
    gstUpdChk.wDay   = stCur.wDay;
} // End SetUpdChkCur


//***************************************************************************
//  $SetUpdChkTimer
//
//  Set a timer event for the next update check
//***************************************************************************

void SetUpdChkTimer
    (void)

{
    APLUINT aplElap;

    // If the frequency is not never, ...
    if (guUpdFrq NE ENUM_UPDFRQ_NEVER)
    {
        // Convert the elapsed time from 100ns to 1ms
        aplElap = updFrq[guUpdFrq].aplElap / (1000*10);

        // If the elapsed time is within a UINT (maximum size for SetTimer), ...
        if (aplElap EQ (UINT) aplElap)
            // Set a timer event to send a WM_TIMER message
            SetTimer (hWndMF, ID_TIMER_UPDCHK, (UINT) aplElap, NULL);
    } // End IF
} // End SetUpdChkTimer


//***************************************************************************
//  $FormatNetErrorMessage
//
//  Format a net error message
//***************************************************************************

void FormatNetErrorMessage
    (LPWCHAR lpwszFcnName)

{
    UINT uRet;
    WCHAR wszTemp1[512],
          wszTemp2[512];
    DWORD dwErr, dwSize = countof (wszTemp1);

    uRet = GetLastError ();

    // Get the net error message
    InternetGetLastResponseInfoW (&dwErr, wszTemp1, &dwSize);

    // Format the system error message
    FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                    NULL,                       // Pointer to  message source
                    uRet,                       // Requested message identifier
                    0,                          // Language identifier for requested message
                    wszTemp2,                   // Pointer to message buffer
                    countof (wszTemp2),         // Maximum size of message buffer
                    NULL);                      // Address of array of message inserts
    // Copy the leading text
    lstrcpyW (lpwszGlbTemp, lpwszFcnName);

    // If there's a net error message, ...
    if (dwErr)
        // Format the entire error message
        wsprintfW (&lpwszGlbTemp[lstrlenW (lpwszGlbTemp)],
                    L" failed with error code %u (%08X) LastResponseInfo %u (%08X) %s\n(%s)",
                    uRet, uRet,
                    dwErr, dwErr,
                    wszTemp1,
                    wszTemp2);
    else
        // Format the entire error message
        wsprintfW (&lpwszGlbTemp[lstrlenW (lpwszGlbTemp)],
                    L" failed with error code %u (%08X)\n(%s)",
                    uRet, uRet,
                    wszTemp2);
    // Display it
    MessageBoxW (hWndMF, lpwszGlbTemp, lpwszAppName, MB_OK | MB_ICONERROR);
} // End FormatNetErrorMessage


//***************************************************************************
//  $FormatSystemErrorMessage
//
//  Format a system error message
//***************************************************************************

void FormatSystemErrorMessage
    (LPWCHAR lpwszFcnName)

{
    UINT uRet;
    WCHAR wszTemp[512];

    uRet = GetLastError ();

    // Format the system error message
    FormatMessageW (FORMAT_MESSAGE_FROM_SYSTEM, // Source and processing options
                    NULL,                       // Pointer to  message source
                    uRet,                       // Requested message identifier
                    0,                          // Language identifier for requested message
                    wszTemp,                    // Pointer to message buffer
                    countof (wszTemp),          // Maximum size of message buffer
                    NULL);                      // Address of array of message inserts
    // Copy the leading text
    lstrcpyW (lpwszGlbTemp, lpwszFcnName);

    // Format the entire error message
    wsprintfW (&lpwszGlbTemp[lstrlenW (lpwszGlbTemp)],
                L" failed with error code %u (%08X)\n(%s)",
                uRet, uRet,
                wszTemp);
    MessageBoxW (hWndMF, lpwszGlbTemp, lpwszAppName, MB_OK | MB_ICONERROR);
} // End FormatSystemErrorMessage


//***************************************************************************
//  $FreeAndClose
//
//  Free and close internat resources and handles
//***************************************************************************

void FreeAndClose
    (LPDNLTHRSTR lpDnlThrStr,           // Ptr to DNLTHRSTR
     UBOOL       bDeleteFile)           // TRUE iff we should delete the temp file

{
////oprintfW (L"%3d:  FreeAndClose start\n", guCnt++);

    if (lpDnlThrStr->hSuspend)
    {
        // We no longer need this resource
////////oprintfW (L"%3d:  CloseHandle on %p (FreeAndClose -- hSuspend)\n", guCnt++, lpDnlThrStr->hSuspend);
        BoolTest (0 NE CloseHandle (lpDnlThrStr->hSuspend)); lpDnlThrStr->hSuspend = NULL;
    } // End IF

    if (lpDnlThrStr->lpDnlDlgStr->lpMem)
    {
        // We no longer need this storage
        MyGlobalFree (lpDnlThrStr->lpDnlDlgStr->lpMem); lpDnlThrStr->lpDnlDlgStr->lpMem = NULL;
    } // End IF

    if (lpDnlThrStr->lpDnlDlgStr->hTempFile NE INVALID_HANDLE_VALUE)
    {
        // We no longer need this resource
////////oprintfW (L"%3d:  CloseHandle on %p (FreeAndClose -- hTempFile)\n", guCnt++, lpDnlThrStr->lpDnlDlgStr->hTempFile);
        BoolTest (0 NE CloseHandle (lpDnlThrStr->lpDnlDlgStr->hTempFile)); lpDnlThrStr->lpDnlDlgStr->hTempFile = INVALID_HANDLE_VALUE;

        // If we should delete the temp file, ...
        if (bDeleteFile)
        {
////////////oprintfW (L"%3d:  DeleteFile on %s (FreeAndClose -- lpwszTempFileName)\n", guCnt++, lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName);

            // We no longer need this storage
            BoolTest (DeleteFileW (lpDnlThrStr->lpDnlDlgStr->lpwszTempFileName))
        } // End IF
    } // End IF

    if (lpDnlThrStr->lpDnlDlgStr->hNetOpenUrl)
    {
        // We no longer need this resource
        BoolTest (InternetCloseHandle (lpDnlThrStr->lpDnlDlgStr->hNetOpenUrl)); lpDnlThrStr->lpDnlDlgStr->hNetOpenUrl = NULL;
    } // End IF

    if (lpDnlThrStr->lpDnlDlgStr->hNetOpen)
    {
        // We no longer need this resource
        BoolTest (InternetCloseHandle (lpDnlThrStr->lpDnlDlgStr->hNetOpen)); lpDnlThrStr->lpDnlDlgStr->hNetOpen = NULL;
    } // End IF

////oprintfW (L"%3d:  FreeAndClose end\n", guCnt++);
} // End FreeAndClose


//***************************************************************************
//  End of File: updates.c
//***************************************************************************
