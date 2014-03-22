//****************************************************************************
//  NARS2000 -- Unicode Constants
//****************************************************************************

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

#ifndef NARS_UNICODE
#define NARS_UNICODE

// When initializing an array of functions (such as PrimFnsTab) use these
//   equates instead of the low-order byte of the corresponding UTF16_xxx
//   so as to avoid a conflict.
#define INDEX_JOTDOT              0x01          // Index for {jot}{dot}
#define INDEX_OPSLASH             0x02          // ...       {slash} as an operator
#define INDEX_OPSLASHBAR          0x03          // ...       {slashbar} ...
#define INDEX_OPSLOPE             0x04          // ...       {slope}    ...
#define INDEX_OPSLOPEBAR          0x05          // ...       {slopebar} ...
#define INDEX_OPTRAIN             0x06          // ...       Train      ...
///////                          0x07-0x18      // (available)

// N.B.:  Whenever changing any of the UTF16_xxx symbol
//   names below, be sure to make a corresponding change
//   to <aSymbolNames> in <symbolnames.h>.

// No keystroke equivalents for these as yet
#define UTF16_PLUS_OR_MINUS       0x00B1        // Plus or minus symbol
#define UTF16_LEFTSINGLEQUOTE     0x2018        // Left single quotation mark
#define UTF16_RIGHTSINGLEQUOTE    0x2019        // Right ...
#define UTF16_HORIZELLIPSIS       0x2026        // Horizontal ellipsis
#define UTF16_EURO                0x20AC        // Euro
#define UTF16_LEFTRIGHTARROWS     0x2194        // Left-right arrows
#define UTF16_UPDOWNARROWS        0x2195        // Up-down arrows
#define UTF16_CIRCLESLASH         0x2205        //
#define UTF16_CHECKMARK           0x221A        // Check mark
#define UTF16_DIERESISDOT         0x2235        //
#define UTF16_NAND                0x22BC        //
#define UTF16_NOR                 0x22BD        //
#define UTF16_IBEAM               0x2336        //
#define UTF16_QUADEQUAL           0x2338        //
#define UTF16_QUADDIVIDE          0x2339        //
#define UTF16_QUADDIAMOND         0x233A        //
#define UTF16_QUADJOT             0x233B        //
#define UTF16_QUADCIRCLE          0x233C        //
#define UTF16_CIRCLEJOT           0x233E        //
#define UTF16_QUADSLASH           0x2341        //
#define UTF16_QUADSLOPE           0x2342        //
#define UTF16_QUADLEFTCARET       0x2343        //
#define UTF16_QUADRIGHTCARET      0x2344        //
#define UTF16_QUADLEFTVANE        0x2345        //
#define UTF16_QUADRIGHTVANE       0x2346        //
#define UTF16_QUADLEFTARROW       0x2347        //
#define UTF16_QUADRIGHTARROW      0x2348        //
#define UTF16_UPTACKUNDERBAR      0x234A        //
#define UTF16_QUADDOWNCARET       0x234C        //
#define UTF16_QUADDELTA           0x234D        //
#define UTF16_UPVANE              0x234F        //
#define UTF16_QUADUPARROW         0x2350        //
#define UTF16_DOWNTACKOVERBAR     0x2351        //
#define UTF16_QUADUPCARET         0x2353        //
#define UTF16_QUADDEL             0x2354        //
#define UTF16_DOWNVANE            0x2356        //
#define UTF16_QUADDOWNARROW       0x2357        //
#define UTF16_QUOTEUNDERBAR       0x2358        //
#define UTF16_DIAMONDUNDERBAR     0x235A        //
#define UTF16_JOTUNDERBAR         0x235B        //
#define UTF16_CIRCLEUNDERBAR      0x235C        //
#define UTF16_QUADCOLON           0x2360        // Variant operator
#define UTF16_LEFTSHOESTILE       0x2367        //
#define UTF16_DIERESISRIGHTCARET  0x2369        //
#define UTF16_DELTILDE            0x236B        // Del-tilde
#define UTF16_SEMICOLONUNDERBAR   0x236E        //
#define UTF16_QUADNOTEQUAL        0x236F        //
#define UTF16_QUADQUERY           0x2370        //
#define UTF16_ALPHAUNDERBAR       0x2376        //
#define UTF16_EPSILONUNDERBAR     0x2377        //
#define UTF16_IOTAUNDERBAR        0x2378        //
#define UTF16_OMEGAUNDERBAR       0x2379        //
#define UTF16_LDC_LT_HORZ         0x2500        // LDC light horizontal
#define UTF16_LDC_LT_VERT         0x2502        // LDC light vertical
#define UTF16_LDC_LT_UL           0x250C        // LDC light upper left
#define UTF16_LDC_LT_UR           0x2510        // LDC light upper right
#define UTF16_LDC_LT_LL           0x2514        // LDC light lower left
#define UTF16_LDC_LT_LR           0x2518        // LDC light lower right
#define UTF16_LDC_LT_VERT_R       0x251C        // LDC light vertical and right
#define UTF16_LDC_LT_VERT_L       0x2524        // LDC light vertical and left
#define UTF16_LDC_LT_HORZ_D       0x252C        // LDC light horizontal and down
#define UTF16_LDC_LT_HORZ_U       0x2534        // LDC light horizontal and up
#define UTF16_LDC_LT_CROSS        0x253C        // LDC light horizontal and vertical
#define UTF16_LDC_DB_HORZ         0x2550        // LDC double horizontal
#define UTF16_LDC_DB_VERT         0x2551        // LDC double vertical
#define UTF16_LDC_DB_UL           0x2554        // LDC double upper left
#define UTF16_LDC_DB_UR           0x2557        // LDC double upper right
#define UTF16_LDC_DB_LL           0x255A        // LDC double lower left
#define UTF16_LDC_DB_LR           0x255D        // LDC double lower right
#define UTF16_LDC_DB_VERT_R       0x2560        // LDC double vertical and right
#define UTF16_LDC_DB_VERT_L       0x2563        // LDC double vertical and left
#define UTF16_LDC_DB_HORZ_D       0x2566        // LDC double horizontal and down
#define UTF16_LDC_DB_HORZ_U       0x2569        // LDC double horizontal and up
#define UTF16_LDC_DB_CROSS        0x256C        // LDC double horizontal and vertical
#define UTF16_REPLACEMENTCHAR     0xFFFD        // Replacement char for unknown

// The alphabet, unshifted
#define UTF16_ALPHA               0x237A        // Alt-'a' - alpha
#define UTF16_UPTACK              0x22A5        // Alt-'b' - base
#define UTF16_UPSHOE              0x2229        // Alt-'c' - intersection
#define UTF16_DOWNSTILE           0x230A        // Alt-'d' - floor
#define UTF16_EPSILON             0x220A        // Alt-'e' - epsilon
#define UTF16_INFINITY            0x221E        // Alt-'f' - infinity
#define UTF16_DEL                 0x2207        // Alt-'g' - del
#define UTF16_DELTA               0x2206        // Alt-'h' - delta
#define UTF16_IOTA                0x2373        // Alt-'i' - iota
#define UTF16_JOT                 0x2218        // Alt-'j' - Compose operator (jot)
#define UTF16_APOSTROPHE          0x0027        // Alt-'k' - single quote
#define UTF16_QUAD                0x2395        // Alt-'l' - quad (not 0x25AF)
#define UTF16_DOWNSHOESTILE       0x2366        // Alt-'m' - down-shoe-stile
#define UTF16_DOWNTACK            0x22A4        // Alt-'n' - representation
#define UTF16_CIRCLE              0x25CB        // Alt-'o' - circle
#define UTF16_PI                  0x03C0        // Alt-'p' - pi
#define UTF16_QUERY               0x003F        // Alt-'q' - question-mark
#define UTF16_RHO                 0x2374        // Alt-'r' - rho
#define UTF16_UPSTILE             0x2308        // Alt-'s' - ceiling
#define UTF16_TILDE               0x223C        // Alt-'t' - tilde
#define UTF16_DOWNARROW           0x2193        // Alt-'u' - down arrow
#define UTF16_DOWNSHOE            0x222A        // Alt-'v' - union
#define UTF16_OMEGA               0x2375        // Alt-'w' - omega
#define UTF16_RIGHTSHOE           0x2283        // Alt-'x' - disclose
#define UTF16_UPARROW             0x2191        // Alt-'y' - up arrow
#define UTF16_LEFTSHOE            0x2282        // Alt-'z' - enclose

// The alphabet, shifted
//efine UTF16_                                  // Alt-'A' - (none)
//efine UTF16_                                  // Alt-'B' - (none)
//efine UTF16_                                  // Alt-'C' - (none)
//efine UTF16_                                  // Alt-'D' - (none)
#define UTF16_EPSILONUNDERBAR     0x2377        // Alt-'E' - epsilon-underbar
//efine UTF16_                                  // Alt-'F' - (none)
#define UTF16_DIERESISDEL         0x2362        // Alt-'G' - Dual operator        (frog)
#define UTF16_DELTAUNDERBAR       0x2359        // Alt-'H' - delta-underbar
#define UTF16_IOTAUNDERBAR        0x2378        // Alt-'I' - iota-underbar
#define UTF16_DIERESISJOT         0x2364        // Alt-'J' - Rank operator        (hoot)
//efine UTF16_                                  // Alt-'K' - (none)
#define UTF16_SQUAD               0x2337        // Alt-'L' - squad
#define UTF16_STILETILDE          0x236D        // Alt-'M' - Partition operator   (dagger)
#define UTF16_DIERESISDOWNTACK    0x2361        // Alt-'N' - Convolution operator (snout)
#define UTF16_DIERESISCIRCLE      0x2365        // Alt-'O' - Composition operator (holler)
#define UTF16_DIERESISSTAR        0x2363        // Alt-'P' - Power operator       (sourpuss)
//efine UTF16_                                  // Alt-'Q' - (none)
#define UTF16_ROOT                0x221A        // Alt-'R' - Root
#define UTF16_SECTION             0x00A7        // Alt-'S' - Multiset Symmetric Difference (section)
#define UTF16_DIERESISTILDE       0x2368        // Alt-'T' - Commute operator     (frown)
//efine UTF16_                                  // Alt-'U' - (none)
//efine UTF16_                                  // Alt-'V' - (none)
//efine UTF16_                                  // Alt-'W' - (none)
#define UTF16_RIGHTSHOEUNDERBAR   0x2287        // Alt-'X' - Superset
//efine UTF16_                                  // Alt-'Y' - (none)
#define UTF16_LEFTSHOEUNDERBAR    0x2286        // Alt-'Z' - Subset

// Top row, unshifted
#define UTF16_DIAMOND             0x22C4        // Alt-'`' - diamond (not 0x25CA)
#define UTF16_DIERESIS            0x00A8        // Alt-'1' - dieresis
#define UTF16_OVERBAR             0x00AF        // Alt-'2' - high minus
#define UTF16_LEFTCARET           0x003C        // Alt-'3' - less
#define UTF16_LEFTCARETUNDERBAR   0x2264        // Alt-'4' - not more
#define UTF16_EQUAL               0x003D        // Alt-'5' - equal
#define UTF16_RIGHTCARETUNDERBAR  0x2265        // Alt-'6' - not less
#define UTF16_RIGHTCARET          0x003E        // Alt-'7' - more
#define UTF16_NOTEQUAL            0x2260        // Alt-'8' - not equal
#define UTF16_DOWNCARET           0x2228        // Alt-'9' - or
#define UTF16_UPCARET             0x2227        // Alt-'0' - and (a.k.a. 0x005E)
#define UTF16_TIMES               0x00D7        // Alt-'-' - times
#define UTF16_COLONBAR            0x00F7        // Alt-'=' - divide

// Top row, shifted
#define UTF16_COMMABAR            0x236A        // Alt-'~' - comma-bar
#define UTF16_EQUALUNDERBAR       0x2261        // Alt-'!' - match
#define UTF16_NOTEQUALUNDERBAR    0x2262        // Alt-'@' - mismatch
#define UTF16_DELSTILE            0x2352        // Alt-'#' - grade-down
#define UTF16_DELTASTILE          0x234B        // Alt-'$' - grade-up
#define UTF16_CIRCLESTILE         0x233D        // Alt-'%' - rotate
#define UTF16_CIRCLESLOPE         0x2349        // Alt-'^' - transpose
#define UTF16_CIRCLEBAR           0x2296        // Alt-'&' - circle-bar
#define UTF16_CIRCLESTAR          0x235F        // Alt-'*' - log
#define UTF16_DOWNCARETTILDE      0x2371        // Alt-'(' - nor
#define UTF16_UPCARETTILDE        0x2372        // Alt-')' - nand
#define UTF16_VARIANT             0x2360        // Alt-'_' - variant
#define UTF16_DOMINO              0x2339        // Alt-'+' - domino

// Second row, unshifted
#define UTF16_LEFTARROW           0x2190        // Alt-'[' - left arrow
#define UTF16_RIGHTARROW          0x2192        // Alt-']' - right arrow
#define UTF16_LEFTTACK            0x22A3        // Alt-'\' - left tack

// Second row, shifted
#define UTF16_QUOTEQUAD           0x235E        // Alt-'{' - quote-quad
#define UTF16_ZILDE               0x236C        // Alt-'}' - zilde
#define UTF16_RIGHTTACK           0x22A2        // Alt-'|' - right tack

// Third row, unshifted
#define UTF16_UPTACKJOT           0x234E        // Alt-';' - execute
#define UTF16_DOWNTACKJOT         0x2355        // Alt-'\''- format

// Third row, shifted
//efine UTF16_                                  // Alt-':' - (none)
//efine UTF16_                                  // Alt-'"' - (none)

// Fourth row, unshifted
#define UTF16_LAMP                0x235D        // Alt-',' - comment
#define UTF16_SLOPEBAR            0x2340        // Alt-'.' - slope-bar
#define UTF16_SLASHBAR            0x233F        // Alt-'/' - slash-bar

// Fourth row, shifted
//efine UTF16_                                  // Alt-'<' - (none)
//efine UTF16_                                  // Alt-'>' - (none)
#define UTF16_CIRCLEMIDDLEDOT     0x2299        // Alt-'?' - circle-middle-dot

// Non-Alt key equivalents
#define UTF16_JOTDOT              0x0001        // Pseudo-symbol for {jot}{dot} digraph
#define UTF16_QUOTEDOT            0x0021        // Quote dot
#define UTF16_DOUBLEQUOTE         0x0022        // Double quote
#define UTF16_NUMBER              0x0023        // Number
#define UTF16_DOLLAR              0x0024        // Dollar
#define UTF16_PERCENT             0x0025        // Percent
#define UTF16_AMPERSAND           0x0026        // Ampersand
#define UTF16_SINGLEQUOTE         0x0027        // Single quote (a.k.a. Apostrophe)
#define UTF16_LEFTPAREN           0x0028        // Left paren
#define UTF16_RIGHTPAREN          0x0029        // Right paren
#define UTF16_STAR                0x002A        // Star
#define UTF16_PLUS                0x002B        // Plus
#define UTF16_COMMA               0x002C        // Comma
#define UTF16_BAR                 0x002D        // Bar
#define UTF16_DOT                 0x002E        // Dot
#define UTF16_SLASH               0x002F        // Slash
#define UTF16_COLON               0x003A        // Colon
#define UTF16_SEMICOLON           0x003B        // Semicolon
#define UTF16_ATSIGN              0x0040        // At sign
#define UTF16_LEFTBRACKET         0x005B        // Left bracket
#define UTF16_SLOPE               0x005C        // Slope
#define UTF16_RIGHTBRACKET        0x005D        // Right bracket
#define UTF16_CIRCUMFLEX          0x005E        // Circumflex (a.k.a. 0x2227)
#define UTF16_UNDERBAR            0x005F        // Underbar
#define UTF16_BACKTICK            0x0060        // Backtick
#define UTF16_LEFTBRACE           0x007B        // Left brace
#define UTF16_STILE2              0x007C        // Stile (a.k.a. 0x2223)
#define UTF16_RIGHTBRACE          0x007D        // Right brace
#define UTF16_TILDE2              0x007E        // Tilde (a.k.a. 0x223C)
#define UTF16_POUND               0x00A3        // Pound
#define UTF16_CURRENCY            0x00A4        // Currency
#define UTF16_COPYRIGHT           0x00A9        // Copyright
#define UTF16_ACUTE               0x00B4        // Acute accent
#define UTF16_MICRO               0x00B5        // Micro
#define UTF16_ONE_FOURTH          0x00BC        // One fourth
#define UTF16_ONE_HALF            0x00BD        // One half
#define UTF16_THREE_FOURTH        0x00BE        // Three fourth
#define UTF16_CAP_A_RING          0x00C5        // Capital A w/ring
#define UTF16_CAP_AE              0x00C6        // Capital AE
#define UTF16_CAP_O_STROKE        0x00D8        // Capital O w/stroke
#define UTF16_LOW_A_RING          0x00E5        // Lower   a w/ring
#define UTF16_LOW_AE              0x00E6        // Lower   ae
#define UTF16_LOW_O_STROKE        0x00F8        // Lower   o w/stroke
#define UTF16_ALPHA2              0x03B1        // Greek small letter alpha
#define UTF16_OMEGA2              0x03C9        // ...                omega
#define UTF16_BAR2                0x2212        // Bar (a.k.a. 0x002D)
#define UTF16_STILE               0x2223        // Stile
#define UTF16_STAR2               0x22C6        // Star (a.k.a. 0x002A)
#define UTF16_QUAD2               0x25AF        // Quad (a.k.a. 0x2395)
#define UTF16_JOT2                0x25E6        // Jot (a.k.a. 0x2218)
#define UTF16_CIRCLE2             0x26AA        // Circle (a.k.a. 0x25CB)
#define UTF16_LEFTCARETUNDERBAR2  0x2A7D        // Not more2 (a.k.a. 0x2264)
#define UTF16_RIGHTCARETUNDERBAR2 0x2A7E        // Not less2 (a.k.a. 0x2265)
#define UTF16_DIAMOND2            0x2B26        // Diamond2 (a.k.a. 0x22C4)
#define UTF16_DIAMOND3            0x25CA        // Diamond3 (a.k.a. 0x22C4)

#define UTF16_A_                  0xE036        // A underbar
#define UTF16_B_                  0xE037        // B underbar
#define UTF16_C_                  0xE038        // C underbar
#define UTF16_D_                  0xE039        // D underbar
#define UTF16_E_                  0xE03A        // E underbar
#define UTF16_F_                  0xE03B        // F underbar
#define UTF16_G_                  0xE03C        // G underbar
#define UTF16_H_                  0xE03D        // H underbar
#define UTF16_I_                  0xE03E        // I underbar
#define UTF16_J_                  0xE03F        // J underbar
#define UTF16_K_                  0xE040        // K underbar
#define UTF16_L_                  0xE041        // L underbar
#define UTF16_M_                  0xE042        // M underbar
#define UTF16_N_                  0xE043        // N underbar
#define UTF16_O_                  0xE044        // O underbar
#define UTF16_P_                  0xE045        // P underbar
#define UTF16_Q_                  0xE046        // Q underbar
#define UTF16_R_                  0xE047        // R underbar
#define UTF16_S_                  0xE048        // S underbar
#define UTF16_T_                  0xE049        // T underbar
#define UTF16_U_                  0xE04A        // U underbar
#define UTF16_V_                  0xE04B        // V underbar
#define UTF16_W_                  0xE04C        // W underbar
#define UTF16_X_                  0xE04D        // X underbar
#define UTF16_Y_                  0xE04E        // Y underbar
#define UTF16_Z_                  0xE04F        // Z underbar


// The corresponding wide string constants

// No keystroke equivalents for these as yet
#define WS_UTF16_PLUS_OR_MINUS       L"\x00B1"  // Plus or minus symbol
#define WS_UTF16_LEFTSINGLEQUOTE     L"\x2018"  // Left single quotation mark
#define WS_UTF16_RIGHTSINGLEQUOTE    L"\x2019"  // Right ...
#define WS_UTF16_HORIZELLIPSIS       L"\x2026"  // Horizontal ellipsis
#define WS_UTF16_EURO                L"\x20AC"  // Euro
#define WS_UTF16_LEFTRIGHTARROWS     L"\x2194"  // Left-right arrows
#define WS_UTF16_UPDOWNARROWS        L"\x2195"  // Up-down arrows
#define WS_UTF16_CIRCLESLASH         L"\x2205"  //
#define WS_UTF16_CHECKMARK           L"\x221A"  // Check mark
#define WS_UTF16_DIERESISDOT         L"\x2235"  //
#define WS_UTF16_NAND                l"\x22BC"  //
#define WS_UTF16_NOR                 L"\x22BD"  //
#define WS_UTF16_IBEAM               L"\x2336"  //
#define WS_UTF16_QUADEQUAL           L"\x2338"  //
#define WS_UTF16_QUADDIVIDE          L"\x2339"  //
#define WS_UTF16_QUADDIAMOND         L"\x233A"  //
#define WS_UTF16_QUADJOT             L"\x233B"  //
#define WS_UTF16_QUADCIRCLE          L"\x233C"  //
#define WS_UTF16_CIRCLEJOT           L"\x233E"  //
#define WS_UTF16_QUADSLASH           L"\x2341"  //
#define WS_UTF16_QUADSLOPE           L"\x2342"  //
#define WS_UTF16_QUADLEFTCARET       L"\x2343"  //
#define WS_UTF16_QUADRIGHTCARET      L"\x2344"  //
#define WS_UTF16_QUADLEFTVANE        L"\x2345"  //
#define WS_UTF16_QUADRIGHTVANE       L"\x2346"  //
#define WS_UTF16_QUADLEFTARROW       L"\x2347"  //
#define WS_UTF16_QUADRIGHTARROW      L"\x2348"  //
#define WS_UTF16_UPTACKUNDERBAR      L"\x234A"  //
#define WS_UTF16_QUADDOWNCARET       L"\x234C"  //
#define WS_UTF16_QUADDELTA           L"\x234D"  //
#define WS_UTF16_UPVANE              L"\x234F"  //
#define WS_UTF16_QUADUPARROW         L"\x2350"  //
#define WS_UTF16_DOWNTACKOVERBAR     L"\x2351"  //
#define WS_UTF16_QUADUPCARET         L"\x2353"  //
#define WS_UTF16_QUADDEL             L"\x2354"  //
#define WS_UTF16_DOWNVANE            L"\x2356"  //
#define WS_UTF16_QUADDOWNARROW       L"\x2357"  //
#define WS_UTF16_QUOTEUNDERBAR       L"\x2358"  //
#define WS_UTF16_DIAMONDUNDERBAR     L"\x235A"  //
#define WS_UTF16_JOTUNDERBAR         L"\x235B"  //
#define WS_UTF16_CIRCLEUNDERBAR      L"\x235C"  //
#define WS_UTF16_QUADCOLON           L"\x2360"  // Variant operator
#define WS_UTF16_LEFTSHOESTILE       L"\x2367"  //
#define WS_UTF16_DIERESISRIGHTCARET  L"\x2369"  //
#define WS_UTF16_DELTILDE            L"\x236B"  // Del-tilde
#define WS_UTF16_SEMICOLONUNDERBAR   L"\x236E"  //
#define WS_UTF16_QUADNOTEQUAL        L"\x236F"  //
#define WS_UTF16_QUADQUERY           L"\x2370"  //
#define WS_UTF16_ALPHAUNDERBAR       L"\x2376"  //
#define WS_UTF16_EPSILONUNDERBAR     L"\x2377"  //
#define WS_UTF16_IOTAUNDERBAR        L"\x2378"  //
#define WS_UTF16_OMEGAUNDERBAR       L"\x2379"  //
#define WS_UTF16_LDC_LT_HORZ         L"\x2500"  // LDC light horizontal
#define WS_UTF16_LDC_LT_VERT         L"\x2502"  // LDC light vertical
#define WS_UTF16_LDC_LT_UL           L"\x250C"  // LDC light upper left
#define WS_UTF16_LDC_LT_UR           L"\x2510"  // LDC light upper right
#define WS_UTF16_LDC_LT_LL           L"\x2514"  // LDC light lower left
#define WS_UTF16_LDC_LT_LR           L"\x2518"  // LDC light lower right
#define WS_UTF16_LDC_LT_VERT_R       L"\x251C"  // LDC light vertical and right
#define WS_UTF16_LDC_LT_VERT_L       L"\x2524"  // LDC light vertical and left
#define WS_UTF16_LDC_LT_HORZ_D       L"\x252C"  // LDC light horizontal and down
#define WS_UTF16_LDC_LT_HORZ_U       L"\x2534"  // LDC light horizontal and up
#define WS_UTF16_LDC_LT_CROSS        L"\x253C"  // LDC light horizontal and vertical
#define WS_UTF16_LDC_DB_HORZ         L"\x2550"  // LDC double horizontal
#define WS_UTF16_LDC_DB_VERT         L"\x2551"  // LDC double vertical
#define WS_UTF16_LDC_DB_UL           L"\x2554"  // LDC double upper left
#define WS_UTF16_LDC_DB_UR           L"\x2557"  // LDC double upper right
#define WS_UTF16_LDC_DB_LL           L"\x255A"  // LDC double lower left
#define WS_UTF16_LDC_DB_LR           L"\x255D"  // LDC double lower right
#define WS_UTF16_LDC_DB_VERT_R       L"\x2560"  // LDC double vertical and right
#define WS_UTF16_LDC_DB_VERT_L       L"\x2563"  // LDC double vertical and left
#define WS_UTF16_LDC_DB_HORZ_D       L"\x2566"  // LDC double horizontal and down
#define WS_UTF16_LDC_DB_HORZ_U       L"\x2569"  // LDC double horizontal and up
#define WS_UTF16_LDC_DB_CROSS        L"\x256C"  // LDC double horizontal and vertical
#define WS_UTF16_REPLACEMENTCHAR     L"\xFFFD"  // Replacement char for unknown

// The alphabet, unshifted
#define WS_UTF16_ALPHA               L"\x237A"  // Alt-'a' - alpha
#define WS_UTF16_UPTACK              L"\x22A5"  // Alt-'b' - base
#define WS_UTF16_UPSHOE              L"\x2229"  // Alt-'c' - intersection
#define WS_UTF16_DOWNSTILE           L"\x230A"  // Alt-'d' - floor
#define WS_UTF16_EPSILON             L"\x220A"  // Alt-'e' - epsilon
#define WS_UTF16_INFINITY            L"\x221E"  // Alt-'f' - infinity
#define WS_UTF16_DEL                 L"\x2207"  // Alt-'g' - del
#define WS_UTF16_DELTA               L"\x2206"  // Alt-'h' - delta
#define WS_UTF16_IOTA                L"\x2373"  // Alt-'i' - iota
#define WS_UTF16_JOT                 L"\x2218"  // Alt-'j' - Compose operator (jot)
#define WS_UTF16_APOSTROPHE          L"\x0027"  // Alt-'k' - single quote
#define WS_UTF16_QUAD                L"\x2395"  // Alt-'l' - quad (not \x25AF)
#define WS_UTF16_DOWNSHOESTILE       L"\x2366"  // Alt-'m' - down-shoe-stile
#define WS_UTF16_DOWNTACK            L"\x22A4"  // Alt-'n' - representation
#define WS_UTF16_CIRCLE              L"\x25CB"  // Alt-'o' - circle
#define WS_UTF16_PI                  L"\x03C0"  // Alt-'p' - pi
#define WS_UTF16_QUERY               L"\x003F"  // Alt-'q' - question-mark
#define WS_UTF16_RHO                 L"\x2374"  // Alt-'r' - rho
#define WS_UTF16_UPSTILE             L"\x2308"  // Alt-'s' - ceiling
#define WS_UTF16_TILDE               L"\x223C"  // Alt-'t' - tilde
#define WS_UTF16_DOWNARROW           L"\x2193"  // Alt-'u' - down arrow
#define WS_UTF16_DOWNSHOE            L"\x222A"  // Alt-'v' - union
#define WS_UTF16_OMEGA               L"\x2375"  // Alt-'w' - omega
#define WS_UTF16_RIGHTSHOE           L"\x2283"  // Alt-'x' - disclose
#define WS_UTF16_UPARROW             L"\x2191"  // Alt-'y' - up arrow
#define WS_UTF16_LEFTSHOE            L"\x2282"  // Alt-'z' - enclose

// The alphabet, shifted
//efine WS_UTF16_                               // Alt-'A' - (none)
//efine WS_UTF16_                               // Alt-'B' - (none)
//efine WS_UTF16_                               // Alt-'C' - (none)
//efine WS_UTF16_                               // Alt-'D' - (none)
#define WS_UTF16_EPSILONUNDERBAR     L"\x2377"  // Alt-'E' - epsilon-underbar
//efine WS_UTF16_                               // Alt-'F' - (none)
#define WS_UTF16_DIERESISDEL         L"\x2362"  // Alt-'G' - Dual operator        (frog)
#define WS_UTF16_DELTAUNDERBAR       L"\x2359"  // Alt-'H' - delta-underbar
#define WS_UTF16_IOTAUNDERBAR        L"\x2378"  // Alt-'I' - iota-underbar
#define WS_UTF16_DIERESISJOT         L"\x2364"  // Alt-'J' - Rank operator        (hoot)
//efine WS_UTF16_                               // Alt-'K' - (none)
#define WS_UTF16_SQUAD               L"\x2337"  // Alt-'L' - squad
#define WS_UTF16_STILETILDE          L"\x236D"  // Alt-'M' - Partition operator   (dagger)
#define WS_UTF16_DIERESISDOWNTACK    L"\x2361"  // Alt-'N' - Convolution operator (snout)
#define WS_UTF16_DIERESISCIRCLE      L"\x2365"  // Alt-'O' - Composition operator (holler)
#define WS_UTF16_DIERESISSTAR        L"\x2363"  // Alt-'P' - Power operator       (sourpuss)
//efine WS_UTF16_                               // Alt-'Q' - (none)
#define WS_UTF16_ROOT                L"\x221A"  // Alt-'R' - Root
#define WS_UTF16_SECTION             L"\x00A7"  // Alt-'S' - Multiset Symmetric Difference (section)
#define WS_UTF16_DIERESISTILDE       L"\x2368"  // Alt-'T' - Commute operator     (frown)
//efine WS_UTF16_                               // Alt-'U' - (none)
//efine WS_UTF16_                               // Alt-'V' - (none)
//efine WS_UTF16_                               // Alt-'W' - (none)
#define WS_UTF16_RIGHTSHOEUNDERBAR   L"\x2287"  // Alt-'X' - Superset
//efine WS_UTF16_                               // Alt-'Y' - (none)
#define WS_UTF16_LEFTSHOEUNDERBAR    L"\x2286"  // Alt-'Z' - Subset

// Top row, unshifted
#define WS_UTF16_DIAMOND             L"\x22C4"  // Alt-'`' - diamond (not \x25CA)
#define WS_UTF16_DIERESIS            L"\x00A8"  // Alt-'1' - dieresis
#define WS_UTF16_OVERBAR             L"\x00AF"  // Alt-'2' - high minus
#define WS_UTF16_LEFTCARET           L"\x003C"  // Alt-'3' - less
#define WS_UTF16_LEFTCARETUNDERBAR   L"\x2264"  // Alt-'4' - not more
#define WS_UTF16_EQUAL               L"\x003D"  // Alt-'5' - equal
#define WS_UTF16_RIGHTCARETUNDERBAR  L"\x2265"  // Alt-'6' - not less
#define WS_UTF16_RIGHTCARET          L"\x003E"  // Alt-'7' - more
#define WS_UTF16_NOTEQUAL            L"\x2260"  // Alt-'8' - not equal
#define WS_UTF16_DOWNCARET           L"\x2228"  // Alt-'9' - or
#define WS_UTF16_UPCARET             L"\x2227"  // Alt-'0' - and (a.k.a. \x005E)
#define WS_UTF16_TIMES               L"\x00D7"  // Alt-'-' - times
#define WS_UTF16_COLONBAR            L"\x00F7"  // Alt-'=' - divide

// Top row, shifted
#define WS_UTF16_COMMABAR            L"\x236A"  // Alt-'~' - comma-bar
#define WS_UTF16_EQUALUNDERBAR       L"\x2261"  // Alt-'!' - match
#define WS_UTF16_NOTEQUALUNDERBAR    L"\x2262"  // Alt-'@' - mismatch
#define WS_UTF16_DELSTILE            L"\x2352"  // Alt-'#' - grade-down
#define WS_UTF16_DELTASTILE          L"\x234B"  // Alt-'$' - grade-up
#define WS_UTF16_CIRCLESTILE         L"\x233D"  // Alt-'%' - rotate
#define WS_UTF16_CIRCLESLOPE         L"\x2349"  // Alt-'^' - transpose
#define WS_UTF16_CIRCLEBAR           L"\x2296"  // Alt-'&' - circle-bar
#define WS_UTF16_CIRCLESTAR          L"\x235F"  // Alt-'*' - log
#define WS_UTF16_DOWNCARETTILDE      L"\x2371"  // Alt-'(' - nor
#define WS_UTF16_UPCARETTILDE        L"\x2372"  // Alt-')' - nand
#define WS_UTF16_VARIANT             L"\x2360"  // Alt-'_' - variant
#define WS_UTF16_DOMINO              L"\x2339"  // Alt-'+' - domino

// Second row, unshifted
#define WS_UTF16_LEFTARROW           L"\x2190"  // Alt-'[' - left arrow
#define WS_UTF16_RIGHTARROW          L"\x2192"  // Alt-']' - right arrow
#define WS_UTF16_LEFTTACK            L"\x22A3"  // Alt-'\' - left tack

// Second row, shifted
#define WS_UTF16_QUOTEQUAD           L"\x235E"  // Alt-'{' - quote-quad
#define WS_UTF16_ZILDE               L"\x236C"  // Alt-'}' - zilde
#define WS_UTF16_RIGHTTACK           L"\x22A2"  // Alt-'|' - right tack

// Third row, unshifted
#define WS_UTF16_UPTACKJOT           L"\x234E"  // Alt-';' - execute
#define WS_UTF16_DOWNTACKJOT         L"\x2355"  // Alt-'\''- format

// Third row, shifted
//efine WS_UTF16_                               // Alt-':' - (none)
//efine WS_UTF16_                               // Alt-'"' - (none)

// Fourth row, unshifted
#define WS_UTF16_LAMP                L"\x235D"  // Alt-',' - comment
#define WS_UTF16_SLOPEBAR            L"\x2340"  // Alt-'.' - slope-bar
#define WS_UTF16_SLASHBAR            L"\x233F"  // Alt-'/' - slash-bar

// Fourth row, shifted
//efine WS_UTF16_                               // Alt-'<' - (none)
//efine WS_UTF16_                               // Alt-'>' - (none)
#define WS_UTF16_CIRCLEMIDDLEDOT     L"\x2299"  // Alt-'?' - circle-middle-dot

// Non-Alt key equivalents
#define WS_UTF16_JOTDOT              L"\x0001"  // Pseudo-symbol for {jot}{dot} digraph
#define WS_UTF16_QUOTEDOT            L"\x0021"  // Quote dot
#define WS_UTF16_DOUBLEQUOTE         L"\x0022"  // Double quote
#define WS_UTF16_NUMBER              L"\x0023"  // Number
#define WS_UTF16_DOLLAR              L"\x0024"  // Dollar
#define WS_UTF16_PERCENT             L"\x0025"  // Percent
#define WS_UTF16_AMPERSAND           L"\x0026"  // Ampersand
#define WS_UTF16_SINGLEQUOTE         L"\x0027"  // Single quote (a.k.a. Apostrophe)
#define WS_UTF16_LEFTPAREN           L"\x0028"  // Left paren
#define WS_UTF16_RIGHTPAREN          L"\x0029"  // Right paren
#define WS_UTF16_STAR                L"\x002A"  // Star
#define WS_UTF16_PLUS                L"\x002B"  // Plus
#define WS_UTF16_COMMA               L"\x002C"  // Comma
#define WS_UTF16_BAR                 L"\x002D"  // Bar
#define WS_UTF16_DOT                 L"\x002E"  // Dot
#define WS_UTF16_SLASH               L"\x002F"  // Slash
#define WS_UTF16_COLON               L"\x003A"  // Colon
#define WS_UTF16_SEMICOLON           L"\x003B"  // Semicolon
#define WS_UTF16_ATSIGN              L"\x0040"  // At sign
#define WS_UTF16_LEFTBRACKET         L"\x005B"  // Left bracket
#define WS_UTF16_SLOPE               L"\x005C"  // Slope
#define WS_UTF16_RIGHTBRACKET        L"\x005D"  // Right bracket
#define WS_UTF16_CIRCUMFLEX          L"\x005E"  // Circumflex (a.k.a. \x2227)
#define WS_UTF16_UNDERBAR            L"\x005F"  // Underbar
#define WS_UTF16_BACKTICK            L"\x0060"  // Backtick
#define WS_UTF16_LEFTBRACE           L"\x007B"  // Left brace
#define WS_UTF16_STILE2              L"\x007C"  // Stile (a.k.a. \x2223)
#define WS_UTF16_RIGHTBRACE          L"\x007D"  // Right brace
#define WS_UTF16_TILDE2              L"\x007E"  // Tilde (a.k.a. \x223C)
#define WS_UTF16_POUND               L"\x00A3"  // Pound
#define WS_UTF16_CURRENCY            L"\x00A4"  // Currency
#define WS_UTF16_COPYRIGHT           L"\x00A9"  // Copyright
#define WS_UTF16_ACUTE               L"\x00B4"  // Acute accent
#define WS_UTF16_MICRO               L"\x00B5"  // Micro
#define WS_UTF16_ONE_FOURTH          L"\x00BC"  // One fourth
#define WS_UTF16_ONE_HALF            L"\x00BD"  // One half
#define WS_UTF16_THREE_FOURTH        L"\x00BE"  // Three fourth
#define WS_UTF16_CAP_A_RING          L"\x00C5"  // Capital A w/ring
#define WS_UTF16_CAP_AE              L"\x00C6"  // Capital AE
#define WS_UTF16_CAP_O_STROKE        L"\x00D8"  // Capital O w/stroke
#define WS_UTF16_LOW_A_RING          L"\x00E5"  // Lower   A w/ring
#define WS_UTF16_LOW_AE              L"\x00E6"  // Lower   ae
#define WS_UTF16_LOW_O_STROKE        L"\x00F8"  // Lower   o w/stroke
#define WS_UTF16_ALPHA2              L"\x03B1"  // Greek small letter alpha
#define WS_UTF16_OMEGA2              L"\x03C9"  // ...                omega
#define WS_UTF16_BAR2                L"\x2212"  // Bar (a.k.a. 0x002D)
#define WS_UTF16_STILE               L"\x2223"  // Stile
#define WS_UTF16_STAR2               L"\x22C6"  // Star (a.k.a. 002A)
#define WS_UTF16_QUAD2               L"\x25AF"  // Quad (a.k.a. 0x2395)
#define WS_UTF16_JOT2                L"\x25E6"  // Jot (a.k.a. 0x2218)
#define WS_UTF16_CIRCLE2             L"\x26AA"  // Circle (a.k.a. 0x25CB)
#define WS_UTF16_LEFTCARETUNDERBAR2  L"\x2A7D"  // Not more2 (a.k.a. 0x2264)
#define WS_UTF16_RIGHTCARETUNDERBAR2 L"\x2A7E"  // Not less2 (a.k.a. 0x2265)
#define WS_UTF16_DIAMOND2            L"\x2B26"  // Diamond2 (a.k.a. 0x22C4)
#define WS_UTF16_DIAMOND3            L"\x25CA"  // Diamond3 (a.k.a. 0x22C4)

#define WS_UTF16_A_                  L"\xE036"  // A underbar
#define WS_UTF16_B_                  L"\xE037"  // B underbar
#define WS_UTF16_C_                  L"\xE038"  // C underbar
#define WS_UTF16_D_                  L"\xE039"  // D underbar
#define WS_UTF16_E_                  L"\xE03A"  // E underbar
#define WS_UTF16_F_                  L"\xE03B"  // F underbar
#define WS_UTF16_G_                  L"\xE03C"  // G underbar
#define WS_UTF16_H_                  L"\xE03D"  // H underbar
#define WS_UTF16_I_                  L"\xE03E"  // I underbar
#define WS_UTF16_J_                  L"\xE03F"  // J underbar
#define WS_UTF16_K_                  L"\xE040"  // K underbar
#define WS_UTF16_L_                  L"\xE041"  // L underbar
#define WS_UTF16_M_                  L"\xE042"  // M underbar
#define WS_UTF16_N_                  L"\xE043"  // N underbar
#define WS_UTF16_O_                  L"\xE044"  // O underbar
#define WS_UTF16_P_                  L"\xE045"  // P underbar
#define WS_UTF16_Q_                  L"\xE046"  // Q underbar
#define WS_UTF16_R_                  L"\xE047"  // R underbar
#define WS_UTF16_S_                  L"\xE048"  // S underbar
#define WS_UTF16_T_                  L"\xE049"  // T underbar
#define WS_UTF16_U_                  L"\xE04A"  // U underbar
#define WS_UTF16_V_                  L"\xE04B"  // V underbar
#define WS_UTF16_W_                  L"\xE04C"  // W underbar
#define WS_UTF16_X_                  L"\xE04D"  // X underbar
#define WS_UTF16_Y_                  L"\xE04E"  // Y underbar
#define WS_UTF16_Z_                  L"\xE04F"  // Z underbar

#define WS_UTF16_LFTOPER    WS_UTF16_ALPHA WS_UTF16_ALPHA
#define WS_UTF16_DELDEL     WS_UTF16_DEL   WS_UTF16_DEL
#define WS_UTF16_RHTOPER    WS_UTF16_OMEGA WS_UTF16_OMEGA

#endif

//***************************************************************************
//  End of File: Unicode.h
//***************************************************************************
