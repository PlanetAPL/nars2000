<?php

// Define the array of conversions
$br2a = array (

'!'                     => 0x0021,
'shreik'                => 0x0021,
'factorial'             => 0x0021,

'#'                     => 0x0023,

'&'                     => 0x0026,

"'"                     => 0x0027,

'power'                 => 0x002A,
'star'                  => 0x002A,

'plus'                  => 0x002B,

'comma'                 => 0x002C,

'minus'                 => 0x002D,

'dot'                   => 0x002E,

'/'                     => 0x002F,
'compress'              => 0x002F,

'lessthan'              => 0x003C,

'='                     => 0x003D,
'equal'                 => 0x003D,

'morethan'              => 0x003E,

'random'                => 0x003F,
'query'                 => 0x003F,

'@'                     => 0x0040,

"\\"                    => 0x005C,
'expand'                => 0x005C,

'underbar'              => 0x005F,

'leftbrace'             => 0x007B,
'('                     => 0x007B,
'from'                  => 0x007B,

'|'                     => 0x007C,
'abs'                   => 0x007C,
'residue'               => 0x007C,
'stile'                 => 0x007C,
'mod'                   => 0x007C,
'magnitude'             => 0x007C,

'rightbrace'            => 0x007D,
')'                     => 0x007D,

'~'                     => 0x007E,
'not'                   => 0x007E,
'less'                  => 0x007E,
'without'               => 0x007E,

'house'                 => 0x007F,

'cent'                  => 0x00A2,
'cents'                 => 0x00A2,

'pounds'                => 0x00A3,

'yen'                   => 0x00A5,

'section'               => 0x00A7,

'each'                  => 0x00A8,
'dieresis'              => 0x00A8,
'with'                  => 0x00A8,

'neg'                   => 0x00AF,
'-'                     => 0x00AF,
'negative'              => 0x00AF,
'highminus'             => 0x00AF,
'overbar'               => 0x00AF,
'_'                     => 0x00AF,

'signum'                => 0x00D7,
'times'                 => 0x00D7,
'x'                     => 0x00D7,
'sign'                  => 0x00D7,
'*'                     => 0x00D7,
'direction'             => 0x00D7,
'multiply'              => 0x00D7,

'divide'                => 0x00F7,
'reciprocal'            => 0x00F7,
'%'                     => 0x00F7,

'dagger'                => 0x2020,
'doubledagger'          => 0x2021,

'peseta'                => 0x20A7,

'euro'                  => 0x20AC,

'pi'                    => 0x03C0,

'<-'                    => 0x2190,
'is'                    => 0x2190,
'gets'                  => 0x2190,
'assign'                => 0x2190,
'leftarrow'             => 0x2190,
'=.'                    => 0x2190,
'=:'                    => 0x2190,
':='                    => 0x2190,

'first'                 => 0x2191,
'take'                  => 0x2191,
'uparrow'               => 0x2191,
'mix'                   => 0x2191,

'->'                    => 0x2192,
'goto'                  => 0x2192,
'go'                    => 0x2192,
'branch'                => 0x2192,
'rightarrow'            => 0x2192,

'split'                 => 0x2193,
'drop'                  => 0x2193,
'downarrow'             => 0x2193,
'raze'                  => 0x2193,

'delta'                 => 0x2206,

'del'                   => 0x2207,
'nabla'                 => 0x2207,

'enlist'                => 0x220A,
'epsilon'               => 0x220A,
'memberof'              => 0x220A,
'membership'            => 0x220A,
'in'                    => 0x220A,
'member'                => 0x220A,
'e.'                    => 0x220A,

'jot'                   => 0x2218,
'null'                  => 0x2218,
'compose'               => 0x2218,

'root'                  => 0x221A,

'infinity'              => 0x221E,

'and'                   => 0x2227,
'^'                     => 0x2227,
'lcm'                   => 0x2227,
'upcaret'               => 0x2227,
'*.'                    => 0x2227,

'or'                    => 0x2228,
'gcd'                   => 0x2228,
'downcaret'             => 0x2228,
'+.'                    => 0x2228,

'upshoe'                => 0x2229,
'intersect'             => 0x2229,
'intersection'          => 0x2229,
'cap'                   => 0x2229,

'downshoe'              => 0x222A,
'union'                 => 0x222A,
'cup'                   => 0x222A,
'nub'                   => 0x222A,

'dieresisdot'           => 0x2235,
'dotdieresis'           => 0x2235,

'tilde'                 => 0x223C,

'/='                    => 0x2260,
'ne'                    => 0x2260,
'noteq'                 => 0x2260,
'notequal'              => 0x2260,
'unequal'               => 0x2260,
'nubsieve'              => 0x2260,
'=/'                    => 0x2260,
'~='                    => 0x2260,
'~:'                    => 0x2260,

'depth'                 => 0x2261,
'match'                 => 0x2261,
'equalunderscore'       => 0x2261,
'equalunderbar'         => 0x2261,
'equal_'                => 0x2261,
'=_'                    => 0x2261,
'-:'                    => 0x2261,

'notmatch'              => 0x2262,
'mismatch'              => 0x2262,
'~=_'                   => 0x2262,

'<='                    => 0x2264,
'le'                    => 0x2264,
'lteq'                  => 0x2264,
'notgreater'            => 0x2264,
'notmore'               => 0x2264,
'>:'                    => 0x2264,

'>='                    => 0x2265,
'ge'                    => 0x2265,
'gteq'                  => 0x2265,
'greq'                  => 0x2265,
'notless'               => 0x2265,
'>:'                    => 0x2265,

'enclose'               => 0x2282,
'subset'                => 0x2282,
'leftshoe'              => 0x2282,
'<'                     => 0x2282,

'disclose'              => 0x2283,
'pick'                  => 0x2283,
'superset'              => 0x2283,
'rightshoe'             => 0x2283,
'weakenclose'           => 0x2283,
'link'                  => 0x2283,
'>'                     => 0x2283,

'leftshoeunderbar'      => 0x2286,
'leftshoe_'             => 0x2286,

'rightshoeunderbar'     => 0x2287,
'rightshoe_'            => 0x2287,

'reversebar'            => 0x2296,
'rotatebar'             => 0x2296,
'circlebar'             => 0x2296,

'circlemiddledot'       => 0x2299,
'nullop'                => 0x2299,

'righttack'             => 0x22A2,
'dex'                   => 0x22A2,
'stop'                  => 0x22A2,
'['                     => 0x22A2,

'lefttack'              => 0x22A3,
'lev'                   => 0x22A3,
'pass'                  => 0x22A3,
']'                     => 0x22A3,

'represent'             => 0x22A4,
'encode'                => 0x22A4,
'top'                   => 0x22A4,
'downtack'              => 0x22A4,
'#:'                    => 0x22A4,

'basevalue'             => 0x22A5,
'decode'                => 0x22A5,
'base'                  => 0x22A5,
'uptack'                => 0x22A5,
'#.'                    => 0x22A5,

'diamond'               => 0x22C4,
'<>'                    => 0x22C4,

'ceiling'               => 0x2308,
'max'                   => 0x2308,
'upstile'               => 0x2308,
'greater'               => 0x2308,
'>.'                    => 0x2308,

'floor'                 => 0x230A,
'min'                   => 0x230A,
'downstile'             => 0x230A,
'lesser'                => 0x230A,
'<.'                    => 0x230A,

'ibeam'                 => 0x2336,

'squad'                 => 0x2337,
'squishquad'            => 0x2337,
'index'                 => 0x2337,

'quadequal'             => 0x2338,

'domino'                => 0x2339,
'quaddivde'             => 0x2339,
'%.'                    => 0x2339,

'quaddiamond'           => 0x233A,

'quadjot'               => 0x233B,

'quadcircle'            => 0x233C,

'reverse'               => 0x233D,
'rotate'                => 0x233D,
'circlestile'           => 0x233D,
'rev'                   => 0x233D,
'|.'                    => 0x233D,

'circlejot'             => 0x233E,

'slashbar'              => 0x233F,
'compressbar'           => 0x233F,
'reducebar'             => 0x233F,
'/-'                    => 0x233F,

'backslashbar'          => 0x2340,
'slopebar'              => 0x2340,
'expandbar'             => 0x2340,
'scanbar'               => 0x2340,
'\-'                    => 0x2340,

'quadslash'             => 0x2341,

'quadbackslash'         => 0x2342,
'quadslope'             => 0x2342,

'quadlessthan'          => 0x2343,

'quadgreaterthan'       => 0x2344,

'leftvane'              => 0x2345,

'rightvane'             => 0x2346,

'quadleft'              => 0x2347,

'quadright'             => 0x2348,

'circlebackslash'       => 0x2349,
'circleslope'           => 0x2349,
'transpose'             => 0x2349,
'|:'                    => 0x2349,

'decodeunderbar'        => 0x234A,
'decode_'               => 0x234A,

'gradeup'               => 0x234B,
'upgrade'               => 0x234B,
'deltastile'            => 0x234B,
'/:'                    => 0x234B,

'quador'                => 0x234C,

'quaddelta'             => 0x234D,

'execute'               => 0x234E,
'do'                    => 0x234E,
'basenull'              => 0x234E,
'eval'                  => 0x234E,
'uptackjot'             => 0x234E,
'".'                    => 0x234E,

'upwardsvane'           => 0x234F,

'quadup'                => 0x2350,

'encodeoverbar'         => 0x2351,

'gradedown'             => 0x2352,
'downgrade'             => 0x2352,
'delstile'              => 0x2352,
'\:'                    => 0x2352,

'quadand'               => 0x2353,

'quaddel'               => 0x2354,

'format'                => 0x2355,
'topnull'               => 0x2355,
'fmt'                   => 0x2355,
'thorn'                 => 0x2355,
'downtackjot'           => 0x2355,
'":'                    => 0x2355,

'downwardsvane'         => 0x2356,

'quaddown'              => 0x2357,

'quoteunderbar'         => 0x2358,
'quote_'                => 0x2358,

'deltaunderscore'       => 0x2359,
'deltaunderbar'         => 0x2359,
'delta_'                => 0x2359,

'diamondunderbar'       => 0x235A,
'diamond_'              => 0x235A,

'jotunderbar'           => 0x235B,
'jot_'                  => 0x235B,

'circleunderbar'        => 0x235C,
'circle_'               => 0x235C,

'lamp'                  => 0x235D,
'comment'               => 0x235D,
'capnull'               => 0x235D,
'rem'                   => 0x235D,
'nb.'                   => 0x235D,

'quotequad'             => 0x235E,

'ln'                    => 0x235F,
'log'                   => 0x235F,
'circlestar'            => 0x235F,
'^.'                    => 0x235F,

'quadcolon'             => 0x2360,
'variant'               => 0x2360,

'downtackdieresis'      => 0x2361,
'encodedieresis'        => 0x2361,
'convolution'           => 0x2361,
'snout'                 => 0x2361,

'deldieresis'           => 0x2362,
'dieresisdel'           => 0x2362,
'dual'                  => 0x2362,
'frog'                  => 0x2362,

'dieresisstar'          => 0x2363,
'stardieresis'          => 0x2363,
'sourpuss'              => 0x2363,

'dieresisjot'           => 0x2364,
'jotdieresis'           => 0x2364,
'on'                    => 0x2364,
'rank'                  => 0x2364,
'hoot'                  => 0x2364,
'cut'                   => 0x2364,
'"'                     => 0x2364,
'paw'                   => 0x2364,

'dieresiscircle'        => 0x2365,
'circledieresis'        => 0x2365,
'hoof'                  => 0x2365,
'upon'                  => 0x2365,
'holler'                => 0x2365,
'composition'           => 0x2365,

'downshoestile'         => 0x2366,
'multiset'              => 0x2366,

'leftshoestile'         => 0x2367,

'tildedieresis'         => 0x2368,
'commute'               => 0x2368,
'frown'                 => 0x2368,

'greaterthandieresis'   => 0x2369,

'commabar'              => 0x236A,
'catbar'                => 0x236A,
',-'                    => 0x236A,
'table'                 => 0x236A,

'deltilde'              => 0x236B,

'zilde'                 => 0x236C,
'0~'                    => 0x236C,

'stiletilde'            => 0x236D,

'semicolonunderbar'     => 0x236E,
'semicolon_'            => 0x236E,

'quadnotequal'          => 0x236F,

'quadquestion'          => 0x2370,

'nor'                   => 0x2371,
'+:'                    => 0x2371,

'nand'                  => 0x2372,
'~^'                    => 0x2372,
'*:'                    => 0x2372,

'iota'                  => 0x2373,
'indexof'               => 0x2373,
'i.'                    => 0x2373,

'rho'                   => 0x2374,
'shape'                 => 0x2374,
'reshape'               => 0x2374,
'$'                     => 0x2374,

'omega'                 => 0x2375,

'alphaunderbar'         => 0x2376,
'alpha_'                => 0x2376,

'find'                  => 0x2377,
'epsilonunderscore'     => 0x2377,
'epsilonunderbar'       => 0x2377,
'epsilon_'              => 0x2377,

'iotaunderscore'        => 0x2378,
'iotaunderbar'          => 0x2378,
'iota_'                 => 0x2378,

'omegaunderbar'         => 0x2379,
'omega_'                => 0x2379,

'alpha'                 => 0x237A,

'quad'                  => 0x2395,      // In recent Unicode spec
'box'                   => 0x2395,      // ...

'quad2'                 => 0x25AF,      // In Arial Unicode MS
'box2'                  => 0x25AF,      // ...

'pitimes'               => 0x25CB,
'circle'                => 0x25CB,
'o.'                    => 0x25CB,

'A_'                    => 0xE036,
'B_'                    => 0xE037,
'C_'                    => 0xE038,
'D_'                    => 0xE039,
'E_'                    => 0xE03A,
'F_'                    => 0xE03B,
'G_'                    => 0xE03C,
'H_'                    => 0xE03D,
'I_'                    => 0xE03E,
'J_'                    => 0xE03F,
'K_'                    => 0xE040,
'L_'                    => 0xE041,
'M_'                    => 0xE042,
'N_'                    => 0xE043,
'O_'                    => 0xE044,
'P_'                    => 0xE045,
'Q_'                    => 0xE046,
'R_'                    => 0xE047,
'S_'                    => 0xE048,
'T_'                    => 0xE049,
'U_'                    => 0xE04A,
'V_'                    => 0xE04B,
'W_'                    => 0xE04C,
'X_'                    => 0xE04D,
'Y_'                    => 0xE04E,
'Z_'                    => 0xE04F,
);

// Define the length of the longest keyword
$MaxKLen = 19;

?>
