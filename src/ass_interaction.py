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
                # extract effect code: (\{[^{]*\}){0,1}[^{]*
                no_k_text = ""
                last_text = ""
                last_text = last_text.join(phrased[9:])
                while True:
                    first_re = re.match(r"(\{[^{]*\}){0,1}[^{]*", last_text)
                    if first_re is None or first_re.group() == "":
                        break
                    re_start_index = last_text.find(first_re.group())
                    re_end_index = last_text.find(first_re.group()) + len(first_re.group())
                    last_text = last_text[:re_start_index] + last_text[re_end_index:]
                    item = first_re.group()
                    # get K
                    ec_match = re.match(r"\{\\[K|k]\d*\}", item)
                    if (ec_match is not None):
                        word = ass_dialogue_word(item.replace(re.match(r"\{\\[K|k]\d*\}", item).group(), ""), start_time + datetime.timedelta(
                            milliseconds=int(ec_match.group().lower().replace("{\k", "").replace("}", "")) * 10))
                        if not word.word == "":
                            words.append(word)
                    else:
                        # clear effect code
                        ec2_match = re.match(r"\{[^\{|\}]*\}", item)
                        if ec2_match is not None:
                            no_k_text += item.replace(ec2_match.group(), "")
                        else:
                            no_k_text += item
                # if no effect code
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


def convert_to_compact_lrc(dialogues, space_end_timespan_ms):
    generated_text = ""
    dialogues.sort()
    blanks_dialogues = list()
    # Add blank line
    for index, item in enumerate(dialogues):
        if not index == len(dialogues) - 1:
            td = dialogues[index + 1].start_time - item.end_time
            if td.total_seconds() * 1000 > space_end_timespan_ms:
                lryic = __generate_lrc_timestamp(item.start_time)
                blanks_dialogues.append(lryic)
    # Generate
    i = 0
    while len(dialogues) > 0:
        this_line_generated_text = __generate_lrc_timestamp(
            dialogues[i].start_time)
        j = i + 1
        this_line_text = __convert_ass_effect_word_to_text(dialogues[i].text)
        while j < len(dialogues):
            if __convert_ass_effect_word_to_text(dialogues[j].text) == this_line_text:
                this_line_generated_text += __generate_lrc_timestamp(
                    dialogues[j].start_time)
                dialogues.pop(j)
            else:
                j += 1
        this_line_generated_text += this_line_text + "\n"
        generated_text += this_line_generated_text
        dialogues.pop(i)
    generated_text += "".join(blanks_dialogues) + " \n"
    return generated_text


def __generate_lrc_timestamp(dt):
    return "[" + str(dt.minute).zfill(2) + ":" + \
        str(dt.second).zfill(2) + "." + \
        str(int(dt.microsecond/10000)).zfill(2) + "]"


def __convert_ass_effect_word_to_text(text):
    if isinstance(text, list):
        text2 = ""
        for item2 in text:
            text2 += item2.word
        return text2
    else:
        return text


def convert_to_normal_lrc(dialogues, space_end_timespan_ms):
    generated_text = ""
    dialogues.sort()
    for index, item in enumerate(dialogues):
        lryic = __generate_lrc_timestamp(item.start_time)
        if isinstance(item.text, list):
            text = ""
            for item2 in item.text:
                text += item2.word
            lryic += text
        else:
            lryic += item.text
        generated_text += lryic + "\n"
        # append ending
        if not index == len(dialogues) - 1:
            td = dialogues[index + 1].start_time - item.end_time
            if td.total_seconds() * 1000 > space_end_timespan_ms:
                lryic = __generate_lrc_timestamp(item.start_time)
                generated_text += lryic + "\n"
        else:
            lryic = __generate_lrc_timestamp(item.start_time)
            generated_text += lryic + "\n"
    return generated_text
