# ASS2LRC
This script can convert ASS subtitle file to common LRC lryics file.

`usage: python .\ass2lrc.py [--help] -i \<inputfile\> -o \<outputfile\> [-t mileseconds]`

`-i --input      An ASS filename which will be converted to lrc.`

`-o --output     A LRC filename where the convered text will be stored.`

`-t --split_timespan     A new blank line will be added if the start time of two between dialogues greater than this argument, default is 1000ms`

Since LRC file does not support ending timestamp, some lryics followed with a long period time of music will be "frozen". This script can add a new blank lyrics whose start timestamp is ending timestamp of the former lyrics. If timespan between the former ending time and the latter start time is greater than option split_timespan, a new blank line will be added.