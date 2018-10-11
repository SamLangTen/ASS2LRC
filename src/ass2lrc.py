from ass_interaction import *
import getopt


def display_usage(name):
    print("usage: python " + name +
          " [--help] -i <inputfile> -o <outputfile> [-t mileseconds] [-s stylename] [-c|-a] [-f [mm:ss.xx]]")
    print("")
    print("-i --input\tAn ASS filename which will be converted to lrc")
    print("-o --output\tA LRC filename where the convered text will be stored")
    print("-t --split_timespan\tA new blank line will be added if the start time of two between dialogues greater than this argument, default is 1000ms")
    print("-s --convert_styles\tA filter to decide which style of dialogue in ASS should be converted into LRC file")
    print("-c --compact\tCombine timestamp of dialogues with same text, not supported by all players. Not compatible with accurate option")
    print("-a --accurate\tConvert Karaoke-tag of ASS file to accurate word-by-word timestamp, not supported by all players. Not compatible with compact option")
    print("-f --accurate_format\tUsed with accurate option, to set format of accurate word-by-word timestamp since every player may have its own accurate timestamp")

if __name__ == "__main__":
    inputfilename = ""
    outputfilename = ""
    split_timespan = 1000
    convert_styles = list()
    is_compact = False
    accurate_format = None
    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], "hi:o:t:s:caf:", [
                                   "help", "input=", "output=", "split_timespan=", "convert_styles=","compact","accurate","accurate_format="])
    except getopt.GetoptError:
        display_usage(sys.argv[0])
        sys.exit(2)
    for opt, arg in opts:
        if (opt in ("-i", "--input")):
            inputfilename = arg
        elif (opt in ("-o", "--output")):
            outputfilename = arg
        elif (opt in ("-t", "--split_timespan")):
            split_timespan = int(arg)
        elif (opt in ("-s", "--convert_styles")):
            convert_styles.extend(arg.split(" "))
        elif (opt in ("-c", "--compact")):
            is_compact = True
        elif (opt in ("-a", "--accurate")):
            if accurate_format is None:
                accurate_format = "[mm:ss.xx]"
        elif (opt in ("-f", "--accurate_format")):
            accurate_format = arg
        elif (opt in ("-h", "--help")):
            display_usage(sys.argv[0])
            sys.exit(2)

    if inputfilename == "" or outputfilename == "":
        display_usage(sys.argv[0])
        sys.exit(2)
    # convert
    dialogues = ass_dialogue.load_lrc(inputfilename)
    # if convert styles set
    if len(convert_styles) != 0:
        dialogues = ass_styles_filter(dialogues, convert_styles)
    #convert: normal, compact, accurate
    if is_compact:
        output_text = convert_to_compact_lrc(dialogues, split_timespan)
    elif accurate_format is not None:
        output_text = convert_to_accurate_lrc(dialogues, split_timespan, accurate_format)
    else:
        output_text = convert_to_normal_lrc(dialogues, split_timespan)
    fout = open(outputfilename, "w")
    fout.write(output_text)
    fout.close()
