#!/bin/bash
# creez executabilul 
#creez directoarele necesare
mkdir tests 2> /dev/null
mkdir tests/rotate 2> /dev/null
mkdir tests/load 2> /dev/null
mkdir tests/select 2> /dev/null
mkdir tests/crop 2> /dev/null
mkdir tests/histogram 2> /dev/null
mkdir tests/equalize 2> /dev/null
mkdir tests/rotate 2> /dev/null
mkdir tests/crop 2> /dev/null
mkdir tests/apply 2> /dev/null
mkdir tests/save 2> /dev/null
mkdir tests/exit 2> /dev/null
# creez imaginile
scripts/create_images.sh
# testele sunt pentru toate cazurile din fisiser
# test LOAD
progName="";
if [ $# == 1 ]
then
    progName=$1;
else
    progName="./main";
fi
echo "Execut test LOAD"
echo "---"
scripts/test_load.sh $progName
echo "---"
echo "Test LOAD executat cu succes"
echo "==="
# test SELECT
echo "Execut test SELECT"
echo "---"
scripts/test_select.sh $progName
echo "---"
echo "Test SELECT executat cu succes"
echo "==="
# test SELECT ALL
echo "Execut test SELECT ALL"
echo "---"
scripts/test_select_all.sh $progName
echo "---"
echo "Test SELECT ALL executat cu succes"
echo "==="
# test HISTOGRAM
echo "Execut test HISTOGRAM"
echo "---"
scripts/test_histogram.sh $progName
echo "---"
echo "Test HISTOGRAM executat cu succes"
echo "==="
# test EQUALIZE
echo "Execut test EQUALIZE"
echo "---"
scripts/test_equalize.sh $progName
echo "---"
echo "Test EQUALIZE executat cu succes"
echo "==="
# test ROTATE
echo "Execut test ROTATE"
echo "---"
scripts/test_rotate.sh $progName
echo "---"
echo "Test ROTATE executat cu succes"
echo "==="
# test CROP
echo "Execut test CROP"
echo "---"
scripts/test_crop.sh $progName
echo "---"
echo "Test CROP executat cu succes"
echo "==="
# test APPLY
echo "Execut test APPLY"
echo "---"
scripts/test_apply.sh $progName
echo "---"
echo "Test APPLY executat cu succes"
echo "==="
# test SAVE
echo "Execut test SAVE"
echo "---"
scripts/test_save.sh $progName
echo "---"
echo "Test SAVE executat cu succes"
echo "==="
# test EXIT
echo "Execut test EXIT"
echo "---"
scripts/test_exit.sh $progName
echo "---"
echo "Test EXIT executat cu succes"
echo "==="
# test invalid command
echo "Execut test INVALID COMMAND"
echo "---"
scripts/test_invalid_command.sh $progName
echo "---"
echo "Test INVALID COMMAND executat cu succes"
echo "==="

echo "Toate testele au fost executate cu succes! Fisierele de output se afla in tests/"