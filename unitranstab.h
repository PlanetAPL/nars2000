//****************************************************************************
//  NARS2000 -- Unicode Translation Table
//****************************************************************************

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

// Translation table for converting chars from
//   various APL systems to NARS.

#define NA      UTF16_REPLACEMENTCHAR
#define SAME    0xFFFE

WCHAR uniTransTab[][UNITRANS_LENGTH] =
{// APL+Win    ISO     APL2  Dyalog  PC3270  Normal  NARS
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   NA  , UTF16_REPLACEMENT0000     },   // 0xFFFF               Missing from ISO charset
    {  NA  , 0x00C3,   NA  ,   NA  , 0x00A0,   NA  , UTF16_A_                  },   // A underbar
    {  NA  , 0x00C4,   NA  ,   NA  , 0x00E2,   NA  , UTF16_B_                  },   // B underbar
    {  NA  , 0x00C5,   NA  ,   NA  , 0x00E4,   NA  , UTF16_C_                  },   // C underbar
    {  NA  , 0x00C6,   NA  ,   NA  , 0x00E0,   NA  , UTF16_D_                  },   // D underbar
    {  NA  , 0x00C7,   NA  ,   NA  , 0x00E1,   NA  , UTF16_E_                  },   // E underbar
    {  NA  , 0x00C8,   NA  ,   NA  , 0x00E3,   NA  , UTF16_F_                  },   // F underbar
    {  NA  , 0x00C9,   NA  ,   NA  , 0x00E5,   NA  , UTF16_G_                  },   // G underbar
    {  NA  , 0x00CA,   NA  ,   NA  , 0x00E7,   NA  , UTF16_H_                  },   // H underbar
    {  NA  , 0x00CB,   NA  ,   NA  , 0x00F1,   NA  , UTF16_I_                  },   // I underbar
    {  NA  , 0x00CC,   NA  ,   NA  , 0x00E9,   NA  , UTF16_J_                  },   // J underbar
    {  NA  , 0x00CD,   NA  ,   NA  , 0x00EA,   NA  , UTF16_K_                  },   // K underbar
    {  NA  , 0x00CE,   NA  ,   NA  , 0x00EB,   NA  , UTF16_L_                  },   // L underbar
    {  NA  , 0x00CF,   NA  ,   NA  , 0x00E8,   NA  , UTF16_M_                  },   // M underbar
    {  NA  , 0x00D0,   NA  ,   NA  , 0x00ED,   NA  , UTF16_N_                  },   // N underbar
    {  NA  , 0x00D1,   NA  ,   NA  , 0x00EE,   NA  , UTF16_O_                  },   // O underbar
    {  NA  , 0x00D2,   NA  ,   NA  , 0x00EF,   NA  , UTF16_P_                  },   // P underbar
    {  NA  , 0x00D3,   NA  ,   NA  , 0x00EC,   NA  , UTF16_Q_                  },   // Q underbar
    {  NA  , 0x00D4,   NA  ,   NA  , 0x00DF,   NA  , UTF16_R_                  },   // R underbar
    {  NA  , 0x00D5,   NA  ,   NA  , 0x00C2,   NA  , UTF16_S_                  },   // S underbar
    {  NA  , 0x00D6,   NA  ,   NA  , 0x00C4,   NA  , UTF16_T_                  },   // T underbar
    {  NA  , 0x00D7,   NA  ,   NA  , 0x00C0,   NA  , UTF16_U_                  },   // U underbar
    {  NA  , 0x00D8,   NA  ,   NA  , 0x00C1,   NA  , UTF16_V_                  },   // V underbar
    {  NA  , 0x00D9,   NA  ,   NA  , 0x00C3,   NA  , UTF16_W_                  },   // W underbar
    {  NA  , 0x00DA,   NA  ,   NA  , 0x00C5,   NA  , UTF16_X_                  },   // X underbar
    {  NA  , 0x00DB,   NA  ,   NA  , 0x00C7,   NA  , UTF16_Y_                  },   // Y underbar
    {  NA  , 0x00DC,   NA  ,   NA  , 0x00D1,   NA  , UTF16_Z_                  },   // Z underbar
//  APL+Win    ISO     APL2  Dyalog  PC3270  Normal  NARS
    {0x00B8, 0x00A9,  SAME ,  SAME , 0x005E,   NA  , UTF16_ALPHA               },   // Alpha    (0x237A)
    {0x00B8, 0x00A9, 0x237A, 0x237A, 0x005E,   NA  , UTF16_ALPHA2              },   // Alpha2   (0x03B1)
    {0x002D, 0x002D, 0x002D, 0x002D, 0x002D,   NA  , UTF16_BAR                 },   // Bar      (0x002D)    These three *MUST* be defined first so that
    {0x0152, 0x00B2, 0x2395, 0x2395, 0x00B0,   NA  , UTF16_QUAD                },   // Quad     (0x2395)      the translation from APL2/Dyalog to NARS
    {0x002A, 0x002A, 0x002A, 0x002A, 0x002A,   NA  , UTF16_STAR                },   // Star     (0x002A)      is correct.
    {0x002D, 0x002D, 0x002D, 0x002D, 0x002D,   NA  , UTF16_BAR2                },   // Bar2     (0x2212)
    {0x0152, 0x00B2, 0x2395, 0x2395, 0x00B0,   NA  , UTF16_QUAD2               },   // Quad2    (0x25AF)
    {0x002A, 0x002A, 0x002A, 0x002A, 0x002A,   NA  , UTF16_STAR2               },   // Star2    (0x22C6)
    {0x00B1, 0x00B4,  SAME ,  SAME , 0x00B8,   NA  , UTF16_CIRCLE              },   // Circle   (0x25CB)
    {0x00B1, 0x00B4, 0x25CB, 0x25CB, 0x00B8,   NA  , UTF16_CIRCLE2             },   // Circle2  (0x26AA)
    {0x00B4, 0x00EE,  SAME ,  SAME , 0x00D2,   NA  , UTF16_CIRCLEBAR           },   // Circle bar
    {0x00B3, 0x00ED,  SAME ,  SAME , 0x00F5,   NA  , UTF16_CIRCLESLOPE         },   // Circle slope
    {0x00B5, 0x00EB,  SAME ,  SAME , 0x00D9,   NA  , UTF16_CIRCLESTAR          },   // Circle star
    {0x00B2, 0x00EC,  SAME ,  SAME , 0x00F2,   NA  , UTF16_CIRCLESTILE         },   // Circle stile
    {0x00F7, 0x00A6,  SAME ,  SAME , 0x00BD,   NA  , UTF16_COLONBAR            },   // Colon bar
    {0x00AE, 0x00EF,  SAME ,  SAME ,   NA  ,   NA  , UTF16_COMMABAR            },   // Commabar
    {0x2019, 0x00AE,  SAME ,  SAME , 0x005B,   NA  , UTF16_DEL                 },   // Del
    {0x201D, 0x00E7,  SAME ,  SAME , 0x00FC,   NA  , UTF16_DELSTILE            },   // Del stile
    {0x2018, 0x00AF,  SAME ,  SAME , 0x005D,   NA  , UTF16_DELTA               },   // Delta
    {0x201C, 0x00E6,  SAME ,  SAME , 0x00F9,   NA  , UTF16_DELTASTILE          },   // Delta stile
    {0x008F, 0x00C2,  SAME ,  SAME , 0x00DC,   NA  , UTF16_DELTAUNDERBAR       },   // Delta underbar
    {0x0090, 0x00E8,  SAME ,  SAME , 0x00DB,   NA  , UTF16_DELTILDE            },   // Del tilde
    {0x00AA, 0x00C0,  SAME ,  SAME , 0x00F8,   NA  , UTF16_DIAMOND             },   // Diamond  (0x22C4)
    {0x00AA, 0x00C0, 0x22C4, 0x22C4, 0x00F8,   NA  , UTF16_DIAMOND2            },   // Diamond2 (0x25C7)
    {0x00AA, 0x00C0, 0x22C4, 0x22C4, 0x00F8,   NA  , UTF16_DIAMOND3            },   // Diamond3 (0x25CA)
    {0x00AA, 0x00C0, 0x22C4, 0x22C4, 0x00F8,   NA  , UTF16_DIAMOND4            },   // Diamond4 (0x2B26)
    {0x00A8, 0x00A1,  SAME ,  SAME , 0x00CA,   NA  , UTF16_DIERESIS            },   // Dieresis
    {  NA  ,   NA  ,   NA  ,   NA  , 0x00D6,   NA  , UTF16_DIERESISDOT         },   // Dieresis dot
    {0x00F0, 0x00F8,   NA  ,   NA  ,   NA  ,   NA  , UTF16_DIERESISJOT         },   // Dieresis jot
    {0x00FE, 0x00A1,   NA  ,   NA  ,   NA  ,   NA  , UTF16_DIERESISTILDE       },   // Dieresis-tilde
    {0x017D, 0x00E1,  SAME ,  SAME , 0x00D3,   NA  , UTF16_DOMINO              },   // Domino
    { SAME ,  SAME , 0x001E,  SAME ,  SAME ,   NA  , UTF16_DOT                 },   // Dot
    {0x2021, 0x00B7,  SAME ,  SAME , 0x00BB,   NA  , UTF16_DOWNARROW           },   // Down arrow
    {0x0178, 0x00A3,  SAME ,  SAME , 0x00CC,   NA  , UTF16_DOWNCARET           },   // Down caret
    {0x2039, 0x00E9,  SAME ,  SAME , 0x00F4,   NA  , UTF16_DOWNCARETTILDE      },   // Downcaret tilde (0x2371)
    {0x2039, 0x00E9, 0x2371, 0x2371, 0x00F4,   NA  , UTF16_NOR                 },   // Nor             (0x22BD)
    {0x017E, 0x00B8,  SAME ,  SAME , 0x00BF,   NA  , UTF16_DOWNSHOE            },   // Down shoe
    {0x02DC, 0x00AC,  SAME ,  SAME , 0x00FE,   NA  , UTF16_DOWNSTILE           },   // Downstile
//  APL+Win    ISO     APL2  Dyalog  PC3270  Normal  NARS
    {0x201A, 0x00B3,  SAME ,  SAME , 0x00AF,   NA  , UTF16_DOWNTACK            },   // Down tack
    {0x2022, 0x00E0,  SAME ,  SAME , 0x00D5,   NA  , UTF16_DOWNTACKJOT         },   // Down tack jot
    {0x00B9, 0x00AD,  SAME ,  SAME , 0x00A3,   NA  , UTF16_EPSILON             },   // Epsilon
    {0x00BA,  SAME ,  SAME ,  SAME , 0x00CD,   NA  , UTF16_EPSILONUNDERBAR     },   // Epsilon underbar     Missing from ISO charset
    {0x00AD, 0x00F4,  SAME ,  SAME , 0x005C,   NA  , UTF16_EQUALUNDERBAR       },   // Equal underbar
    { SAME ,   NA  ,  SAME ,  SAME ,   NA  ,   NA  , UTF16_EURO                },   // Euro
    {  NA  ,   NA  ,   NA  ,   NA  , 0x00B9,   NA  , UTF16_IBEAM               },   // I-beam
    {  NA  ,   NA  ,   NA  ,   NA  , 0x00C8,   NA  , UTF16_IOTAUNDERBAR        },   // Iota underbar
    {0x00BC, 0x00B0,  SAME ,  SAME , 0x00A5,   NA  , UTF16_IOTA                },   // Iota
    {0x00B0, 0x00B1,  SAME ,  SAME , 0x00AE,   NA  , UTF16_JOT                 },   // Jot  (0x2218)
    {0x00B0, 0x00B1, 0x2218, 0x2218, 0x00AE,   NA  , UTF16_JOT2                },   // Jot2 (0x25E6)
    {0x00A9, 0x00DD,  SAME ,  SAME , 0x00FF,   NA  , UTF16_LAMP                },   // Lamp
    { SAME ,  SAME ,  SAME ,  SAME , 0x00DD,   NA  , UTF16_LEFTBRACKET         },   // Left bracket
    {0x201E, 0x00BC,  SAME ,  SAME , 0x00A4,   NA  , UTF16_LEFTARROW           },   // Left arrow
    {0x02C6, 0x00A2,  SAME ,  SAME , 0x00F0,   NA  , UTF16_LEFTCARETUNDERBAR   },   // Left caret underbar  (0x2264)
    {0x02C6, 0x00A2, 0x2264, 0x2264, 0x00F0,   NA  , UTF16_LEFTCARETUNDERBAR2  },   // Left caret underbar2 (0x2A7D)
    {0x203A, 0x00BB,  SAME ,  SAME , 0x00BA,   NA  , UTF16_LEFTSHOE            },   // Left shoe
    {0x0081, 0x00C1,  SAME ,  SAME , 0x00CF,   NA  , UTF16_LEFTTACK            },   // Left tack
    {0x00AC, 0x00A5,  SAME ,  SAME , 0x00B4,   NA  , UTF16_NOTEQUAL            },   // Not equal
    {0x00BE, 0x00B9,  SAME ,  SAME , 0x00A9,   NA  , UTF16_OMEGA               },   // Omega    (0x2375)
    {0x00BE, 0x00B9, 0x2375, 0x2375, 0x00A9,   NA  , UTF16_OMEGA2              },   // Omega2   (0x03C9)
    {0x00AF, 0x00A8,  SAME ,  SAME , 0x00B5,   NA  , UTF16_OVERBAR             },   // Overbar
////{0x00A7,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_PARA                },   // Section
    {0x0152, 0x00B2,  SAME ,  SAME , 0x00B0,   NA  , UTF16_QUAD                },   // Quad
    {  NA  ,   NA  ,   NA  ,   NA  , 0x00CB,   NA  , UTF16_QUADJOT             },   // Quad jot
    {  NA  ,   NA  ,   NA  ,   NA  , 0x00F3,   NA  , UTF16_QUADSLOPE           },   // Quad slope
    {  NA  ,   NA  ,   NA  ,   NA  , 0x00FB,   NA  , UTF16_QUOTEDOT            },   // Quote dot
    {0x008D, 0x00E4,  SAME ,  SAME , 0x00FA,   NA  , UTF16_QUOTEQUAD           },   // Quote-quad
    { SAME ,  SAME ,  SAME ,  SAME , 0x00A8,   NA  , UTF16_RIGHTBRACKET        },   // Right bracket
    {0x00BD, 0x00B5, 0x03C1,  SAME , 0x00B7,   NA  , UTF16_RHO                 },   // Rho
//  APL+Win    ISO     APL2  Dyalog  PC3270  Normal  NARS
    {0x2026, 0x00BE,  SAME ,  SAME , 0x00B1,   NA  , UTF16_RIGHTARROW          },   // Right arrow
    {0x2030, 0x00BF,  SAME ,  SAME , 0x00DE,   NA  , UTF16_RIGHTCARETUNDERBAR  },   // Right caret underbar  (0x2265)
    {0x2030, 0x00BF, 0x2265, 0x2265, 0x00DE,   NA  , UTF16_RIGHTCARETUNDERBAR2 },   // Right caret underbar2 (0x2A7E)
    {0x0153, 0x00BA,  SAME ,  SAME , 0x00AA,   NA  , UTF16_RIGHTSHOE           },   // Right shoe
    {0x00A4, 0x00BD,  SAME ,  SAME , 0x00CE,   NA  , UTF16_RIGHTTACK           },   // Right tack
    { SAME ,  SAME ,  SAME ,  SAME ,  SAME ,  SAME , UTF16_SINGLEQUOTE         },   // Left single quotation mark
    {0x2018, 0x2018, 0x2018, 0x2018,  SAME , 0x2018, UTF16_SINGLEQUOTE         },   // Left ...
    {0x2019, 0x2019, 0x2019, 0x2019,  SAME , 0x2019, UTF16_SINGLEQUOTE         },   // Right ...
    {0x0161, 0x00F0,  SAME ,  SAME , 0x00B2,   NA  , UTF16_SLASHBAR            },   // Slash bar
    { SAME ,  SAME ,  SAME ,  SAME , 0x00BC,   NA  , UTF16_SLOPE               },   // Slope
    {0x2122, 0x00F1,  SAME ,  SAME , 0x00D4,   NA  , UTF16_SLOPEBAR            },   // slope-bar
    {0x00DE,  SAME ,  SAME ,  SAME , 0x00F6,   NA  , UTF16_SQUAD               },   // Squad                Missing from ISO charset
    { SAME ,  SAME ,  SAME ,  SAME , 0x00D7,   NA  , UTF16_STILE2              },   // Stile2   (0x007C)
    {0x007C, 0x007C, 0x007C,  SAME , 0x00D7,   NA  , UTF16_STILE               },   // Stile    (0x2223)
    { SAME ,  SAME ,  SAME ,  SAME , 0x00D8,   NA  , UTF16_TILDE2              },   // Tilde2   (0x007E)
    {0x007E, 0x007E, 0x007E, 0x007E, 0x00D8,   NA  , UTF16_TILDE               },   // Tilde    (0x223C)
    {0x00D7, 0x00A7, 0x000D,  SAME , 0x00B6,   NA  , UTF16_TIMES               },   // Times
    {0x2020, 0x00F5,  SAME ,  SAME , 0x00AB,   NA  , UTF16_UPARROW             },   // Up arrow
    { SAME , 0x00A4,  SAME ,  SAME , 0x00C9,   NA  , UTF16_CIRCUMFLEX          },   // Circumflex (0x005E)
    {0x005E, 0x00A4, 0x005E,  SAME , 0x00C9,   NA  , UTF16_UPCARET             },   // Up caret   (0x2227)
    {0x0160, 0x00EA,  SAME ,  SAME , 0x00AD,   NA  , UTF16_UPCARETTILDE        },   // Upcaret tilde (0x2372)
    {0x0160, 0x00EA, 0x2372, 0x2372, 0x00AD,   NA  , UTF16_NAND                },   // Nand          (0x22BC)
    {0x009D, 0x00AB,  SAME ,  SAME , 0x00A1,   NA  , UTF16_UPSHOE              },   // Up shoe
    {0x2014, 0x00B6,  SAME ,  SAME , 0x00FD,   NA  , UTF16_UPSTILE             },   // Upstile
    {0x0192, 0x00AA,  SAME ,  SAME , 0x00D0,   NA  , UTF16_UPTACK              },   // Up tack
    {0x2013, 0x00DF,  SAME ,  SAME , 0x00DA,   NA  , UTF16_UPTACKJOT           },   // Up tack jot
    {  NA  ,   NA  ,   NA  ,  SAME ,   NA  ,   NA  , UTF16_VARIANT             },   // Variant
    {0x00D0,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_ZILDE               },   // Zilde                Missing from ISO charset

//  APL+Win    ISO     APL2  Dyalog  PC3270  Normal  NARS
//  {0x00E1,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // a acute
//  {0x00E5,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // a circle
//  {0x00E2,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // a circumflex
//  {0x00E0,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // a grave
//  {0x00E3,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // a tilde
//  {0x00E4,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // a umlaut
//  {0x00E6,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // ae dipthong
//  {0x00C1,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // A acute
//  {0x00C5,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // A circle
//  {0x00C2,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // A circumflex
//  {0x00C0,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // A grave
//  {0x00C3,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // A tilde
//  {0x00C4,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // A umlaut
//  {0x00C6,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // AE dipthong
//  {0x00E7,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // c cedilla
//  {0x00C7,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // C cedilla
//  {0x00E9,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // e acute
//  {0x00EA,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // e circumflex
//  {0x00E8,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // e grave
//  {0x00EB,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // e umlaut
//  {0x00C9,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // E acute
//  {0x00CA,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // E circumflex
//  {0x00C8,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // E grave
//  {0x00CB,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // E umlaut
//  {0x00ED,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // i acute
//  {0x00EE,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // i circumflex
//  APL+Win    ISO     APL2  Dyalog  PC3270  Normal  NARS
//  {0x00EC,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // i grave
//  {0x00EF,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // i umlaut
//  {0x00CD,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // I acute
//  {0x00CE,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // I circumflex
//  {0x00CC,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // I grave
//  {0x00CF,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // I umlaut
//  {0x00F1,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // n tilde
//  {0x00D1,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // N tilde
//  {0x00F3,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // o acute
//  {0x00F4,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // o circumflex
//  {0x00F2,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // o grave
//  {0x00F5,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // o tilde
//  {0x00F6,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // o umlaut
//  {0x00D3,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // O acute
//  {0x00D4,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // O circumflex
//  {0x00D2,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // O grave
//  {0x00D8,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // O slash
//  {0x00D5,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // O tilde
//  {0x00D6,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // O umlaut
//  {0x00FA,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // u acute
//  {0x00FB,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // u circumflex
//  {0x00F9,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // u grave
//  {0x00FC,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // u umlaut
//  {0x00DA,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // U acute
//  {0x00DB,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // U circumflex
//  {0x00D9,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // U grave
//  {0x00DC,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // U umlaut
//  {0x00FD,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // y acute
//  {0x00FF,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // y umlaut
//  {0x00DD,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // Y acute

//  APL+Win    ISO     APL2  Dyalog  PC3270  Normal  NARS
//  {0x00A6,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // broken stile
//  {0x00BF,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // upside down query
//  {0x00F8,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // 0 slash
//  {0x00A1,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // upside down shriek
//  {0x00AB,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // Left chevron
//  {0x00BB,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // right chevron
//  {0x00DF,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // Beta
//  {0x00A2,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // cents
//  {0x00A3,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // pounds
//  {0x00A5,   NA  ,   NA  ,   NA  ,   NA  ,   NA  , UTF16_                    },   // yen
};

#undef  NA

// The # rows in the above table
#define UNITRANS_NROWS  (sizeof (uniTransTab) / sizeof (uniTransTab[0]))


//***************************************************************************
//  End of File: unitranstab.h
//***************************************************************************
