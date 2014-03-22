//***************************************************************************
//  NARS2000 -- Prime Factoring Header
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


//***************************************************************************
//  Prime factoring and number theoretic functions
//***************************************************************************

typedef struct tagNTHPRIMESTR
{
    APLINT aplNthCnt,           // Nth prime count
           aplNthVal;           // Nth prime value
} NTHPRIMESTR;


typedef struct tagMEMTMP
{
    HGLOBAL  hGlbMem;               // 00:  Global memory handle
    LPAPLMPI lpMemOrg,              // 04:  Ptr to original base
             lpMemNxt;              // 08:  Ptr to current
    UINT     uNumEntry,             // 0C:  # valid entries
             uMaxEntry;             // 10:  # entries allocated
                                    // 14:  Length
} MEMTMP, *LPMEMTMP;


typedef struct tagPROCESSPRIME
{
    APLUINT uFcnIndex;              // 00:  Function index (see enum NUMTHEORY) ( 8 bytes)
    APLMPI  mpzDivisor,             // 08:  Result of divisor count/sum         (12 bytes)
            mpzTotient;             // 14:  Result of totient function          (12 bytes)
    UBOOL   bSquareFree;            // 20:  TRUE iff the number is square-free  ( 4 bytes)
    UINT    seed1,                  // 24:  Random seeds
            seed2;                  // 28:  ...
    LPUBOOL lpbCtrlBreak;           // 2C:  Ptr to Ctrl-Break flag
} PROCESSPRIME, *LPPROCESSPRIME;


#define PRECOMPUTED_PRIME_NEXT          100003                  // Next prime after PRECOMPUTED_PRIME_MAX
                                                                //   = min # not factorable by trial division
#define PRECOMPUTED_PRIME_NEXT2         10000600009L            // Square of PRECOMPUTED_PRIME_NEXT
#define PRECOMPUTED_PRIME_NEXT2_MP      {2, {0x54150BC9, 2}}    // PRECOMPUTED_PRIME_NEXT2 as mp_t
#define PRECOMPUTED_PRIME_NEXT2_LOG2    33                      // Floor Log2 of PRECOMPUTED_PRIME_NEXT2

// The Nth prime (in origin-0) in increments of 1E5
APLINT NthPrimeTab[]
#ifdef DEFINE_VALUES
//    xxx00000   xxx100000   xxx200000   xxx300000   xxx400000   xxx500000   xxx600000   xxx700000   xxx800000   xxx900000      // xxxd00000
 = {         2,    1299721,    2750161,    4256249,    5800139,    7368791,    8960467,   10570849,   12195263,   13834127,     //   0dxxxxx
      15485867,   17144507,   18815249,   20495861,   22182379,   23879539,   25582163,   27290311,   29005549,   30723773,     //   1dxxxxx
      32452867,   34186063,   35926309,   37667713,   39410869,   41161751,   42920209,   44680327,   46441223,   48210719,     //   2dxxxxx
      49979693,   51754999,   53533523,   55316939,   57099349,   58886033,   60678109,   62473049,   64268783,   66068923,     //   3dxxxxx
      67867979,   69672853,   71480063,   73289717,   75103543,   76918277,   78736487,   80556709,   82376243,   84200287,     //   4dxxxxx
      86028157,   87857533,   89687693,   91519081,   93354689,   95189093,   97026263,   98866931,  100711463,  102551573,     //   5dxxxxx
     104395303,  106244939,  108092983,  109938683,  111794899,  113648413,  115507919,  117363997,  119227013,  121086479,     //   6dxxxxx
     122949829,  124811189,  126673979,  128541817,  130408843,  132276713,  134151007,  136023763,  137896303,  139772291,     //   7dxxxxx
     141650963,  143522957,  145403471,  147281041,  149163299,  151048973,  152935963,  154819711,  156704131,  158594209,     //   8dxxxxx
     160481219,  162373339,  164263007,  166158649,  168048739,  169941223,  171834301,  173729893,  175628473,  177525769,     //   9dxxxxx
     179424691,  181327579,  183236569,  185133833,  187038041,  188943817,  190850581,  192761047,  194667089,  196576759,     //  10dxxxxx
     198491329,  200402017,  202313627,  204227479,  206141401,  208055047,  209968651,  211887457,  213808403,  215723341,     //  11dxxxxx
     217645199,  219566131,  221489561,  223407641,  225330881,  227254213,  229179877,  231104369,  233031229,  234961007,     //  12dxxxxx
     236887699,  238810207,  240737963,  242673449,  244605377,  246534487,  248467649,  250399343,  252334751,  254269523,     //  13dxxxxx
     256203221,  258142009,  260079361,  262016011,  263956243,  265892021,  267836671,  269778617,  271723979,  273666941,     //  14dxxxxx
     275604547,  277547609,  279495497,  281436553,  283384393,  285335587,  287282227,  289230101,  291178001,  293126107,     //  15dxxxxx
     295075153,  297027277,  298981139,  300928469,  302883151,  304836293,  306789653,  308739247,  310695797,  312650557,     //  16dxxxxx
     314606891,  316564793,  318525731,  320486191,  322445923,  324407131,  326363251,  328323209,  330287569,  332252363,     //  17dxxxxx
     334214467,  336175529,  338141569,  340104241,  342066091,  344032391,  345996263,  347966401,  349931947,  351901169,     //  18dxxxxx
     353868019,  355836337,  357807473,  359777777,  361749979,  363720403,  365691323,  367660399,  369638219,  371614363,     //  19dxxxxx
     373587911,  375556459,  377528093,  379499353,  381471617,  383446691,  385426549,  387404137,  389381621,  391364153,     //  20dxxxxx
     393342743,  395318837,  397303559,  399284917,  401265737,  403245719,  405225649,  407205781,  409192999,  411174227,     //  21dxxxxx
     413158523,  415142293,  417125279,  419113753,  421101649,  423087253,  425069573,  427057793,  429042797,  431031857,     //  22dxxxxx
     433024253,  435011267,  437003069,  438990701,  440981153,  442967117,  444960067,  446952503,  448943711,  450937139,     //  23dxxxxx
     452930477,  454923607,  456911509,  458910607,  460908053,  462900953,  464895103,  466890341,  468884879,  470882803,     //  24dxxxxx
     472882049,  474883949,  476880797,  478876103,  480871423,  482873137,  484870097,  486870889,  488874803,  490872911,     //  25dxxxxx
     492876863,  494877797,  496876907,  498879481,  500883577,  502895647,  504900127,  506904221,  508910533,  510921781,     //  26dxxxxx
     512927377,  514936657,  516939919,  518944409,  520951843,  522960533,  524967347,  526975483,  528981773,  530991367,     //  27dxxxxx
     533000401,  535010699,  537018373,  539028983,  541037269,  543052501,  545066747,  547079011,  549088471,  551096149,     //  28dxxxxx
     553105253,  555119489,  557136683,  559149623,  561166163,  563178743,  565194727,  567211129,  569225611,  571245413,     //  29dxxxxx
     573259433,  575276873,  577292777,  579311287,  581330611,  583345003,  585367063,  587385023,  589405753,  591419089,     //  30dxxxxx
     593441861,  595459199,  597473749,  599500333,  601518833,  603538541,  605558641,  607582961,  609606539,  611630147,     //  31dxxxxx
     613651369,  615673501,  617707261,  619728821,  621753667,  623781269,  625804937,  627826343,  629853437,  631878503,     //  32dxxxxx
     633910111,  635935931,  637961257,  639991241,  642016891,  644047709,  646075673,  648104887,  650132141,  652161791,     //  33dxxxxx
     654188429,  656222549,  658251619,  660280177,  662308589,  664338817,  666372947,  668405789,  670437871,  672474799,     //  34dxxxxx
     674506111,  676541557,  678576179,  680610467,  682647811,  684681301,  686712391,  688744921,  690775747,  692812097,     //  35dxxxxx
     694847539,  696881947,  698922179,  700959719,  702993937,  705031199,  707069533,  709104961,  711145559,  713180471,     //  36dxxxxx
     715225741,  717266401,  719304541,  721343171,  723379843,  725420411,  727459583,  729505201,  731547571,  733588393,     //  37dxxxxx
     735632797,  737675791,  739716349,  741751537,  743798113,  745843009,  747889573,  749934701,  751980769,  754028207,     //  38dxxxxx
     756065179,  758113399,  760158937,  762201677,  764252267,  766301759,  768347903,  770388767,  772432847,  774481033,     //  39dxxxxx
     776531419,  778577621,  780625631,  782671133,  784716623,  786760649,  788811659,  790852651,  792904111,  794957347,     //  40dxxxxx
     797003437,  799048279,  801102823,  803155729,  805200211,  807247109,  809299703,  811349603,  813404717,  815457043,     //  41dxxxxx
     817504253,  819558713,  821611859,  823663229,  825719371,  827772511,  829827433,  831882217,  833934359,  835988203,     //  42dxxxxx
     838041647,  840094061,  842149313,  844202239,  846260237,  848321921,  850372561,  852430819,  854486027,  856542257,     //  43dxxxxx
     858599509,  860662669,  862721441,  864775789,  866836543,  868891399,  870947863,  873008797,  875065823,  877132621,     //  44dxxxxx
     879190841,  881253493,  883311217,  885375367,  887435303,  889495223,  891560609,  893617871,  895682083,  897746123,     //  45dxxxxx
     899809363,  901866943,  903932807,  905995897,  908054801,  910112683,  912171059,  914234743,  916292957,  918363373,     //  46dxxxxx
     920419823,  922484831,  924551941,  926615597,  928683689,  930754037,  932819479,  934881863,  936948223,  939014701,     //  47dxxxxx
     941083987,  943152383,  945217501,  947282681,  949349537,  951421147,  953485517,  955550359,  957618023,  959683157,     //  48dxxxxx
     961748941,  963816187,  965886697,  967956853,  970024753,  972092467,  974165923,  976240123,  978308869,  980380399,     //  49dxxxxx
     982451707,  984516803,  986587207,  988659977,  990729269,  992801861,  994872917,  996943217,  999009049, 1001088113,     //  50dxxxxx
    1003162837, 1005234953, 1007304029, 1009380649, 1011451901, 1013526181, 1015595227, 1017668207, 1019744191, 1021817309,     //  51dxxxxx
    1023893887, 1025965753, 1028046001, 1030118737, 1032193363, 1034271001, 1036348757, 1038420917, 1040493253, 1042567741,     //  52dxxxxx
    1044645419, 1046724781, 1048806709, 1050884533, 1052959793, 1055040229, 1057120819, 1059198451, 1061278429, 1063360577,     //  53dxxxxx
    1065433427, 1067506249, 1069583819, 1071659503, 1073741237, 1075824283, 1077906079, 1079986291, 1082063023, 1084139227,     //  54dxxxxx    where 1,073,741,824 = 1G
    1086218501, 1088297053, 1090378951, 1092458683, 1094539643, 1096621151, 1098701629, 1100784599, 1102864309, 1104944293,     //  55dxxxxx
    1107029839, 1109107267, 1111192087, 1113276349, 1115359823, 1117444369, 1119529007, 1121609659, 1123692473, 1125782597,     //  56dxxxxx
    1127870683, 1129953169, 1132040579, 1134127457, 1136217703, 1138305547, 1140386777, 1142478383, 1144570529, 1146654779,     //  57dxxxxx
    1148739817, 1150826477, 1152910469, 1154994991, 1157079827, 1159168537, 1161258401, 1163341769, 1165422547, 1167514399,     //  58dxxxxx
    1169604841, 1171689557, 1173775303, 1175864983, 1177951703, 1180041943, 1182135197, 1184227537, 1186320001, 1188407929,     //  59dxxxxx
    1190494771, 1192588091, 1194679561, 1196767777, 1198855069, 1200949609, 1203042521, 1205137013, 1207224973, 1209316411,     //  60dxxxxx
    1211405387, 1213498541, 1215591287, 1217680003, 1219769791, 1221863261, 1223955221, 1226052953, 1228150969, 1230242567,     //  61dxxxxx
    1232332813, 1234429793, 1236521513, 1238613797, 1240713121, 1242809783, 1244904761, 1246994599, 1249088273, 1251177269,     //  62dxxxxx
    1253270833, 1255366603, 1257465739, 1259559797, 1261655519, 1263751141, 1265848777, 1267938673, 1270027261, 1272127859,     //  63dxxxxx
    1274224999, 1276323821, 1278422053, 1280512439, 1282612601, 1284710771, 1286807377, 1288907093, 1291002247, 1293100099,     //  64dxxxxx
    1295202523, 1297303627, 1299403709, 1301502331, 1303601473, 1305698249, 1307795903, 1309889057, 1311987569, 1314089327,     //  65dxxxxx
    1316196209, 1318295609, 1320395761, 1322492419, 1324597811, 1326697579, 1328798579, 1330902721, 1332998417, 1335097129,     //  66dxxxxx
    1337195527, 1339296239, 1341399359, 1343497217, 1345602031, 1347701867, 1349804867, 1351911679, 1354008899, 1356105089,     //  67dxxxxx
    1358208613, 1360311259, 1362413383, 1364516117, 1366619797, 1368724913, 1370828741, 1372933259, 1375040479, 1377146369,     //  68dxxxxx
    1379256029, 1381359671, 1383461393, 1385570029, 1387679113, 1389786323, 1391880653, 1393991609, 1396094023, 1398200047,     //  69dxxxxx
    1400305369, 1402416709, 1404522397, 1406632621, 1408737047, 1410844907, 1412953543, 1415056499, 1417168889, 1419270851,     //  70dxxxxx
    1421376533, 1423485509, 1425593963, 1427705249, 1429808747, 1431916091, 1434028837, 1436135293, 1438247879, 1440355853,     //  71dxxxxx
    1442469313, 1444576379, 1446680891, 1448788681, 1450899347, 1453010737, 1455122611, 1457229539, 1459338679, 1461446149,     //  72dxxxxx
    1463555011, 1465666369, 1467781039, 1469897921, 1472008807, 1474118929, 1476223313, 1478328623, 1480443061, 1482557983,     //  73dxxxxx
    1484670179, 1486778317, 1488884053, 1490994899, 1493105833, 1495213271, 1497325307, 1499438813, 1501552333, 1503670907,     //  74dxxxxx
    1505776963, 1507890367, 1510006049, 1512124819, 1514242211, 1516351777, 1518466949, 1520577089, 1522690633, 1524806009,     //  75dxxxxx
    1526922017, 1529034803, 1531145257, 1533263597, 1535377609, 1537493917, 1539613021, 1541729071, 1543847689, 1545959707,     //  76dxxxxx
    1548074371, 1550188553, 1552302701, 1554418813, 1556536493, 1558655507, 1560772021, 1562894093, 1565016179, 1567130891,     //  77dxxxxx
    1569250363, 1571368111, 1573488181, 1575604103, 1577719123, 1579833509, 1581950137, 1584065603, 1586187761, 1588305281,     //  78dxxxxx
    1590425983, 1592539523, 1594668377, 1596782353, 1598903321, 1601020433, 1603141829, 1605258419, 1607375933, 1609500229,     //  79dxxxxx
    1611623887, 1613743589, 1615863233, 1617986023, 1620110831, 1622223991, 1624347299, 1626468821, 1628590751, 1630707487,     //  80dxxxxx
    1632828059, 1634944561, 1637064479, 1639187119, 1641305923, 1643429663, 1645557623, 1647680453, 1649802461, 1651927223,     //  81dxxxxx
    1654054511, 1656180787, 1658302651, 1660426589, 1662547979, 1664674819, 1666793189, 1668920849, 1671043009, 1673164289,     //  82dxxxxx
    1675293223, 1677416077, 1679535239, 1681661801, 1683786827, 1685912299, 1688031619, 1690153867, 1692281111, 1694404681,     //  83dxxxxx
    1696528907, 1698655967, 1700784643, 1702911323, 1705036303, 1707155683, 1709282387, 1711403641, 1713531997, 1715661247,     //  84dxxxxx
    1717783153, 1719908503, 1722031709, 1724159627, 1726290703, 1728417367, 1730548843, 1732677257, 1734807163, 1736937557,     //  85dxxxxx
    1739062387, 1741187431, 1743317047, 1745442953, 1747573771, 1749701927, 1751828929, 1753957537, 1756083473, 1758214607,     //  86dxxxxx
    1760341447, 1762473959, 1764602687, 1766727161, 1768858579, 1770989611, 1773118147, 1775253677, 1777379839, 1779508279,     //  87dxxxxx
    1781636627, 1783766059, 1785896681, 1788027713, 1790159317, 1792287229, 1794417769, 1796542421, 1798670243, 1800800369,     //  88dxxxxx
    1802933621, 1805066983, 1807198873, 1809327319, 1811461153, 1813593029, 1815727999, 1817859781, 1819993891, 1822127981,     //  89dxxxxx
    1824261419, 1826397187, 1828525481, 1830661411, 1832796751, 1834925117, 1837056673, 1839190973, 1841324857, 1843456007,     //  90dxxxxx
    1845587717, 1847726633, 1849864297, 1851998119, 1854134629, 1856264467, 1858397413, 1860536957, 1862670913, 1864804703,     //  91dxxxxx
    1866941123, 1869080069, 1871212333, 1873350991, 1875484267, 1877619461, 1879754147, 1881892861, 1884030431, 1886170673,     //  92dxxxxx
    1888303063, 1890435221, 1892570851, 1894712087, 1896848363, 1898979371, 1901109701, 1903250719, 1905386173, 1907527541,     //  93dxxxxx
    1909662913, 1911801673, 1913940361, 1916074207, 1918218437, 1920354661, 1922496853, 1924632277, 1926775157, 1928909987,     //  94dxxxxx
    1931045239, 1933183303, 1935319703, 1937458529, 1939601639, 1941743653, 1943880457, 1946020463, 1948154267, 1950293473,     //  95dxxxxx
    1952429177, 1954566829, 1956706319, 1958847713, 1960990007, 1963130473, 1965269363, 1967413237, 1969551757, 1971688717,     //  96dxxxxx
    1973828669, 1975967183, 1978106729, 1980245543, 1982386079, 1984525423, 1986669829, 1988809573, 1990951133, 1993090313,     //  97dxxxxx
    1995230821, 1997376151, 1999520693, 2001658177, 2003793959, 2005933283, 2008075067, 2010220561, 2012361313, 2014495829,     //  98dxxxxx
    2016634099, 2018781719, 2020923649, 2023066369, 2025211421, 2027354369, 2029501879, 2031647507, 2033788411, 2035929109,     //  99dxxxxx
    2038074751, 2040217609, 2042368613, 2044505839, 2046648391, 2048795527, 2050930523, 2053078397, 2055223741, 2057369521,     // 100dxxxxx
    2059519673, 2061668221, 2063812651, 2065956253, 2068101319, 2070248617, 2072396201, 2074538057, 2076681877, 2078825471,     // 101dxxxxx
    2080975187, 2083115831, 2085262129, 2087412781, 2089556039, 2091702673, 2093846743, 2095989439, 2098139233, 2100286781,     // 102dxxxxx
    2102429887, 2104575691, 2106722209, 2108866009, 2111010919, 2113154951, 2115298249, 2117447639, 2119595167, 2121744971,     // 103dxxxxx
    2123895979, 2126044717, 2128193651, 2130347629, 2132500423, 2134651583, 2136801209, 2138954401, 2141097799, 2143243741,     // 104dxxxxx
    2145390539, 2147535979, 2149686131, 2151833513, 2153982079, 2156130677, 2158281547, 2160428339, 2162578637, 2164729093,     // 105dxxxxx    where 2,147,483,648 = 2G
    2166877481, 2169029321, 2171173561, 2173328107, 2175476579, 2177624387, 2179772723, 2181925957, 2184081491, 2186234227,     // 106dxxxxx
    2188381957, 2190533663, 2192681971, 2194832347, 2196982049, 2199134039, 2201288497, 2203445081, 2205600209, 2207748611,     // 107dxxxxx
    2209898671, 2212055033, 2214204073, 2216358643, 2218510111, 2220657821, 2222812763, 2224969289, 2227120039, 2229271663,     // 108dxxxxx
    2231422093, 2233580837, 2235730729, 2237880241, 2240033473, 2242187771, 2244340841, 2246490031, 2248642763, 2250792527,     // 109dxxxxx
    2252945257, 2255101193, 2257257917, 2259413383, 2261561303, 2263710389, 2265866021, 2268014689, 2270169017, 2272317287,     // 110dxxxxx
    2274464659, 2276616451, 2278774681, 2280933937, 2283090947, 2285248019, 2287397159, 2289549397, 2291707609, 2293861511,     // 111dxxxxx
    2296014689, 2298172127, 2300323691, 2302483829, 2304639437, 2306797483, 2308955413, 2311109513, 2313263651, 2315416249,     // 112dxxxxx
    2317573397, 2319740557, 2321892679, 2324048533, 2326203227, 2328364933, 2330527981, 2332684619, 2334837431, 2336994871,     // 113dxxxxx
    2339150659, 2341308941, 2343465811, 2345629549, 2347785241, 2349946231, 2352105467, 2354260163, 2356419301, 2358574763,     // 114dxxxxx
    2360739659, 2362897561, 2365058299, 2367215261, 2369369921, 2371528573, 2373688277, 2375848831, 2378005579, 2380165901,     // 115dxxxxx
    2382327907, 2384482307, 2386642177, 2388804289, 2390960111, 2393118311, 2395280449, 2397444697, 2399604409, 2401765339,     // 116dxxxxx
    2403927809, 2406089921, 2408249147, 2410409503, 2412570299, 2414731211, 2416896929, 2419051183, 2421215431, 2423375081,     // 117dxxxxx
    2425530221, 2427686903, 2429851483, 2432015323, 2434176011, 2436343337, 2438499667, 2440656541, 2442818113, 2444982427,     // 118dxxxxx
    2447142781, 2449309903, 2451476669, 2453633893, 2455793537, 2457955477, 2460121063, 2462284483, 2464452761, 2466611449,     // 119dxxxxx
    2468776133, 2470936051, 2473094599, 2475257219, 2477417027, 2479578841, 2481738949, 2483901631, 2486067323, 2488233623,     // 120dxxxxx
    2490401363, 2492567843, 2494728941, 2496897539, 2499062479, 2501226809, 2503396517, 2505557069, 2507716111, 2509880057,     // 121dxxxxx
    2512046843, 2514205943, 2516368573, 2518531091, 2520697103, 2522861227, 2525029421, 2527193737, 2529355201, 2531522083,     // 122dxxxxx
    2533680377, 2535848869, 2538009013, 2540175259, 2542335767, 2544507943, 2546674133, 2548840403, 2550999653, 2553167537,     // 123dxxxxx
    2555333791, 2557496441, 2559660497, 2561827691, 2563996187, 2566154609, 2568322373, 2570486813, 2572651589, 2574815443,     // 124dxxxxx
    2576983873, 2579150657, 2581317611, 2583483107, 2585650901, 2587819889, 2589986309, 2592148309, 2594320973, 2596486447,     // 125dxxxxx
    2598653297, 2600820121, 2602993553, 2605158877, 2607331117, 2609503703, 2611669399, 2613838133, 2616009647, 2618176537,     // 126dxxxxx
    2620343189, 2622509909, 2624680111, 2626850959, 2629018373, 2631187709, 2633357521, 2635531831, 2637704143, 2639873773,     // 127dxxxxx
    2642042567, 2644214371, 2646386147, 2648555513, 2650724443, 2652900961, 2655067109, 2657230267, 2659402243, 2661577519,     // 128dxxxxx
    2663748127, 2665918813, 2668089751, 2670257279, 2672430223, 2674597333, 2676766199, 2678935099, 2681104301, 2683282663,     // 129dxxxxx
    2685457433, 2687622491, 2689790849, 2691962827, 2694137417, 2696305529, 2698474739, 2700647113, 2702816713, 2704990663,     // 130dxxxxx
    2707161469, 2709332477, 2711504317, 2713675961, 2715848117, 2718019547, 2720191237, 2722366121, 2724545333, 2726714603,     // 131dxxxxx
    2728895899, 2731072679, 2733243439, 2735417189, 2737592047, 2739760333, 2741928947, 2744111827, 2746285397, 2748463891,     // 132dxxxxx
    2750638589, 2752808467, 2754985879, 2757153713, 2759329813, 2761509757, 2763680107, 2765859673, 2768031547, 2770205287,     // 133dxxxxx
    2772373147, 2774547079, 2776720357, 2778894683, 2781070697, 2783243693, 2785422019, 2787594461, 2789769049, 2791944593,     // 134dxxxxx
    2794119131, 2796295231, 2798468521, 2800648619, 2802818729, 2804992699, 2807167877, 2809344539, 2811522379, 2813700797,     // 135dxxxxx
    2815869337, 2818047049, 2820220097, 2822394391, 2824565087, 2826744619, 2828921663, 2831095769, 2833276913, 2835454199,     // 136dxxxxx
    2837630989, 2839807657, 2841987199, 2844165283, 2846341207, 2848518527, 2850696463, 2852869109, 2855043773, 2857218857,     // 137dxxxxx
    2859399871, 2861572339, 2863748071, 2865925081, 2868102539, 2870280241, 2872456337, 2874634591, 2876815757, 2878993121,     // 138dxxxxx
    2881172071, 2883350191, 2885528593, 2887707149, 2889887657, 2892069269, 2894247179, 2896423219, 2898605057, 2900781991,     // 139dxxxxx
    2902958803, 2905136473, 2907315659, 2909496283, 2911680397, 2913859031, 2916038911, 2918217551, 2920398053, 2922576841,     // 140dxxxxx
    2924759987, 2926933783, 2929110647, 2931290717, 2933470459, 2935653631, 2937834223, 2940012401, 2942189213, 2944369321,     // 141dxxxxx
    2946551947, 2948730569, 2950909573, 2953089133, 2955277349, 2957462479, 2959641187, 2961824527, 2964002089, 2966180537,     // 142dxxxxx
    2968362017, 2970548249, 2972731679, 2974914017, 2977094339, 2979272123, 2981452597, 2983636237, 2985818029, 2987993791,     // 143dxxxxx
    2990179867, 2992357211, 2994540379, 2996726651, 2998916159, 3001098253, 3003285599, 3005464871, 3007652057, 3009838703,     // 144dxxxxx
    3012017611, 3014196763, 3016380307, 3018564407, 3020747123, 3022932383, 3025117103, 3027303871, 3029486503, 3031670651,     // 145dxxxxx
    3033850621, 3036035243, 3038216611, 3040400039, 3042582019, 3044762219, 3046946557, 3049131769, 3051317111, 3053500139,     // 146dxxxxx
    3055685131, 3057865907, 3060049987, 3062237987, 3064417313, 3066595537, 3068781091, 3070964389, 3073143019, 3075326179,     // 147dxxxxx
    3077513203, 3079698221, 3081882521, 3084069887, 3086253281, 3088441351, 3090625441, 3092812741, 3095002487, 3097185809,     // 148dxxxxx
    3099373553, 3101565163, 3103753867, 3105943691, 3108125719, 3110310727, 3112497469, 3114684823, 3116871101, 3119053753,     // 149dxxxxx
    3121238921, 3123425003, 3125607073, 3127792961, 3129976477, 3132168493, 3134354633, 3136544167, 3138730249, 3140916613,     // 150dxxxxx
    3143102489, 3145292561, 3147479983, 3149663879, 3151848547, 3154034641, 3156218749, 3158407573, 3160595059, 3162785833,     // 151dxxxxx
    3164969221, 3167154127, 3169343011, 3171528671, 3173720609, 3175911679, 3178100351, 3180282541, 3182476289, 3184665647,     // 152dxxxxx
    3186853097, 3189035099, 3191224697, 3193415807, 3195602369, 3197792513, 3199982543, 3202170841, 3204353941, 3206541979,     // 153dxxxxx
    3208726531, 3210918047, 3213106847, 3215298001, 3217486639, 3219680071, 3221870881, 3224058779, 3226250441, 3228442981,     // 154dxxxxx    where 3,221,225,472 = 3G
    3230631853, 3232825181, 3235010839, 3237201389, 3239395259, 3241584583, 3243775357, 3245963213, 3248151899, 3250344481,     // 155dxxxxx
    3252531833, 3254715923, 3256906669, 3259098749, 3261286601, 3263483599, 3265674793, 3267868801, 3270065153, 3272255347,     // 156dxxxxx
    3274449047, 3276637673, 3278828323, 3281019649, 3283211231, 3285400849, 3287591201, 3289782409, 3291976351, 3294166319,     // 157dxxxxx
    3296363299, 3298549997, 3300748513, 3302940049, 3305127299, 3307315987, 3309511717, 3311708833, 3313899553, 3316090513,     // 158dxxxxx
    3318283337, 3320467751, 3322657379, 3324850241, 3327040439, 3329233717, 3331427753, 3333617327, 3335809489, 3338000537,     // 159dxxxxx
    3340200043, 3342394453, 3344588909, 3346780709, 3348976171, 3351166237, 3353360531, 3355555397, 3357756287, 3359953969,     // 160dxxxxx
    3362143859, 3364335361, 3366531803, 3368723441, 3370919921, 3373110319, 3375306989, 3377497259, 3379693123, 3381893591,     // 161dxxxxx
    3384088481, 3386281567, 3388472387, 3390661201, 3392864413, 3395057293, 3397249733, 3399446269, 3401642099, 3403833401,     // 162dxxxxx
    3406031011, 3408226193, 3410423389, 3412617629, 3414812273, 3417006781, 3419202971, 3421394507, 3423582221, 3425778289,     // 163dxxxxx
    3427973513, 3430165223, 3432367451, 3434568233, 3436762403, 3438957517, 3441153383, 3443345371, 3445543379, 3447733151,     // 164dxxxxx
    3449931913, 3452125381, 3454319107, 3456516781, 3458717489, 3460911109, 3463104439, 3465297509, 3467493737, 3469691507,     // 165dxxxxx
    3471893413, 3474089557, 3476285399, 3478487581, 3480676327, 3482876317, 3485070943, 3487265219, 3489463573, 3491665123,     // 166dxxxxx
    3493866479, 3496055923, 3498253679, 3500453507, 3502652189, 3504849037, 3507041269, 3509237059, 3511434611, 3513630301,     // 167dxxxxx
    3515827261, 3518025289, 3520224571, 3522423367, 3524621279, 3526816697, 3529013723, 3531218927, 3533412479, 3535610657,     // 168dxxxxx
    3537803431, 3540008323, 3542203957, 3544408961, 3546605833, 3548802041, 3550997759, 3553199743, 3555394351, 3557594263,     // 169dxxxxx
    3559788191, 3561998131, 3564197789, 3566394131, 3568593377, 3570787741, 3572990563, 3575194463, 3577395499, 3579595319,     // 170dxxxxx
    3581791333, 3583990453, 3586191269, 3588392131, 3590594257, 3592802909, 3595004423, 3597197509, 3599401627, 3601603739,     // 171dxxxxx
    3603805043, 3605999533, 3608204513, 3610407833, 3612608327, 3614818279, 3617013943, 3619210171, 3621408919, 3623607413,     // 172dxxxxx
    3625809203, 3628012369, 3630209881, 3632409373, 3634608799, 3636814219, 3639014641, 3641209769, 3643407409, 3645606821,     // 173dxxxxx
    3647809351, 3650010421, 3652211533, 3654408227, 3656614817, 3658818853, 3661026029, 3663229439, 3665435021, 3667626463,     // 174dxxxxx
    3669829409, 3672029839, 3674234351, 3676433729, 3678629399, 3680834581, 3683037373, 3685245523, 3687450839, 3689654117,     // 175dxxxxx
    3691857809, 3694059833, 3696259397, 3698469113, 3700670669, 3702875531, 3705079459, 3707284219, 3709488271, 3711694801,     // 176dxxxxx
    3713895593, 3716097001, 3718300529, 3720499303, 3722693483, 3724898249, 3727105069, 3729307429, 3731506147, 3733710011,     // 177dxxxxx
    3735911771, 3738115211, 3740320303, 3742523479, 3744725731, 3746933573, 3749137213, 3751345049, 3753546281, 3755756393,     // 178dxxxxx
    3757961587, 3760163249, 3762366991, 3764572897, 3766780481, 3768978067, 3771187151, 3773392877, 3775598777, 3777801619,     // 179dxxxxx
    3780008341, 3782210141, 3784419719, 3786626237, 3788834303, 3791040413, 3793249637, 3795451783, 3797654911, 3799864637,     // 180dxxxxx
    3802067861, 3804269099, 3806476607, 3808680311, 3810890089, 3813097259, 3815301883, 3817503259, 3819708199, 3821916383,     // 181dxxxxx
    3824122123, 3826330819, 3828541859, 3830744909, 3832953203, 3835154971, 3837365591, 3839570311, 3841775183, 3843986897,     // 182dxxxxx
    3846198217, 3848411381, 3850617097, 3852826381, 3855035539, 3857245747, 3859447573, 3861656893, 3863859443, 3866073391,     // 183dxxxxx
    3868277323, 3870480511, 3872690959, 3874899649, 3877106141, 3879316349, 3881519371, 3883725577, 3885932653, 3888141773,     // 184dxxxxx
    3890351573, 3892563317, 3894769981, 3896982077, 3899190901, 3901399937, 3903608213, 3905807201, 3908014087, 3910226377,     // 185dxxxxx
    3912435827, 3914650721, 3916865189, 3919076123, 3921283571, 3923497027, 3925704353, 3927917639, 3930125609, 3932333701,     // 186dxxxxx
    3934544881, 3936754721, 3938959489, 3941169319, 3943377967, 3945592097, 3947795309, 3949999919, 3952210709, 3954419309,     // 187dxxxxx
    3956628541, 3958840429, 3961052321, 3963254951, 3965464133, 3967676117, 3969889349, 3972103279, 3974311609, 3976520923,     // 188dxxxxx
    3978735119, 3980944789, 3983156341, 3985369897, 3987579247, 3989791471, 3992007119, 3994218091, 3996428773, 3998635057,     // 189dxxxxx
    4000846303, 4003052743, 4005264779, 4007479163, 4009688971, 4011903817, 4014120191, 4016331647, 4018548481, 4020755537,     // 190dxxxxx
    4022963653, 4025176643, 4027391581, 4029602107, 4031815471, 4034025031, 4036238273, 4038452947, 4040666039, 4042872851,     // 191dxxxxx
    4045093763, 4047306449, 4049516921, 4051729349, 4053938279, 4056148843, 4058361883, 4060572413, 4062784681, 4064995499,     // 192dxxxxx
    4067203993, 4069420681, 4071641647, 4073859323, 4076068391, 4078283087, 4080501433, 4082712187, 4084924471, 4087134277,     // 193dxxxxx
    4089353459, 4091563067, 4093774211, 4095986189, 4098204443, 4100420459, 4102637333, 4104848519, 4107060553, 4109272547,     // 194dxxxxx
    4111485211, 4113700457, 4115908103, 4118123323, 4120332373, 4122542101, 4124755963, 4126969313, 4129183769, 4131392467,     // 195dxxxxx
    4133602933, 4135817921, 4138035697, 4140255289, 4142471057, 4144683817, 4146898363, 4149115933, 4151325961, 4153542307,     // 196dxxxxx
    4155754903, 4157975123, 4160188057, 4162405417, 4164626957, 4166844277, 4169058253, 4171272829, 4173493889, 4175710111,     // 197dxxxxx
    4177926547, 4180142441, 4182358963, 4184576113, 4186789463, 4189007569, 4191218831, 4193442493, 4195655407, 4197871061,     // 198dxxxxx
    4200086581, 4202297153, 4204505887, 4206722291, 4208938133, 4211155609, 4213371193, 4215583919, 4217800343, 4220018497,     // 199dxxxxx
    4222234763, 4224447677, 4226670769, 4228885891, 4231100827, 4233321593, 4235534983, 4237752017, 4239974341, 4242190873,     // 200dxxxxx
    4244406163, 4246622213, 4248837883, 4251053783, 4253271247, 4255494379, 4257712259, 4259930347, 4262147663, 4264364647,     // 201dxxxxx
    4266581191, 4268801143, 4271019583, 4273234573, 4275449249, 4277659189, 4279875371, 4282091083, 4284310847, 4286521973,     // 202dxxxxx
    4288745717, 4290967259, 4293184109,                                                                                         // 203dxxxxx    where 4,294,967,296 = 4G
    -1
   }
#endif
;

EXTERN
int NthPrimeInc
#ifdef DEFINE_VALUES
 = 100000
#endif
;

EXTERN
UINT NthPrimeCnt
#ifdef DEFINE_VALUES
 = countof (NthPrimeTab) - 1
#endif
;

typedef struct tagNTHPOWERTAB
{
    LPCHAR lpChar;
    APLMPI aplMPI;
} NTHPOWERTAB, *LPNTHPOWERTAB;

// The Nth prime (in origin-0) in powers of 10
// The following values were obtained from
//   http://oeis.org/A006988/
EXTERN
NTHPOWERTAB NthPowerTab[]
#ifdef DEFINE_VALUES
= {{"3",                     0},    // 1E0
   {"31",                    0},    // 1E1
   {"547",                   0},    // 1E2
   {"7927",                  0},    // 1E3
   {"104743",                0},    // 1E4
   {"1299721",               0},    // 1E5
   {"15485867",              0},    // 1E6
   {"179424691",             0},    // 1E7
   {"2038074751",            0},    // 1E8
   {"22801763513",           0},    // 1E9
   {"252097800629",          0},    // 1E10
   {"2760727302559",         0},    // 1E11
   {"29996224275851",        0},    // 1E12
   {"323780508946379",       0},    // 1E13
   {"3475385758524587",      0},    // 1E14
   {"37124508045065489",     0},    // 1E15
   {"394906913903735377",    0},    // 1E16
   {"4185296581467695677",   0},    // 1E17
   {"44211790234832169353",  0},    // 1E18
   {"-1"                  ,  0},
  }
#endif
;

EXTERN
UINT NthPowerCnt
#ifdef DEFINE_VALUES
 = countof (NthPowerTab)
#endif
;

// The # primes <= powers of ten
// The following values were obtained from
//   http://oeis.org/A006880
EXTERN
APLINT NumPowerTab[]
#ifdef DEFINE_VALUES
 = {0,                      // 1E0
    4,                      // 1E1
    25,                     // 1E2
    168,                    // 1E3
    1229,                   // 1E4
    9592,                   // 1E5
    78498,                  // 1E6
    664579,                 // 1E7
    5761455,                // 1E8
    50847534,               // 1E9
    455052511,              // 1E10
    4118054813,             // 1E11
    37607912018,            // 1E12
    346065536839,           // 1E13
    3204941750802,          // 1E14
    29844570422669,         // 1E15
    279238341033925,        // 1E16
    2623557157654233,       // 1E17
    24739954287740860,      // 1E18
    234057667276344607,     // 1E19
    2220819602560918840,    // 1E20
   }
#endif
;


//***************************************************************************
//  End of File: primes.h
//***************************************************************************