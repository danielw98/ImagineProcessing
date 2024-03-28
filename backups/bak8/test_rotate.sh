#!/bin/bash
#creez directoarele daca nu exista, redirectionez STDERR la /dev/null sa il ignor
mkdir teste 2> /dev/null
mkdir teste/rotate 2> /dev/null
#creez executabilul
make noWarnings
# iau input-ul din fisier
./main < test_rotate.txt
# deschid toate imaginile si le vad
cd teste/rotate
display p2.pgm &
display p2_90_right.pgm &
display p2_180_left.pgm &
cd ..