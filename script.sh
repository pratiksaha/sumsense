#!/bin/bash

SRC=`zenity --file-selection --text="Select Input File" --title="Select a File"`


PRC=`zenity --entry --text="Enter Percentage here" --title="Enter Percentage"`

cp -f $SRC /home/pratiksaha/sumsensev5.2/inp.txt

./sumsense inp.txt $PRC | (zenity --progress --pulsate --title="SUMSENSE")

OPT=`zenity --question --text "Would you like to see the summary now?"`

echo $OPT

CMD="gedit output.txt"
if [$OPT -eq0 ];
	then
		$CMD
else
	`zenity --info --text "The summary is stored in output.txt"`
fi



