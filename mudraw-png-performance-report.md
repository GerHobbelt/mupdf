
# Measuring performance of MuDraw rendering a PDF to page images

Command run:

```
$ ( ( for f in 0 1 2 3 4 5 6 7 8 9 ; do ./mutool.exe draw -J $f -j content -T0 -stmf -w 3600 -h 3600 -r 0 -F png -o page-compress$f.png  1905.07689.pdf ; ls -lsk page-compress$f.0*.png ;  done ) 2>&1 ) > ../../../../mudraw-png-performance-report.md
```

Output:

```
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 659ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 658ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 937ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 2010ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 1774ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 788ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 2668ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 2339ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 2244ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 1823ms (total)
Glyph Cache Size: 1048565
total 15900ms (1ms layout) / 10 pages for an average of 1590ms
fastest page 2: 658ms
slowest page 7: 2668ms
 4292 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0001.png
 4808 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0002.png
 8684 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0003.png
10428 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0004.png
 6172 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0005.png
10644 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0006.png
18020 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0007.png
18296 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0008.png
18340 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0009.png
17916 -rw-r--r-- 1 Ger 197121 30004951 Jun 13 01:12 page-compress0.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 411ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 411ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 438ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 381ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 403ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 3147ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 321ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 340ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 459ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 370ms (total)
Glyph Cache Size: 1048565
total 6681ms (1ms layout) / 10 pages for an average of 668ms
fastest page 7: 321ms
slowest page 6: 3147ms
1676 -rw-r--r-- 1 Ger 197121 1774680 Jun 13 01:12 page-compress1.0001.png
1948 -rw-r--r-- 1 Ger 197121 2055907 Jun 13 01:12 page-compress1.0002.png
1648 -rw-r--r-- 1 Ger 197121 1745545 Jun 13 01:12 page-compress1.0003.png
1824 -rw-r--r-- 1 Ger 197121 1929157 Jun 13 01:12 page-compress1.0004.png
1796 -rw-r--r-- 1 Ger 197121 1898440 Jun 13 01:12 page-compress1.0005.png
1940 -rw-r--r-- 1 Ger 197121 2044354 Jun 13 01:13 page-compress1.0006.png
1300 -rw-r--r-- 1 Ger 197121 1389266 Jun 13 01:13 page-compress1.0007.png
1568 -rw-r--r-- 1 Ger 197121 1665970 Jun 13 01:13 page-compress1.0008.png
1972 -rw-r--r-- 1 Ger 197121 2078912 Jun 13 01:13 page-compress1.0009.png
2144 -rw-r--r-- 1 Ger 197121 2254918 Jun 13 01:13 page-compress1.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 403ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 391ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 489ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 382ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 417ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 400ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 346ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 374ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 447ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 444ms (total)
Glyph Cache Size: 1048565
total 4093ms (1ms layout) / 10 pages for an average of 409ms
fastest page 7: 346ms
slowest page 3: 489ms
1372 -rw-r--r-- 1 Ger 197121 1463587 Jun 13 01:13 page-compress2.0001.png
1624 -rw-r--r-- 1 Ger 197121 1724302 Jun 13 01:13 page-compress2.0002.png
1344 -rw-r--r-- 1 Ger 197121 1436854 Jun 13 01:13 page-compress2.0003.png
1516 -rw-r--r-- 1 Ger 197121 1611454 Jun 13 01:13 page-compress2.0004.png
1488 -rw-r--r-- 1 Ger 197121 1582136 Jun 13 01:13 page-compress2.0005.png
1616 -rw-r--r-- 1 Ger 197121 1715370 Jun 13 01:13 page-compress2.0006.png
1016 -rw-r--r-- 1 Ger 197121 1097937 Jun 13 01:13 page-compress2.0007.png
1272 -rw-r--r-- 1 Ger 197121 1360647 Jun 13 01:13 page-compress2.0008.png
1656 -rw-r--r-- 1 Ger 197121 1756397 Jun 13 01:13 page-compress2.0009.png
1804 -rw-r--r-- 1 Ger 197121 1907702 Jun 13 01:13 page-compress2.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 421ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 499ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 482ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 419ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 412ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 401ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 365ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 358ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 449ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 403ms (total)
Glyph Cache Size: 1048565
total 4209ms (1ms layout) / 10 pages for an average of 420ms
fastest page 8: 358ms
slowest page 2: 499ms
1316 -rw-r--r-- 1 Ger 197121 1408881 Jun 13 01:13 page-compress3.0001.png
1552 -rw-r--r-- 1 Ger 197121 1647142 Jun 13 01:13 page-compress3.0002.png
1300 -rw-r--r-- 1 Ger 197121 1388912 Jun 13 01:13 page-compress3.0003.png
1456 -rw-r--r-- 1 Ger 197121 1551025 Jun 13 01:13 page-compress3.0004.png
1428 -rw-r--r-- 1 Ger 197121 1523688 Jun 13 01:13 page-compress3.0005.png
1552 -rw-r--r-- 1 Ger 197121 1649719 Jun 13 01:13 page-compress3.0006.png
 980 -rw-r--r-- 1 Ger 197121 1062309 Jun 13 01:13 page-compress3.0007.png
1216 -rw-r--r-- 1 Ger 197121 1305113 Jun 13 01:13 page-compress3.0008.png
1600 -rw-r--r-- 1 Ger 197121 1697379 Jun 13 01:13 page-compress3.0009.png
1740 -rw-r--r-- 1 Ger 197121 1839171 Jun 13 01:13 page-compress3.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 447ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 447ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 497ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 436ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 453ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 403ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 342ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 389ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 476ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 418ms (total)
Glyph Cache Size: 1048565
total 4308ms (1ms layout) / 10 pages for an average of 430ms
fastest page 7: 342ms
slowest page 3: 497ms
1312 -rw-r--r-- 1 Ger 197121 1401337 Jun 13 01:13 page-compress4.0001.png
1540 -rw-r--r-- 1 Ger 197121 1636789 Jun 13 01:13 page-compress4.0002.png
1292 -rw-r--r-- 1 Ger 197121 1380444 Jun 13 01:13 page-compress4.0003.png
1448 -rw-r--r-- 1 Ger 197121 1542912 Jun 13 01:13 page-compress4.0004.png
1424 -rw-r--r-- 1 Ger 197121 1515609 Jun 13 01:13 page-compress4.0005.png
1544 -rw-r--r-- 1 Ger 197121 1640539 Jun 13 01:13 page-compress4.0006.png
 972 -rw-r--r-- 1 Ger 197121 1056129 Jun 13 01:13 page-compress4.0007.png
1208 -rw-r--r-- 1 Ger 197121 1295934 Jun 13 01:13 page-compress4.0008.png
1592 -rw-r--r-- 1 Ger 197121 1688814 Jun 13 01:13 page-compress4.0009.png
1728 -rw-r--r-- 1 Ger 197121 1828945 Jun 13 01:13 page-compress4.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 497ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 461ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 499ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 489ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 431ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 440ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 357ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 378ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 474ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 452ms (total)
Glyph Cache Size: 1048565
total 4478ms (1ms layout) / 10 pages for an average of 447ms
fastest page 7: 357ms
slowest page 3: 499ms
1276 -rw-r--r-- 1 Ger 197121 1367049 Jun 13 01:13 page-compress5.0001.png
1500 -rw-r--r-- 1 Ger 197121 1595825 Jun 13 01:13 page-compress5.0002.png
1256 -rw-r--r-- 1 Ger 197121 1344391 Jun 13 01:13 page-compress5.0003.png
1412 -rw-r--r-- 1 Ger 197121 1504357 Jun 13 01:13 page-compress5.0004.png
1388 -rw-r--r-- 1 Ger 197121 1479134 Jun 13 01:13 page-compress5.0005.png
1504 -rw-r--r-- 1 Ger 197121 1597760 Jun 13 01:13 page-compress5.0006.png
 944 -rw-r--r-- 1 Ger 197121 1024667 Jun 13 01:13 page-compress5.0007.png
1176 -rw-r--r-- 1 Ger 197121 1261891 Jun 13 01:13 page-compress5.0008.png
1556 -rw-r--r-- 1 Ger 197121 1652619 Jun 13 01:13 page-compress5.0009.png
1696 -rw-r--r-- 1 Ger 197121 1797559 Jun 13 01:13 page-compress5.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 505ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 508ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 530ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 503ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 482ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 453ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 373ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 544ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 501ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 459ms (total)
Glyph Cache Size: 1048565
total 4858ms (1ms layout) / 10 pages for an average of 485ms
fastest page 7: 373ms
slowest page 8: 544ms
1272 -rw-r--r-- 1 Ger 197121 1360352 Jun 13 01:13 page-compress6.0001.png
1488 -rw-r--r-- 1 Ger 197121 1581107 Jun 13 01:13 page-compress6.0002.png
1248 -rw-r--r-- 1 Ger 197121 1335836 Jun 13 01:13 page-compress6.0003.png
1404 -rw-r--r-- 1 Ger 197121 1496760 Jun 13 01:13 page-compress6.0004.png
1376 -rw-r--r-- 1 Ger 197121 1469914 Jun 13 01:13 page-compress6.0005.png
1496 -rw-r--r-- 1 Ger 197121 1589753 Jun 13 01:13 page-compress6.0006.png
 936 -rw-r--r-- 1 Ger 197121 1019643 Jun 13 01:13 page-compress6.0007.png
1164 -rw-r--r-- 1 Ger 197121 1252209 Jun 13 01:13 page-compress6.0008.png
1548 -rw-r--r-- 1 Ger 197121 1644979 Jun 13 01:13 page-compress6.0009.png
1692 -rw-r--r-- 1 Ger 197121 1793875 Jun 13 01:13 page-compress6.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 583ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 651ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 792ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 654ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 574ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 550ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 504ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 508ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 578ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 543ms (total)
Glyph Cache Size: 1048565
total 5937ms (2ms layout) / 10 pages for an average of 593ms
fastest page 7: 504ms
slowest page 3: 792ms
1288 -rw-r--r-- 1 Ger 197121 1379105 Jun 13 01:13 page-compress7.0001.png
1512 -rw-r--r-- 1 Ger 197121 1605821 Jun 13 01:13 page-compress7.0002.png
1264 -rw-r--r-- 1 Ger 197121 1355495 Jun 13 01:13 page-compress7.0003.png
1424 -rw-r--r-- 1 Ger 197121 1517143 Jun 13 01:13 page-compress7.0004.png
1396 -rw-r--r-- 1 Ger 197121 1490719 Jun 13 01:13 page-compress7.0005.png
1516 -rw-r--r-- 1 Ger 197121 1611279 Jun 13 01:13 page-compress7.0006.png
 952 -rw-r--r-- 1 Ger 197121 1036089 Jun 13 01:13 page-compress7.0007.png
1184 -rw-r--r-- 1 Ger 197121 1270390 Jun 13 01:13 page-compress7.0008.png
1568 -rw-r--r-- 1 Ger 197121 1665188 Jun 13 01:13 page-compress7.0009.png
1712 -rw-r--r-- 1 Ger 197121 1812495 Jun 13 01:13 page-compress7.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 939ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 943ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 963ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 847ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 831ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 860ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 715ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 790ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 899ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 852ms (total)
Glyph Cache Size: 1048565
total 8639ms (1ms layout) / 10 pages for an average of 863ms
fastest page 7: 715ms
slowest page 3: 963ms
1276 -rw-r--r-- 1 Ger 197121 1365681 Jun 13 01:13 page-compress8.0001.png
1492 -rw-r--r-- 1 Ger 197121 1586511 Jun 13 01:13 page-compress8.0002.png
1252 -rw-r--r-- 1 Ger 197121 1341145 Jun 13 01:13 page-compress8.0003.png
1408 -rw-r--r-- 1 Ger 197121 1501921 Jun 13 01:13 page-compress8.0004.png
1384 -rw-r--r-- 1 Ger 197121 1474883 Jun 13 01:13 page-compress8.0005.png
1500 -rw-r--r-- 1 Ger 197121 1596709 Jun 13 01:13 page-compress8.0006.png
 944 -rw-r--r-- 1 Ger 197121 1025938 Jun 13 01:13 page-compress8.0007.png
1172 -rw-r--r-- 1 Ger 197121 1258936 Jun 13 01:13 page-compress8.0008.png
1556 -rw-r--r-- 1 Ger 197121 1650783 Jun 13 01:13 page-compress8.0009.png
1700 -rw-r--r-- 1 Ger 197121 1799332 Jun 13 01:13 page-compress8.0010.png
Output format: png (DeviceRGB)
page 1905.07689.pdf 1 grayscale
 1988ms (total)
Glyph Cache Size: 726229
page 1905.07689.pdf 2 grayscale
 2051ms (total)
Glyph Cache Size: 1048542
page 1905.07689.pdf 3 color
 1844ms (total)
Glyph Cache Size: 1048472
page 1905.07689.pdf 4 grayscale
 1919ms (total)
Glyph Cache Size: 1048560
page 1905.07689.pdf 5 grayscale
 1883ms (total)
Glyph Cache Size: 1048550
page 1905.07689.pdf 6 grayscale
 2050ms (total)
Glyph Cache Size: 1048343
page 1905.07689.pdf 7 grayscale
 1535ms (total)
Glyph Cache Size: 1048471
page 1905.07689.pdf 8 grayscale
 1766ms (total)
Glyph Cache Size: 1048466
page 1905.07689.pdf 9 color
 1965ms (total)
Glyph Cache Size: 1048523
page 1905.07689.pdf 10 grayscale
 1830ms (total)
Glyph Cache Size: 1048565
total 18831ms (1ms layout) / 10 pages for an average of 1883ms
fastest page 7: 1535ms
slowest page 2: 2051ms
1324 -rw-r--r-- 1 Ger 197121 1361842 Jun 13 01:13 page-compress9.0001.png
1540 -rw-r--r-- 1 Ger 197121 1577594 Jun 13 01:13 page-compress9.0002.png
1300 -rw-r--r-- 1 Ger 197121 1335949 Jun 13 01:13 page-compress9.0003.png
1460 -rw-r--r-- 1 Ger 197121 1496179 Jun 13 01:13 page-compress9.0004.png
1440 -rw-r--r-- 1 Ger 197121 1470989 Jun 13 01:13 page-compress9.0005.png
1496 -rw-r--r-- 1 Ger 197121 1590473 Jun 13 01:13 page-compress9.0006.png
 944 -rw-r--r-- 1 Ger 197121 1025178 Jun 13 01:13 page-compress9.0007.png
1164 -rw-r--r-- 1 Ger 197121 1251243 Jun 13 01:13 page-compress9.0008.png
1552 -rw-r--r-- 1 Ger 197121 1647683 Jun 13 01:13 page-compress9.0009.png
1696 -rw-r--r-- 1 Ger 197121 1795074 Jun 13 01:13 page-compress9.0010.png
```

# Conclusion

At "no compression", i.e. `-J 0`, it's raw MuPDF output and SHOULD theoretically be fastest, IFF you don't take the disk I/O into account. Even on SSD storage (used as dedicated developer disk here), the ~ 30MByte per page image at this resolution (sharp display on HD and 'good enough' on 4K displays) is costly enough on NTFS/Win10 to be on par with the highest compression setting for the same files, while taking up ~20 times more disk space (30MByte / 1.5MByte ~= factor 20).

Meanwhile, for this PDF at least, which is representative for usual "papers" and "publications", the compression setting doesn't bring that much to the table in size gain once compression has been turned on *at all*: the difference between 'fastest' (mode 1) and 'best' (mode 9) PNG compression settings, the difference is almost unnoticable: 2255KByte vs 1795KB for the most costly page (page 9) is a factor 2255/1795 ~= factor 1.256 or a gain of 26%, or when you enjoy "doing the numbers to make your outcome more palatable" ("In statistics, no-one lies or deceives, we may just happen to disagree on applied methodology") this is identical to stating that picking the fastest compression mode rather than the *best* one, comes at a mere *cost increase* of 1 - 1795/2255 ~= 0.204, ergo ~ 20%, hence *costing* us only 20% more disk space, while gaining a speed gain of 18831/6681 ~= 2.8 or almost a speed increase by a factor of 3!

Meanwhile, the raw data dump (mode 0: no compression) takes up as much time as maximum compression would: 15900 vs 18831ms.

Now for the **optimum compression level**: we want to spend as little time as possible on rendering PDF pages, and without taking PNG depacking at the receiver side, the time taken doesn't grow noticable until compression level 5..6:

```
total 15900ms (1ms layout) / 10 pages for an average of 1590ms - mode 0
total 6681ms (1ms layout) / 10 pages for an average of 668ms   - mode 1
total 4093ms (1ms layout) / 10 pages for an average of 409ms   - mode 2
total 4209ms (1ms layout) / 10 pages for an average of 420ms   - mode 3
total 4308ms (1ms layout) / 10 pages for an average of 430ms   - mode 4
total 4478ms (1ms layout) / 10 pages for an average of 447ms   - mode 5
total 4858ms (1ms layout) / 10 pages for an average of 485ms   - mode 6
total 5937ms (2ms layout) / 10 pages for an average of 593ms   - mode 7
total 8639ms (1ms layout) / 10 pages for an average of 863ms   - mode 8
total 18831ms (1ms layout) / 10 pages for an average of 1883ms - mode 9
```

At compression level 2, funnily enough the total time spent is even less than in mode 1, while the output sizes are near identical.

While the absolute bottom time, hence *optimum*, for this sample is at compression level 2, we aim for the **potentially best, while fast** compression level: level 5, which is only 447/409 ~ less than 10% slower than the raw optimum, while the storage costs are noticably better: 1797559/2254918 ~ slightly more than 20% reduction in storage cost. 

Mode 6 might look like another viable candidate, but we pick level 5 as level 6 clearly is at the start of the CPU cost "knee" in the time cost curve and we should keep a little bit of margin for our conclusion and decision-making process is based on a single PDF sample right now, so there's plenty opportunity for mistakes through the large potential variance.


## Quick check with another PDF?




