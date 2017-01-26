//***************************************************************************
//  NARS2000 -- Resource Constants
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2016 Sudley Place Software

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


// Menu IDs
#define IDR_MFMENU                                20
#define IDR_SMMENU                                21
#define IDR_FEMENU                                22
#define IDR_VEMENU                                23
#define IDR_MEMENU                                24

// Accelerator IDs
#define IDR_ACCEL                                 30

// Bitmap IDs
#define IDB_WORKSPACE                             41
#define IDB_EDIT                                  42
#define IDB_OBJECTS                               43

// Icon IDs
#define IDI_MF_LARGE                             102
#define IDI_MF_SMALL                             103
#define IDI_SM_LARGE                             104
#define IDI_SM_SMALL                             105
#define IDI_DB_LARGE                             106
#define IDI_DB_SMALL                             107
#define IDI_FE_LARGE                             108
#define IDI_FE_SMALL                             109
#define IDI_ME_LARGE                             110
#define IDI_ME_SMALL                             111
#define IDI_VE_LARGE                             112
#define IDI_VE_SMALL                             113
#define IDI_CC_LARGE                             114
#define IDI_CC_SMALL                             115
#define IDI_PM_LARGE                             116
#define IDI_PM_SMALL                             117
#define IDI_WC_LARGE                             118
#define IDI_WC_SMALL                             119
#define IDI_ABOUT                                120
#define IDI_CLOSE                                121
#define IDI_NARS                                 122
#define IDI_CUSTOM                               123

#define IDD_ABOUT                                130
#define IDD_UPDATES                              131
#define IDD_CUSTOMIZE                            132
#define IDD_WEBCOLORS                            133
#define IDD_NEWKEYB                              134

// The following define must be the first number
//   in the IDD_xxx sequence
#define IDD_PROPPAGE_START                       150

// The following defines must be in sequence
#define IDD_PROPPAGE_CLEARWS_VALUES              150
#define IDD_PROPPAGE_DIRS                        151
#define IDD_PROPPAGE_FONTS                       152
#define IDD_PROPPAGE_KEYBS                       153
#define IDD_PROPPAGE_RANGE_LIMITS                154
#define IDD_PROPPAGE_SYNTAX_COLORING             155
#define IDD_PROPPAGE_SYSTEM_VAR_RESET            156
#define IDD_PROPPAGE_TAB_COLORS                         158 // Not implemented as yet
#define IDD_PROPPAGE_USER_PREFS                  157

// Message IDs
#define IDM_EXIT                                 102
#define IDM_STATUSBAR                            103

#define IDM_CASCADE                              300
#define IDM_TILE_HORZ                            301
#define IDM_TILE_VERT                            302
#define IDM_ARRANGE_ICONS                        303
#define IDM_CHILDWINDOW                          304

#define IDM_HELP_CONTENTS                        400
#define IDM_UPDATES                              401
#define IDM_TUTORIALS                            402
#define IDM_DOCUMENTATION                        403
#define IDM_ABOUT                                404

#define IDM_NEW_WS                               500
#define IDM_DUP_WS                               501
#define IDM_SAVECLOSE_WS                         502
#define IDM_CLOSE_WS                             503
#define IDM_LOAD_WS                              504
#define IDM_XLOAD_WS                             505
#define IDM_COPY_WS                              506
#define IDM_PCOPY_WS                             507
#define IDM_SAVE_WS                              508
#define IDM_SAVE_AS_WS                           509
#define IDM_DROP_WS                              510
#define IDM_PRINT_WS                             511

// The following IDs must be consecutive
// These entries must be in the same order as the <aRebarBands> and
//   <IDR_SMMENU>/<IDR_FEMENU> entries
#define IDM_TB_FIRST                             IDM_TB_WS
#define IDM_TB_WS                                600
#define IDM_TB_ED                                601
#define IDM_TB_FW                                602
#define IDM_TB_OW                                603
#define IDM_TB_LW                                604
#define IDM_TB_LAST                              IDM_TB_LW

#define IDM_UNDO                                 700
#define IDM_REDO                                 701
#define IDM_CUT                                  702
#define IDM_DELETE                               703
#define IDM_DELETEALL                            704
#define IDM_SELECTALL                            705

#define IDM_COPY                                 710
#define IDM_COPY_APLWIN                          711
#define IDM_COPY_APL2                            712
#define IDM_COPY_DYALOG                          713
#define IDM_COPY_ISO                             714
#define IDM_COPY_PC3270                          715
#define IDM_COPY_BRACES                          716

#define IDM_PASTE                                720
#define IDM_PASTE_APLWIN                         721
#define IDM_PASTE_APL2                           722
#define IDM_PASTE_DYALOG                         723
#define IDM_PASTE_ISO                            724
#define IDM_PASTE_PC3270                         725
#define IDM_PASTE_BRACES                         726

#define IDM_LOCALIZE                             730
#define IDM_UNLOCALIZE                           731

#define IDM_NEW_FCN                              740
#define IDM_CLOSE_FCN                            741
#define IDM_SAVE_FCN                             742
#define IDM_SAVE_AS_FCN                          743
#define IDM_SAVECLOSE_FCN                        744
#define IDM_TOGGLE_LNS_FCN                       745

#define IDM_CUSTOMIZE                            750

// The following IDs are used as a base of length <uNumRecentFiles>
#define IDM_LOAD_BASE                          11000
#define IDM_XLOAD_BASE                         12000
#define IDM_COPY_BASE                          13000

// Control IDs
#define IDC_VERSION                              800
#define IDC_LINK                                 801
#define IDC_WEBVER                               802
#define IDC_VERACTION                            803
#define IDC_FILEVER                              804
#define IDC_LOADEDFROM                           805
#define IDC_VERSION2                             806
#define IDC_ABOUT_COPY                           807
#define IDC_ABOUT_NOTE                           808

#define IDC_APPLY                                810
#define IDC_CUST_LB                              811
#define IDC_GROUPBOX                             812

// The _EC and matching _LEN IDs must be consecutive
#define IDC_CLEARWS_ALX_EC                       820
#define IDC_CLEARWS_ALX_LEN                      821
#define IDC_CLEARWS_CT_EC                        822
#define IDC_CLEARWS_DT_CB                        823
#define IDC_CLEARWS_ELX_EC                       824
#define IDC_CLEARWS_ELX_LEN                      825
#define IDC_CLEARWS_FC_EC                        826
#define IDC_CLEARWS_FC_LEN                       827
#define IDC_CLEARWS_FEATURE_CB1                  828
#define IDC_CLEARWS_FEATURE_CB2                  829
#define IDC_CLEARWS_FPC_EC                       830
#define IDC_CLEARWS_FPC_UD                       831
#define IDC_CLEARWS_IC_CB1                       832
#define IDC_CLEARWS_IC_CB2                       833
#define IDC_CLEARWS_IO_EC                        834
#define IDC_CLEARWS_IO_UD                        835
#define IDC_CLEARWS_LX_EC                        836
#define IDC_CLEARWS_LX_LEN                       837
#define IDC_CLEARWS_MF_CB                        838
#define IDC_CLEARWS_PP_EC                        839
#define IDC_CLEARWS_PP_UD                        840
#define IDC_CLEARWS_PW_EC                        841
#define IDC_CLEARWS_PW_UD                        842
#define IDC_CLEARWS_RL_EC                        843

#define IDC_FONTS1                               840
#define IDC_FONTS2                               841
#define IDC_FONTS3                               842
#define IDC_FONTS4                               843
#define IDC_FONTS5                               844
#define IDC_FONTS6                               845
#define IDC_FONTS7                               846
#define IDC_FONTS8                               847
#define IDC_FONTS_LAST                           IDC_FONTS8

#define IDC_FONTS_RADIO1A                        850
#define IDC_FONTS_RADIO2A                        851
#define IDC_FONTS_RADIO2B                        852
#define IDC_FONTS_RADIO3A                        853
#define IDC_FONTS_RADIO3B                        854
#define IDC_FONTS_RADIO3C                        855
#define IDC_FONTS_RADIO4A                        856
#define IDC_FONTS_RADIO4B                        857
#define IDC_FONTS_RADIO4C                        858
#define IDC_FONTS_RADIO4D                        859
#define IDC_FONTS_RADIO5A                        860
#define IDC_FONTS_RADIO5B                        861
#define IDC_FONTS_RADIO5C                        862
#define IDC_FONTS_RADIO5D                        863
#define IDC_FONTS_RADIO5E                        864
#define IDC_FONTS_RADIO6A                        865
#define IDC_FONTS_RADIO6B                        866
#define IDC_FONTS_RADIO6C                        867
#define IDC_FONTS_RADIO6D                        868
#define IDC_FONTS_RADIO6E                        869
#define IDC_FONTS_RADIO6F                        870
#define IDC_FONTS_RADIO7A                        871
#define IDC_FONTS_RADIO7B                        872
#define IDC_FONTS_RADIO7C                        873
#define IDC_FONTS_RADIO7D                        874
#define IDC_FONTS_RADIO7E                        875
#define IDC_FONTS_RADIO7F                        876
#define IDC_FONTS_RADIO7G                        877
#define IDC_FONTS_RADIO8A                        878
#define IDC_FONTS_RADIO8B                        879
#define IDC_FONTS_RADIO8C                        880
#define IDC_FONTS_RADIO8D                        881
#define IDC_FONTS_RADIO8E                        882
#define IDC_FONTS_RADIO8F                        883
#define IDC_FONTS_RADIO8G                        884
#define IDC_FONTS_RADIO8H                        885

#define IDC_FONTS_TEXT                           889

#define IDC_RANGE_XB_CT                          890
#define IDC_RANGE_XB_IO                          891
#define IDC_RANGE_XB_FEATURE                     892
#define IDC_RANGE_XB_FPC                         893
#define IDC_RANGE_XB_IC                          894
#define IDC_RANGE_XB_PP                          895
#define IDC_RANGE_XB_PW                          896
#define IDC_RANGE_XB_RL                          897

#define IDC_RESET_TEXT                           900

#define IDC_RESET_CT_RADIO1                      901
#define IDC_RESET_CT_RADIO2                      902
#define IDC_RESET_DT_RADIO1                      903
#define IDC_RESET_DT_RADIO2                      904
#define IDC_RESET_FC_RADIO1                      905
#define IDC_RESET_FC_RADIO2                      906
#define IDC_RESET_FEATURE_RADIO1                 907
#define IDC_RESET_FEATURE_RADIO2                 908
#define IDC_RESET_FPC_RADIO1                     909
#define IDC_RESET_FPC_RADIO2                     910
#define IDC_RESET_IC_RADIO1                      911
#define IDC_RESET_IC_RADIO2                      912
#define IDC_RESET_IO_RADIO1                      913
#define IDC_RESET_IO_RADIO2                      914
#define IDC_RESET_PP_RADIO1                      915
#define IDC_RESET_PP_RADIO2                      916
#define IDC_RESET_PW_RADIO1                      917
#define IDC_RESET_PW_RADIO2                      918
#define IDC_RESET_RL_RADIO1                      919
#define IDC_RESET_RL_RADIO2                      920

#define IDC_SYNTCLR_MI1                         1020
#define IDC_SYNTCLR_MI2                         1021
#define IDC_SYNTCLR_MI3                         1022
#define IDC_SYNTCLR_MI4                         1023
#define IDC_SYNTCLR_MI5                         1024
#define IDC_SYNTCLR_MI6                         1025
#define IDC_SYNTCLR_MI7                         1026
#define IDC_SYNTCLR_MI8                         1027
#define IDC_SYNTCLR_MI9                         1028
#define IDC_SYNTCLR_MI10                        1029
#define IDC_SYNTCLR_MI11                        1030
#define IDC_SYNTCLR_MI12                        1031
#define IDC_SYNTCLR_MI13                        1032
#define IDC_SYNTCLR_MI14                        1033
#define IDC_SYNTCLR_MI15                        1034
#define IDC_SYNTCLR_MI16                        1035
#define IDC_SYNTCLR_MI17                        1036
#define IDC_SYNTCLR_MI18                        1037
#define IDC_SYNTCLR_MI_LAST                     IDC_SYNTCLR_MI18
#define IDC_SYNTCLR_MI_CUSTOM                   IDC_SYNTCLR_MI17    // ID of the Custom Colors entry
#define IDC_SYNTCLR_MI_WEBCOLORS                IDC_SYNTCLR_MI18    // ...       Web    ...

#define IDC_SYNTCLR_XB_CLRFCNS                  1040
#define IDC_SYNTCLR_XB_CLRSESS                  1041
#define IDC_SYNTCLR_XB_CLRPRNT                  1042
#define IDC_SYNTCLR_BN_RESTORE                  1043

#define IDC_USER_PREFS_XB_ADJUSTPW              1100
#define IDC_USER_PREFS_XB_UNDERBARTOLOWERCASE   1101
#define IDC_USER_PREFS_XB_NEWTABONCLEAR         1102
#define IDC_USER_PREFS_XB_NEWTABONLOAD          1103
#define IDC_USER_PREFS_XB_USELOCALTIME          1104
#define IDC_USER_PREFS_XB_BACKUPONLOAD          1105
#define IDC_USER_PREFS_XB_BACKUPONSAVE          1106
#define IDC_USER_PREFS_XB_NOCOPYRIGHTMSG        1107
#define IDC_USER_PREFS_XB_CHECKGROUP            1108
#define IDC_USER_PREFS_XB_INSSTATE              1109
#define IDC_USER_PREFS_XB_REVDBLCLK             1110
#define IDC_USER_PREFS_XB_DEFDISPFCNLINENUMS    1111
#define IDC_USER_PREFS_XB_DISPMPSUF             1112
#define IDC_USER_PREFS_XB_OUTPUTDEBUG           1113
#define IDC_USER_PREFS_CB_DEFAULTPASTE          1114
#define IDC_USER_PREFS_CB_DEFAULTCOPY           1115
#define IDC_USER_PREFS_CB_UPDFRQ                1116

#define IDC_USER_PREFS_EC_UNICODE               1120
#define IDC_USER_PREFS_BN_UNICODE               1121
#define IDC_USER_PREFS_RB_DEC                   1122
#define IDC_USER_PREFS_RB_HEX                   1123

#define IDC_SYNTCLR_LT1                         1400    // 00:  Global Name
#define IDC_SYNTCLR_LT2                         1401    // 01:  Local  ...
#define IDC_SYNTCLR_LT3                         1402    // 02:  Label
#define IDC_SYNTCLR_LT4                         1403    // 03:  Primitive Function
#define IDC_SYNTCLR_LT5                         1404    // 04:  Primitive Monadic Operator
#define IDC_SYNTCLR_LT6                         1405    // 05:  Primitive Dyadic Operator
#define IDC_SYNTCLR_LT7                         1406    // 06:  System Function
#define IDC_SYNTCLR_LT8                         1407    // 07:  Global System Variable
#define IDC_SYNTCLR_LT9                         1408    // 08:  Local  ...
#define IDC_SYNTCLR_LT10                        1409    // 09:  Control Structure
#define IDC_SYNTCLR_LT11                        1410    // 0A:  Numeric constant
#define IDC_SYNTCLR_LT12                        1411    // 0B:  Character constant
#define IDC_SYNTCLR_LT13                        1412    // 0C:  Point notation separator
#define IDC_SYNTCLR_LT14                        1413    // 0D:  Comment
#define IDC_SYNTCLR_LT15                        1414    // 0E:  Line drawing chars
#define IDC_SYNTCLR_LT16                        1415    // 0F:  Function line numbers
#define IDC_SYNTCLR_LT17                        1416    // 10:  Matched Grouping Symbols Level 1
#define IDC_SYNTCLR_LT18                        1417    // 11:  ...                            2
#define IDC_SYNTCLR_LT19                        1418    // 12:  ...                            3
#define IDC_SYNTCLR_LT20                        1419    // 13:  ...                            4
#define IDC_SYNTCLR_LT21                        1420    // 14:  Unmatched Grouping Symbols
#define IDC_SYNTCLR_LT22                        1421    // 15:  Improperly Nested Grouping Symbols
#define IDC_SYNTCLR_LT23                        1422    // 16:  Unknown symbol
#define IDC_SYNTCLR_LT24                        1423    // 17:  Line Continuation
#define IDC_SYNTCLR_LT25                        1424    // 18:  Window background
#define IDC_SYNTCLR_LT_LAST                     IDC_SYNTCLR_LT25

#define IDC_SYNTCLR_XB_TRANS1                   1500    // 00:  Global Name
#define IDC_SYNTCLR_XB_TRANS2                   1501    // 01:  Local  ...
#define IDC_SYNTCLR_XB_TRANS3                   1502    // 02:  Label
#define IDC_SYNTCLR_XB_TRANS4                   1503    // 03:  Primitive Function
#define IDC_SYNTCLR_XB_TRANS5                   1504    // 04:  Primitive Monadic Operator
#define IDC_SYNTCLR_XB_TRANS6                   1505    // 05:  Primitive Dyadic Operator
#define IDC_SYNTCLR_XB_TRANS7                   1506    // 06:  System Function
#define IDC_SYNTCLR_XB_TRANS8                   1507    // 07:  Global System Variable
#define IDC_SYNTCLR_XB_TRANS9                   1508    // 08:  Local  ...
#define IDC_SYNTCLR_XB_TRANS10                  1509    // 09:  Control Structure
#define IDC_SYNTCLR_XB_TRANS11                  1510    // 0A:  Numeric constant
#define IDC_SYNTCLR_XB_TRANS12                  1511    // 0B:  Character constant
#define IDC_SYNTCLR_XB_TRANS13                  1512    // 0C:  Point notation separator
#define IDC_SYNTCLR_XB_TRANS14                  1513    // 0D:  Comment
#define IDC_SYNTCLR_XB_TRANS15                  1514    // 0E:  Line drawing chars
#define IDC_SYNTCLR_XB_TRANS16                  1515    // 0F:  Function line numbers
#define IDC_SYNTCLR_XB_TRANS17                  1516    // 10:  Matched Grouping Symbols Level 1
#define IDC_SYNTCLR_XB_TRANS18                  1517    // 11:  ...                            2
#define IDC_SYNTCLR_XB_TRANS19                  1518    // 12:  ...                            3
#define IDC_SYNTCLR_XB_TRANS20                  1519    // 13:  ...                            4
#define IDC_SYNTCLR_XB_TRANS21                  1520    // 14:  Unmatched Grouping Symbols
#define IDC_SYNTCLR_XB_TRANS22                  1521    // 15:  Improperly Nested Grouping Symbols
#define IDC_SYNTCLR_XB_TRANS23                  1522    // 16:  Unknown symbol
#define IDC_SYNTCLR_XB_TRANS24                  1523    // 17:  Line Continuation
#define IDC_SYNTCLR_XB_TRANS25                  1524    // 18:  Window background
#define IDC_SYNTCLR_XB_TRANS_LAST               IDC_SYNTCLR_XB_TRANS25

#define IDC_SYNTCLR_BN_FGCLR1                   1600    // 00:  Global Name
#define IDC_SYNTCLR_BN_FGCLR2                   1601    // 01:  Local  ...
#define IDC_SYNTCLR_BN_FGCLR3                   1602    // 02:  Label
#define IDC_SYNTCLR_BN_FGCLR4                   1603    // 03:  Primitive Function
#define IDC_SYNTCLR_BN_FGCLR5                   1604    // 04:  Primitive Monadic Operator
#define IDC_SYNTCLR_BN_FGCLR6                   1605    // 05:  Primitive Dyadic Operator
#define IDC_SYNTCLR_BN_FGCLR7                   1606    // 06:  System Function
#define IDC_SYNTCLR_BN_FGCLR8                   1607    // 07:  Global System Variable
#define IDC_SYNTCLR_BN_FGCLR9                   1608    // 08:  Local  ...
#define IDC_SYNTCLR_BN_FGCLR10                  1609    // 09:  Control Structure
#define IDC_SYNTCLR_BN_FGCLR11                  1610    // 0A:  Numeric constant
#define IDC_SYNTCLR_BN_FGCLR12                  1611    // 0B:  Character constant
#define IDC_SYNTCLR_BN_FGCLR13                  1612    // 0C:  Point notation separator
#define IDC_SYNTCLR_BN_FGCLR14                  1613    // 0D:  Comment
#define IDC_SYNTCLR_BN_FGCLR15                  1614    // 0E:  Line drawing chars
#define IDC_SYNTCLR_BN_FGCLR16                  1615    // 0F:  Function line numbers
#define IDC_SYNTCLR_BN_FGCLR17                  1616    // 10:  Matched Grouping Symbols Level 1
#define IDC_SYNTCLR_BN_FGCLR18                  1617    // 11:  ...                            2
#define IDC_SYNTCLR_BN_FGCLR19                  1618    // 12:  ...                            3
#define IDC_SYNTCLR_BN_FGCLR20                  1619    // 13:  ...                            4
#define IDC_SYNTCLR_BN_FGCLR21                  1620    // 14:  Unmatched Grouping Symbols
#define IDC_SYNTCLR_BN_FGCLR22                  1621    // 15:  Improperly Nested Grouping Symbols
#define IDC_SYNTCLR_BN_FGCLR23                  1622    // 16:  Unknown symbol
#define IDC_SYNTCLR_BN_FGCLR24                  1623    // 17:  Line Continuation
#define IDC_SYNTCLR_BN_FGCLR25                  1624    // 18:  Window background
#define IDC_SYNTCLR_BN_FGCLR_LAST               IDC_SYNTCLR_BN_FGCLR25

#define IDC_SYNTCLR_BN_BGCLR1                   1700    // 00:  Global Name
#define IDC_SYNTCLR_BN_BGCLR2                   1701    // 01:  Local  ...
#define IDC_SYNTCLR_BN_BGCLR3                   1702    // 02:  Label
#define IDC_SYNTCLR_BN_BGCLR4                   1703    // 03:  Primitive Function
#define IDC_SYNTCLR_BN_BGCLR5                   1704    // 04:  Primitive Monadic Operator
#define IDC_SYNTCLR_BN_BGCLR6                   1705    // 05:  Primitive Dyadic Operator
#define IDC_SYNTCLR_BN_BGCLR7                   1706    // 06:  System Function
#define IDC_SYNTCLR_BN_BGCLR8                   1707    // 07:  Global System Variable
#define IDC_SYNTCLR_BN_BGCLR9                   1708    // 08:  Local  ...
#define IDC_SYNTCLR_BN_BGCLR10                  1709    // 09:  Control Structure
#define IDC_SYNTCLR_BN_BGCLR11                  1710    // 0A:  Numeric constant
#define IDC_SYNTCLR_BN_BGCLR12                  1711    // 0B:  Character constant
#define IDC_SYNTCLR_BN_BGCLR13                  1712    // 0C:  Point notation separator
#define IDC_SYNTCLR_BN_BGCLR14                  1713    // 0D:  Comment
#define IDC_SYNTCLR_BN_BGCLR15                  1714    // 0E:  Line drawing chars
#define IDC_SYNTCLR_BN_BGCLR16                  1715    // 0F:  Function line numbers
#define IDC_SYNTCLR_BN_BGCLR17                  1716    // 10:  Matched Grouping Symbols Level 1
#define IDC_SYNTCLR_BN_BGCLR18                  1717    // 11:  ...                            2
#define IDC_SYNTCLR_BN_BGCLR19                  1718    // 12:  ...                            3
#define IDC_SYNTCLR_BN_BGCLR20                  1719    // 13:  ...                            4
#define IDC_SYNTCLR_BN_BGCLR21                  1720    // 14:  Unmatched Grouping Symbols
#define IDC_SYNTCLR_BN_BGCLR22                  1721    // 15:  Improperly Nested Grouping Symbols
#define IDC_SYNTCLR_BN_BGCLR23                  1722    // 16:  Unknown symbol
#define IDC_SYNTCLR_BN_BGCLR24                  1723    // 17:  Line Continuation
#define IDC_SYNTCLR_BN_BGCLR25                  1724    // 18:  Window background
#define IDC_SYNTCLR_BN_BGCLR_LAST               IDC_SYNTCLR_BN_BGCLR25

#define IDC_SYNTCLR_LT_FGMRK1                   1800    // 00:  Global Name
#define IDC_SYNTCLR_LT_FGMRK2                   1801    // 01:  Local  ...
#define IDC_SYNTCLR_LT_FGMRK3                   1802    // 02:  Label
#define IDC_SYNTCLR_LT_FGMRK4                   1803    // 03:  Primitive Function
#define IDC_SYNTCLR_LT_FGMRK5                   1804    // 04:  Primitive Monadic Operator
#define IDC_SYNTCLR_LT_FGMRK6                   1805    // 05:  Primitive Dyadic Operator
#define IDC_SYNTCLR_LT_FGMRK7                   1806    // 06:  System Function
#define IDC_SYNTCLR_LT_FGMRK8                   1807    // 07:  Global System Variable
#define IDC_SYNTCLR_LT_FGMRK9                   1808    // 08:  Local  ...
#define IDC_SYNTCLR_LT_FGMRK10                  1809    // 09:  Control Structure
#define IDC_SYNTCLR_LT_FGMRK11                  1810    // 0A:  Numeric constant
#define IDC_SYNTCLR_LT_FGMRK12                  1811    // 0B:  Character constant
#define IDC_SYNTCLR_LT_FGMRK13                  1812    // 0C:  Point notation separator
#define IDC_SYNTCLR_LT_FGMRK14                  1813    // 0D:  Comment
#define IDC_SYNTCLR_LT_FGMRK15                  1814    // 0E:  Line drawing chars
#define IDC_SYNTCLR_LT_FGMRK16                  1815    // 0F:  Function line numbers
#define IDC_SYNTCLR_LT_FGMRK17                  1816    // 10:  Matched Grouping Symbols Level 1
#define IDC_SYNTCLR_LT_FGMRK18                  1817    // 11:  ...                            2
#define IDC_SYNTCLR_LT_FGMRK19                  1818    // 12:  ...                            3
#define IDC_SYNTCLR_LT_FGMRK20                  1819    // 13:  ...                            4
#define IDC_SYNTCLR_LT_FGMRK21                  1820    // 14:  Unmatched Grouping Symbols
#define IDC_SYNTCLR_LT_FGMRK22                  1821    // 15:  Improperly Nested Grouping Symbols
#define IDC_SYNTCLR_LT_FGMRK23                  1822    // 16:  Unknown symbol
#define IDC_SYNTCLR_LT_FGMRK24                  1823    // 17:  Line Continuation
#define IDC_SYNTCLR_LT_FGMRK25                  1824    // 18:  Window background
#define IDC_SYNTCLR_LT_FGMRK_LAST               IDC_SYNTCLR_LT_FGMRK25

#define IDC_SYNTCLR_LT_BGMRK1                   1900    // 00:  Global Name
#define IDC_SYNTCLR_LT_BGMRK2                   1901    // 01:  Local  ...
#define IDC_SYNTCLR_LT_BGMRK3                   1902    // 02:  Label
#define IDC_SYNTCLR_LT_BGMRK4                   1903    // 03:  Primitive Function
#define IDC_SYNTCLR_LT_BGMRK5                   1904    // 04:  Primitive Monadic Operator
#define IDC_SYNTCLR_LT_BGMRK6                   1905    // 05:  Primitive Dyadic Operator
#define IDC_SYNTCLR_LT_BGMRK7                   1906    // 06:  System Function
#define IDC_SYNTCLR_LT_BGMRK8                   1907    // 07:  Global System Variable
#define IDC_SYNTCLR_LT_BGMRK9                   1908    // 08:  Local  ...
#define IDC_SYNTCLR_LT_BGMRK10                  1909    // 09:  Control Structure
#define IDC_SYNTCLR_LT_BGMRK11                  1910    // 0A:  Numeric constant
#define IDC_SYNTCLR_LT_BGMRK12                  1911    // 0B:  Character constant
#define IDC_SYNTCLR_LT_BGMRK13                  1912    // 0C:  Point notation separator
#define IDC_SYNTCLR_LT_BGMRK14                  1913    // 0D:  Comment
#define IDC_SYNTCLR_LT_BGMRK15                  1914    // 0E:  Line drawing chars
#define IDC_SYNTCLR_LT_BGMRK16                  1915    // 0F:  Function line numbers
#define IDC_SYNTCLR_LT_BGMRK17                  1916    // 10:  Matched Grouping Symbols Level 1
#define IDC_SYNTCLR_LT_BGMRK18                  1917    // 11:  ...                            2
#define IDC_SYNTCLR_LT_BGMRK19                  1918    // 12:  ...                            3
#define IDC_SYNTCLR_LT_BGMRK20                  1919    // 13:  ...                            4
#define IDC_SYNTCLR_LT_BGMRK21                  1920    // 14:  Unmatched Grouping Symbols
#define IDC_SYNTCLR_LT_BGMRK22                  1921    // 15:  Improperly Nested Grouping Symbols
#define IDC_SYNTCLR_LT_BGMRK23                  1922    // 16:  Unknown symbol
#define IDC_SYNTCLR_LT_BGMRK24                  1923    // 17:  Line Continuation
#define IDC_SYNTCLR_LT_BGMRK25                  1924    // 18:  Window background
#define IDC_SYNTCLR_LT_BGMRK_LAST               IDC_SYNTCLR_LT_BGMRK25

#define IDC_WEBCLR_LT001                        2001
#define IDC_WEBCLR_LT002                        2002
#define IDC_WEBCLR_LT003                        2003
#define IDC_WEBCLR_LT004                        2004
#define IDC_WEBCLR_LT005                        2005
#define IDC_WEBCLR_LT006                        2006
#define IDC_WEBCLR_LT007                        2007
#define IDC_WEBCLR_LT008                        2008
#define IDC_WEBCLR_LT009                        2009
#define IDC_WEBCLR_LT010                        2010

#define IDC_WEBCLR_LT011                        2011
#define IDC_WEBCLR_LT012                        2012
#define IDC_WEBCLR_LT013                        2013
#define IDC_WEBCLR_LT014                        2014
#define IDC_WEBCLR_LT015                        2015
#define IDC_WEBCLR_LT016                        2016
#define IDC_WEBCLR_LT017                        2017
#define IDC_WEBCLR_LT018                        2018
#define IDC_WEBCLR_LT019                        2019
#define IDC_WEBCLR_LT020                        2020

#define IDC_WEBCLR_LT021                        2021
#define IDC_WEBCLR_LT022                        2022
#define IDC_WEBCLR_LT023                        2023
#define IDC_WEBCLR_LT024                        2024
#define IDC_WEBCLR_LT025                        2025
#define IDC_WEBCLR_LT026                        2026
#define IDC_WEBCLR_LT027                        2027
#define IDC_WEBCLR_LT028                        2028
#define IDC_WEBCLR_LT029                        2029
#define IDC_WEBCLR_LT030                        2030

#define IDC_WEBCLR_LT031                        2031
#define IDC_WEBCLR_LT032                        2032
#define IDC_WEBCLR_LT033                        2033
#define IDC_WEBCLR_LT034                        2034
#define IDC_WEBCLR_LT035                        2035
#define IDC_WEBCLR_LT036                        2036
#define IDC_WEBCLR_LT037                        2037
#define IDC_WEBCLR_LT038                        2038
#define IDC_WEBCLR_LT039                        2039
#define IDC_WEBCLR_LT040                        2040

#define IDC_WEBCLR_LT041                        2041
#define IDC_WEBCLR_LT042                        2042
#define IDC_WEBCLR_LT043                        2043
#define IDC_WEBCLR_LT044                        2044
#define IDC_WEBCLR_LT045                        2045
#define IDC_WEBCLR_LT046                        2046
#define IDC_WEBCLR_LT047                        2047
#define IDC_WEBCLR_LT048                        2048
#define IDC_WEBCLR_LT049                        2049
#define IDC_WEBCLR_LT050                        2050

#define IDC_WEBCLR_LT051                        2051
#define IDC_WEBCLR_LT052                        2052
#define IDC_WEBCLR_LT053                        2053
#define IDC_WEBCLR_LT054                        2054
#define IDC_WEBCLR_LT055                        2055
#define IDC_WEBCLR_LT056                        2056
#define IDC_WEBCLR_LT057                        2057
#define IDC_WEBCLR_LT058                        2058
#define IDC_WEBCLR_LT059                        2059
#define IDC_WEBCLR_LT060                        2060

#define IDC_WEBCLR_LT061                        2061
#define IDC_WEBCLR_LT062                        2062
#define IDC_WEBCLR_LT063                        2063
#define IDC_WEBCLR_LT064                        2064
#define IDC_WEBCLR_LT065                        2065
#define IDC_WEBCLR_LT066                        2066
#define IDC_WEBCLR_LT067                        2067
#define IDC_WEBCLR_LT068                        2068
#define IDC_WEBCLR_LT069                        2069
#define IDC_WEBCLR_LT070                        2070

#define IDC_WEBCLR_LT071                        2071
#define IDC_WEBCLR_LT072                        2072
#define IDC_WEBCLR_LT073                        2073
#define IDC_WEBCLR_LT074                        2074
#define IDC_WEBCLR_LT075                        2075
#define IDC_WEBCLR_LT076                        2076
#define IDC_WEBCLR_LT077                        2077
#define IDC_WEBCLR_LT078                        2078
#define IDC_WEBCLR_LT079                        2079
#define IDC_WEBCLR_LT080                        2080

#define IDC_WEBCLR_LT081                        2081
#define IDC_WEBCLR_LT082                        2082
#define IDC_WEBCLR_LT083                        2083
#define IDC_WEBCLR_LT084                        2084
#define IDC_WEBCLR_LT085                        2085
#define IDC_WEBCLR_LT086                        2086
#define IDC_WEBCLR_LT087                        2087
#define IDC_WEBCLR_LT088                        2088
#define IDC_WEBCLR_LT089                        2089
#define IDC_WEBCLR_LT090                        2090

#define IDC_WEBCLR_LT091                        2091
#define IDC_WEBCLR_LT092                        2092
#define IDC_WEBCLR_LT093                        2093
#define IDC_WEBCLR_LT094                        2094
#define IDC_WEBCLR_LT095                        2095
#define IDC_WEBCLR_LT096                        2096
#define IDC_WEBCLR_LT097                        2097
#define IDC_WEBCLR_LT098                        2098
#define IDC_WEBCLR_LT099                        2099
#define IDC_WEBCLR_LT100                        2100

#define IDC_WEBCLR_LT101                        2101
#define IDC_WEBCLR_LT102                        2102
#define IDC_WEBCLR_LT103                        2103
#define IDC_WEBCLR_LT104                        2104
#define IDC_WEBCLR_LT105                        2105
#define IDC_WEBCLR_LT106                        2106
#define IDC_WEBCLR_LT107                        2107
#define IDC_WEBCLR_LT108                        2108
#define IDC_WEBCLR_LT109                        2109
#define IDC_WEBCLR_LT110                        2110

#define IDC_WEBCLR_LT111                        2111
#define IDC_WEBCLR_LT112                        2112
#define IDC_WEBCLR_LT113                        2113
#define IDC_WEBCLR_LT114                        2114
#define IDC_WEBCLR_LT115                        2115
#define IDC_WEBCLR_LT116                        2116
#define IDC_WEBCLR_LT117                        2117
#define IDC_WEBCLR_LT118                        2118
#define IDC_WEBCLR_LT119                        2119
#define IDC_WEBCLR_LT120                        2120

#define IDC_WEBCLR_LT121                        2121
#define IDC_WEBCLR_LT122                        2122
#define IDC_WEBCLR_LT123                        2123
#define IDC_WEBCLR_LT124                        2124
#define IDC_WEBCLR_LT125                        2125
#define IDC_WEBCLR_LT126                        2126
#define IDC_WEBCLR_LT127                        2127
#define IDC_WEBCLR_LT128                        2128
#define IDC_WEBCLR_LT129                        2129
#define IDC_WEBCLR_LT130                        2130

#define IDC_WEBCLR_LT131                        2131
#define IDC_WEBCLR_LT132                        2132
#define IDC_WEBCLR_LT133                        2133
#define IDC_WEBCLR_LT134                        2134
#define IDC_WEBCLR_LT135                        2135
#define IDC_WEBCLR_LT136                        2136
#define IDC_WEBCLR_LT137                        2137
#define IDC_WEBCLR_LT138                        2138
#define IDC_WEBCLR_LT139                        2139
#define IDC_WEBCLR_LT140                        2140
#define IDC_WEBCLR_LT_LAST                      IDC_WEBCLR_LT140

#define IDC_WEBCLR_BN001                        3001
#define IDC_WEBCLR_BN002                        3002
#define IDC_WEBCLR_BN003                        3003
#define IDC_WEBCLR_BN004                        3004
#define IDC_WEBCLR_BN005                        3005
#define IDC_WEBCLR_BN006                        3006
#define IDC_WEBCLR_BN007                        3007
#define IDC_WEBCLR_BN008                        3008
#define IDC_WEBCLR_BN009                        3009
#define IDC_WEBCLR_BN010                        3010

#define IDC_WEBCLR_BN011                        3011
#define IDC_WEBCLR_BN012                        3012
#define IDC_WEBCLR_BN013                        3013
#define IDC_WEBCLR_BN014                        3014
#define IDC_WEBCLR_BN015                        3015
#define IDC_WEBCLR_BN016                        3016
#define IDC_WEBCLR_BN017                        3017
#define IDC_WEBCLR_BN018                        3018
#define IDC_WEBCLR_BN019                        3019
#define IDC_WEBCLR_BN020                        3020

#define IDC_WEBCLR_BN021                        3021
#define IDC_WEBCLR_BN022                        3022
#define IDC_WEBCLR_BN023                        3023
#define IDC_WEBCLR_BN024                        3024
#define IDC_WEBCLR_BN025                        3025
#define IDC_WEBCLR_BN026                        3026
#define IDC_WEBCLR_BN027                        3027
#define IDC_WEBCLR_BN028                        3028
#define IDC_WEBCLR_BN029                        3029
#define IDC_WEBCLR_BN030                        3030

#define IDC_WEBCLR_BN031                        3031
#define IDC_WEBCLR_BN032                        3032
#define IDC_WEBCLR_BN033                        3033
#define IDC_WEBCLR_BN034                        3034
#define IDC_WEBCLR_BN035                        3035
#define IDC_WEBCLR_BN036                        3036
#define IDC_WEBCLR_BN037                        3037
#define IDC_WEBCLR_BN038                        3038
#define IDC_WEBCLR_BN039                        3039
#define IDC_WEBCLR_BN040                        3040

#define IDC_WEBCLR_BN041                        3041
#define IDC_WEBCLR_BN042                        3042
#define IDC_WEBCLR_BN043                        3043
#define IDC_WEBCLR_BN044                        3044
#define IDC_WEBCLR_BN045                        3045
#define IDC_WEBCLR_BN046                        3046
#define IDC_WEBCLR_BN047                        3047
#define IDC_WEBCLR_BN048                        3048
#define IDC_WEBCLR_BN049                        3049
#define IDC_WEBCLR_BN050                        3050

#define IDC_WEBCLR_BN051                        3051
#define IDC_WEBCLR_BN052                        3052
#define IDC_WEBCLR_BN053                        3053
#define IDC_WEBCLR_BN054                        3054
#define IDC_WEBCLR_BN055                        3055
#define IDC_WEBCLR_BN056                        3056
#define IDC_WEBCLR_BN057                        3057
#define IDC_WEBCLR_BN058                        3058
#define IDC_WEBCLR_BN059                        3059
#define IDC_WEBCLR_BN060                        3060

#define IDC_WEBCLR_BN061                        3061
#define IDC_WEBCLR_BN062                        3062
#define IDC_WEBCLR_BN063                        3063
#define IDC_WEBCLR_BN064                        3064
#define IDC_WEBCLR_BN065                        3065
#define IDC_WEBCLR_BN066                        3066
#define IDC_WEBCLR_BN067                        3067
#define IDC_WEBCLR_BN068                        3068
#define IDC_WEBCLR_BN069                        3069
#define IDC_WEBCLR_BN070                        3070

#define IDC_WEBCLR_BN071                        3071
#define IDC_WEBCLR_BN072                        3072
#define IDC_WEBCLR_BN073                        3073
#define IDC_WEBCLR_BN074                        3074
#define IDC_WEBCLR_BN075                        3075
#define IDC_WEBCLR_BN076                        3076
#define IDC_WEBCLR_BN077                        3077
#define IDC_WEBCLR_BN078                        3078
#define IDC_WEBCLR_BN079                        3079
#define IDC_WEBCLR_BN080                        3080

#define IDC_WEBCLR_BN081                        3081
#define IDC_WEBCLR_BN082                        3082
#define IDC_WEBCLR_BN083                        3083
#define IDC_WEBCLR_BN084                        3084
#define IDC_WEBCLR_BN085                        3085
#define IDC_WEBCLR_BN086                        3086
#define IDC_WEBCLR_BN087                        3087
#define IDC_WEBCLR_BN088                        3088
#define IDC_WEBCLR_BN089                        3089
#define IDC_WEBCLR_BN090                        3090

#define IDC_WEBCLR_BN091                        3091
#define IDC_WEBCLR_BN092                        3092
#define IDC_WEBCLR_BN093                        3093
#define IDC_WEBCLR_BN094                        3094
#define IDC_WEBCLR_BN095                        3095
#define IDC_WEBCLR_BN096                        3096
#define IDC_WEBCLR_BN097                        3097
#define IDC_WEBCLR_BN098                        3098
#define IDC_WEBCLR_BN099                        3099
#define IDC_WEBCLR_BN100                        3100

#define IDC_WEBCLR_BN101                        3101
#define IDC_WEBCLR_BN102                        3102
#define IDC_WEBCLR_BN103                        3103
#define IDC_WEBCLR_BN104                        3104
#define IDC_WEBCLR_BN105                        3105
#define IDC_WEBCLR_BN106                        3106
#define IDC_WEBCLR_BN107                        3107
#define IDC_WEBCLR_BN108                        3108
#define IDC_WEBCLR_BN109                        3109
#define IDC_WEBCLR_BN110                        3110

#define IDC_WEBCLR_BN111                        3111
#define IDC_WEBCLR_BN112                        3112
#define IDC_WEBCLR_BN113                        3113
#define IDC_WEBCLR_BN114                        3114
#define IDC_WEBCLR_BN115                        3115
#define IDC_WEBCLR_BN116                        3116
#define IDC_WEBCLR_BN117                        3117
#define IDC_WEBCLR_BN118                        3118
#define IDC_WEBCLR_BN119                        3119
#define IDC_WEBCLR_BN120                        3120

#define IDC_WEBCLR_BN121                        3121
#define IDC_WEBCLR_BN122                        3122
#define IDC_WEBCLR_BN123                        3123
#define IDC_WEBCLR_BN124                        3124
#define IDC_WEBCLR_BN125                        3125
#define IDC_WEBCLR_BN126                        3126
#define IDC_WEBCLR_BN127                        3127
#define IDC_WEBCLR_BN128                        3128
#define IDC_WEBCLR_BN129                        3129
#define IDC_WEBCLR_BN130                        3130

#define IDC_WEBCLR_BN131                        3131
#define IDC_WEBCLR_BN132                        3132
#define IDC_WEBCLR_BN133                        3133
#define IDC_WEBCLR_BN134                        3134
#define IDC_WEBCLR_BN135                        3135
#define IDC_WEBCLR_BN136                        3136
#define IDC_WEBCLR_BN137                        3137
#define IDC_WEBCLR_BN138                        3138
#define IDC_WEBCLR_BN139                        3139
#define IDC_WEBCLR_BN140                        3140
#define IDC_WEBCLR_BN_LAST                      IDC_WEBCLR_BN140

#define IDC_WEBCLR_LT_REP                       4000

#define IDC_KEYB_CB_LAYOUT                      4150
#define IDC_KEYB_BN_MAKEACT                     4151
#define IDC_KEYB_LT_ISACT                       4152
#define IDC_KEYB_XB_ALT                         4153    // MUST be lowest
#define IDC_KEYB_XB_CTRL                        4154    // ...     middle
#define IDC_KEYB_XB_SHIFT                       4155    // ...     highest
#define IDC_KEYB_EC_UNICODE                     4156
#define IDC_KEYB_BN_UNICODE                     4157
#define IDC_KEYB_RB_DEC                         4158
#define IDC_KEYB_RB_HEX                         4159
#define IDC_KEYB_RB_CLIP0                       4160
#define IDC_KEYB_RB_CLIP1                       4161
#define IDC_KEYB_RB_UNDO0                       4162
#define IDC_KEYB_RB_UNDO1                       4163
#define IDC_KEYB_RB_FNED0                       4164
#define IDC_KEYB_RB_FNED1                       4165
#define IDC_KEYB_BN_COPY                        4166
#define IDC_KEYB_BN_DEL                         4167
#define IDC_KEYB_BN_FONT                        4168
#define IDC_KEYB_TC                             4169
#define IDC_NEWKEYB_EC                          4170

#define IDC_KEYB_BN_KC_00                       4200
#define IDC_KEYB_BN_KC_01                       4201
#define IDC_KEYB_BN_KC_02                       4202
#define IDC_KEYB_BN_KC_03                       4203
#define IDC_KEYB_BN_KC_04                       4204
#define IDC_KEYB_BN_KC_05                       4205
#define IDC_KEYB_BN_KC_06                       4206
#define IDC_KEYB_BN_KC_07                       4207
#define IDC_KEYB_BN_KC_08                       4208
#define IDC_KEYB_BN_KC_09                       4209
#define IDC_KEYB_BN_KC_0A                       4210
#define IDC_KEYB_BN_KC_0B                       4211
#define IDC_KEYB_BN_KC_0C                       4212

#define IDC_KEYB_BN_KC_10                       4220
#define IDC_KEYB_BN_KC_11                       4221
#define IDC_KEYB_BN_KC_12                       4222
#define IDC_KEYB_BN_KC_13                       4223
#define IDC_KEYB_BN_KC_14                       4224
#define IDC_KEYB_BN_KC_15                       4225
#define IDC_KEYB_BN_KC_16                       4226
#define IDC_KEYB_BN_KC_17                       4227
#define IDC_KEYB_BN_KC_18                       4228
#define IDC_KEYB_BN_KC_19                       4229
#define IDC_KEYB_BN_KC_1A                       4230
#define IDC_KEYB_BN_KC_1B                       4231
#define IDC_KEYB_BN_KC_1C                       4232

#define IDC_KEYB_BN_KC_20                       4240
#define IDC_KEYB_BN_KC_21                       4241
#define IDC_KEYB_BN_KC_22                       4242
#define IDC_KEYB_BN_KC_23                       4243
#define IDC_KEYB_BN_KC_24                       4244
#define IDC_KEYB_BN_KC_25                       4245
#define IDC_KEYB_BN_KC_26                       4246
#define IDC_KEYB_BN_KC_27                       4247
#define IDC_KEYB_BN_KC_28                       4248
#define IDC_KEYB_BN_KC_29                       4249
#define IDC_KEYB_BN_KC_2A                       4250
#define IDC_KEYB_BN_KC_2B                       4251

#define IDC_KEYB_BN_KC_30                       4260
#define IDC_KEYB_BN_KC_31                       4261
#define IDC_KEYB_BN_KC_32                       4262
#define IDC_KEYB_BN_KC_33                       4263
#define IDC_KEYB_BN_KC_34                       4264
#define IDC_KEYB_BN_KC_35                       4265
#define IDC_KEYB_BN_KC_36                       4266
#define IDC_KEYB_BN_KC_37                       4267
#define IDC_KEYB_BN_KC_38                       4268
#define IDC_KEYB_BN_KC_39                       4269
#define IDC_KEYB_BN_KC_3A                       4270

#define IDC_KEYB_BN_KC_LAST                     IDC_KEYB_BN_KC_3A

// N.B.:  The individual entries within a row must be consecutive
#define IDC_KEYB_BN_TC_00                       4300
#define IDC_KEYB_BN_TC_01                       4301
#define IDC_KEYB_BN_TC_02                       4302
#define IDC_KEYB_BN_TC_03                       4303
#define IDC_KEYB_BN_TC_04                       4304
#define IDC_KEYB_BN_TC_05                       4305

#define IDC_KEYB_BN_TC_10                       4310
#define IDC_KEYB_BN_TC_11                       4311
#define IDC_KEYB_BN_TC_12                       4312
#define IDC_KEYB_BN_TC_13                       4313
#define IDC_KEYB_BN_TC_14                       4314
#define IDC_KEYB_BN_TC_15                       4315

#define IDC_KEYB_BN_TC_20                       4320
#define IDC_KEYB_BN_TC_21                       4321
#define IDC_KEYB_BN_TC_22                       4322
#define IDC_KEYB_BN_TC_23                       4323
#define IDC_KEYB_BN_TC_24                       4324
#define IDC_KEYB_BN_TC_25                       4325

#define IDC_KEYB_BN_TC_30                       4330
#define IDC_KEYB_BN_TC_31                       4331
#define IDC_KEYB_BN_TC_32                       4332
#define IDC_KEYB_BN_TC_33                       4333
#define IDC_KEYB_BN_TC_34                       4334
#define IDC_KEYB_BN_TC_35                       4335

#define IDC_KEYB_BN_TC_40                       4340
#define IDC_KEYB_BN_TC_41                       4341
#define IDC_KEYB_BN_TC_42                       4342
#define IDC_KEYB_BN_TC_43                       4343
#define IDC_KEYB_BN_TC_44                       4344
#define IDC_KEYB_BN_TC_45                       4345

#define IDC_KEYB_BN_TC_50                       4350
#define IDC_KEYB_BN_TC_51                       4351
#define IDC_KEYB_BN_TC_52                       4352
#define IDC_KEYB_BN_TC_53                       4353
#define IDC_KEYB_BN_TC_54                       4354
#define IDC_KEYB_BN_TC_55                       4355

#define IDC_KEYB_BN_TC_60                       4360
#define IDC_KEYB_BN_TC_61                       4361
#define IDC_KEYB_BN_TC_62                       4362
#define IDC_KEYB_BN_TC_63                       4363
#define IDC_KEYB_BN_TC_64                       4364
#define IDC_KEYB_BN_TC_65                       4365

#define IDC_KEYB_BN_TC_70                       4370
#define IDC_KEYB_BN_TC_71                       4371
#define IDC_KEYB_BN_TC_72                       4372
#define IDC_KEYB_BN_TC_73                       4373
#define IDC_KEYB_BN_TC_74                       4374
#define IDC_KEYB_BN_TC_75                       4375

#define IDC_KEYB_BN_TC_80                       4380
#define IDC_KEYB_BN_TC_81                       4381
#define IDC_KEYB_BN_TC_82                       4382
#define IDC_KEYB_BN_TC_83                       4383
#define IDC_KEYB_BN_TC_84                       4384
#define IDC_KEYB_BN_TC_85                       4385

#define IDC_KEYB_BN_TC_90                       4390
#define IDC_KEYB_BN_TC_91                       4391
#define IDC_KEYB_BN_TC_92                       4392
#define IDC_KEYB_BN_TC_93                       4393
#define IDC_KEYB_BN_TC_94                       4394
#define IDC_KEYB_BN_TC_95                       4395

#define IDC_KEYB_BN_TC_A0                       4400
#define IDC_KEYB_BN_TC_A1                       4401
#define IDC_KEYB_BN_TC_A2                       4402
#define IDC_KEYB_BN_TC_A3                       4403
#define IDC_KEYB_BN_TC_A4                       4404
#define IDC_KEYB_BN_TC_A5                       4405

#define IDC_KEYB_BN_TC_B0                       4410
#define IDC_KEYB_BN_TC_B1                       4411
#define IDC_KEYB_BN_TC_B2                       4412
#define IDC_KEYB_BN_TC_B3                       4413
#define IDC_KEYB_BN_TC_B4                       4414
#define IDC_KEYB_BN_TC_B5                       4415

#define IDC_KEYB_BN_TC_C0                       4420
#define IDC_KEYB_BN_TC_C1                       4421
#define IDC_KEYB_BN_TC_C2                       4422
#define IDC_KEYB_BN_TC_C3                       4423
#define IDC_KEYB_BN_TC_C4                       4424
#define IDC_KEYB_BN_TC_C5                       4425

#define IDC_KEYB_BN_TC_LAST                     IDC_KEYB_BN_TC_C5

#define IDC_FONTS_LT1                           5000
#define IDC_FONTS_LT2                           5001
#define IDC_FONTS_LT3                           5002
#define IDC_FONTS_LT4                           5003
#define IDC_FONTS_LT5                           5004
#define IDC_FONTS_LT6                           5005
#define IDC_FONTS_LT7                           5006
#define IDC_FONTS_LT8                           5007
#define IDC_FONTS_LT_LAST                        IDC_FONTS_LT8

#define IDC_DIRS_CB_LIBDIRS                     5100
#define IDC_DIRS_BN_BROWSE                      5101
#define IDC_DIRS_BN_ADD                         5102
#define IDC_DIRS_BN_DEL                         5103
#define IDC_DIRS_LT1                            5104
#define IDC_DIRS_LT2                            5105

#define IDC_DNLPCT_LT                           6000
#define IDC_DNLRUN_BN                           6001
#define IDC_DNLPAUSE_BN                         6002
#define IDC_DNL_LT                              6003
#define IDC_DNL_XB                              6004
#define IDC_DNL_PB                              6005

#define IDC_STATIC                                -1

// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
  #ifndef APSTUDIO_READONLY_SYMBOLS
    #define _APS_NEXT_RESOURCE_VALUE          109
    #define _APS_NEXT_COMMAND_VALUE         40001
    #define _APS_NEXT_CONTROL_VALUE           850
    #define _APS_NEXT_SYMED_VALUE             101
  #endif
#endif


//***************************************************************************
//  End of File: resource.h
//***************************************************************************
