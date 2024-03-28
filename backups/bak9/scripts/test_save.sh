#!/bin/bash
progName=$1;
$progName < test_inputs/test_save.txt
# verific daca am salvat bine imaginea P2
# diff images/p2.pgm tests/save/p2.pgm
convert images/p2.pgm p2.bmp 
convert tests/save/p2.pgm p2_save.bmp
diff p2.bmp p2_save.bmp

# verific daca am salvat bine imaginea P3
# diff images/p3.ppm tests/save/p3.ppm
convert images/p3.ppm p3.bmp
convert tests/save/p3.ppm p3_save.bmp
diff p3.bmp p3_save.bmp

# verific daca am salvat bine imaginea P5
# diff images/p5.pgm tests/save/p5.pgm
convert images/p5.pgm p5.bmp
convert tests/save/p5.pgm p5_save.bmp
diff p5.bmp p5_save.bmp

# verific daca am salvat bine imaginea P6
# diff images/p6.ppm tests/save/p6.ppm
convert images/p6.ppm p6.bmp
convert tests/save/p6.ppm p6_save.bmp
diff p6.bmp p6_save.bmp
#cleanup
rm *.bmp