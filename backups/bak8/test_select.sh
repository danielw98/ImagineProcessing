#!/bin/bash
#creez directoarele daca nu exista, redirectionez STDERR la /dev/null sa il ignor
mkdir teste 2> /dev/null
mkdir teste/apply 2> /dev/null
#creez executabilul
make noWarnings
# iau input-ul din fisier
./main < test_apply.txt
# deschid toate imaginile si le vad
cd teste/apply
display p3.ppm &
display p3_edge.ppm &
display p3_sharpen.ppm &
display p3_blur.ppm &
display p3_gaussian_blur.ppm &
