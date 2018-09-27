# ASS2LRC
This script can convert ASS subtitle file to common LRC lryics file.

    usage: python .\ass2lrc.py [--help] -i <inputfile> -o <outputfile> [-t mileseconds] [-s stylename] [-c]
    
    -i --input              An ASS filename which will be converted to lrc
    -o --output             A LRC filename where the convered text will be stored
    -t --split_timespan     A new blank line will be added if the start time of two between dialogues greater than this argument, default is 1000ms
    -s --convert_styles     A filter to decide which style of dialogue in ASS should be converted into LRC file
    -c --compact            Combine timestamp of dialogues with same text, not supported by all player

Since LRC file does not support ending timestamp, some lryics followed with a long period time of music will be "frozen". This script can add a new blank lyrics whose start timestamp is ending timestamp of the former lyrics. If timespan between the former ending time and the latter start time is greater than option split_timespan, a new blank line will be added.

Some ASS files may contain more than one kind of style because different style represents different kind of subtitle. Convert Styles setting can filter the dialogues with style which you want to convert into LRC dialogues.