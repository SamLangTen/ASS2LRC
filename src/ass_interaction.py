import sys
import datetime
import re
import time


class ass_dialogue_word(object):

    def __init__(self, word, start_time):
        self.word = word
        self.start_time = start_time

class ass_dialogue(object):

    @staticmethod
    def load_lrc(filename):
        dialogues = list()
        files = open(filename, "r")
        for line in files:
            if line.startswith("Dialogue:"):
                phrased = line.split(",")
                start_time = datetime.datetime(1, 1, 1, int(phrased[1].split(":")[0]), int(phrased[1].split(":")[1]), int(
                    phrased[1].split(":")[2].split(".")[0]), int(phrased[1].split(":")[2].split(".")[1])*10000)
                end_time = datetime.datetime(1, 1, 1, int(phrased[2].split(":")[0]), int(phrased[2].split(":")[1]), int(
                    phrased[2].split(":")[2].split(".")[0]), int(phrased[2].split(":")[2].split(".")[1])*10000)
                style = phrased[3]
                words = list()
                #extract effect code: (\{[^{]*\}){0,1}[^{]*
                no_k_text = ""
                last_text = ""
                last_text = last_text.join(phrased[9:])
                while True:
                    first_re = re.match(r"(\{[^{]*\}){0,1}[^{]*", last_text)
                    if first_re is None or first_re.group() == "":
                        break
                    last_text = last_text.replace(first_re.group(), "")
                    item = first_re.group()
                    #get K
                    ec_match = re.match(r"\{\\[K|k]\d*\}", item)
                    if (ec_match is not None):
                        word = ass_dialogue_word(item.replace(re.match(r"\{\\[K|k]\d*\}", item).group(), ""), start_time + datetime.timedelta(
                            milliseconds=int(ec_match.group().lower().replace("{\k", "").replace("}", "")) * 10))
                        if not word.word == "":
                            words.append(word)
                    else:
                        #clear effect code
                        ec2_match = re.match(r"\{[^\{|\}]*\}", item)
                        if ec2_match is not None:
                            no_k_text += item.replace(ec2_match.group(), "")
                        else:
                            no_k_text += item
                #if no effect code
                if len(words) == 0:
                    words = no_k_text.strip()
                dialogue = ass_dialogue(start_time, end_time, style, words)
                dialogues.append(dialogue)
        files.close()
        return dialogues

    def __lt__(self, other):  
        if self.start_time < other.start_time:
            return True
        return False

    def __init__(self, start_time, end_time, style, text):
        self.start_time = start_time
        self.end_time = end_time
        self.style = style
        self.text = text

def ass_styles_filter(dialogues, styles):
    new_dialogues = list()
    for style_item in dialogues:
        if style_item.style in styles:
            new_dialogues.append(style_item)
    return new_dialogues

def convert_to_normal_lrc(dialogues, space_end_timespan_ms):
    generated_text = ""
    dialogues.sort()
    for index, item in enumerate(dialogues):
        lryic = "[" + str(item.start_time.minute).zfill(2) + ":" + \
            str(item.start_time.second).zfill(2) + "." + \
            str(int(item.start_time.microsecond/10000)).zfill(2) + "]"
        if isinstance(item.text, list):
            text = ""
            for item2 in item.text:
                text += item2.word
            lryic += text
        else:
            lryic += item.text
        generated_text += lryic + "\n"
        #append ending
        if not index == len(dialogues) - 1:
            td = dialogues[index + 1].start_time - item.end_time
            if td.total_seconds() * 1000 > space_end_timespan_ms:
                lryic = "[" + str(item.end_time.minute).zfill(2) + ":" + \
                    str(item.end_time.second).zfill(2) + "." + \
                    str(int(item.end_time.microsecond / 10000)).zfill(2) + "]"
                generated_text += lryic + "\n"
        else:
            lryic = "[" + str(item.end_time.minute).zfill(2) + ":" + \
                    str(item.end_time.second).zfill(2) + "." + \
                    str(int(item.end_time.microsecond / 10000)).zfill(2) + "]"
            generated_text += lryic + "\n"
    return generated_text
