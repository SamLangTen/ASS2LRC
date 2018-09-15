import sys
import datetime
import re
import time
class lrc_dialogue(object):

    def __init__(self, time, lryic):
        self.time = time
        self.lryic = lryic

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
                start_time = datetime.datetime(1, 1, 1, int(phrased[1].split(":")[0]), int(phrased[1].split(":")[1]), int(phrased[1].split(":")[2].split(".")[0]), int(phrased[1].split(":")[2].split(".")[1])*10000)
                end_time = datetime.datetime(1, 1, 1, int(phrased[2].split(":")[0]), int(phrased[2].split(":")[1]), int(phrased[2].split(":")[2].split(".")[0]), int(phrased[2].split(":")[2].split(".")[1])*10000)
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
                    last_text = last_text.replace(first_re.group(),"")
                    item = first_re.group()
                    #get K
                    ec_match = re.match(r"\{\\[K|k]\d*\}", item)
                    if (ec_match is not None):
                        word = ass_dialogue_word(item.replace(re.match(r"\{\\[K|k]\d*\}", item).group(), ""), start_time + datetime.timedelta(milliseconds=int(ec_match.group().lower().replace("{\k","").replace("}","")) * 10))
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
                    words = no_k_text
                dialogue = ass_dialogue(start_time, end_time, style, words)
                dialogues.append(dialogue)
        files.close()
        return dialogues

    def __init__(self, start_time, end_time, style, text):
        self.start_time = start_time
        self.end_time = end_time
        self.style = style
        self.text = text

def convert_to_standard_lrc():
    print("h")

if __name__ == "__main__":
    #filename = sys.argv[1]
    filename = "test.ass"
    dialogues = ass_dialogue.load_lrc(filename)
    for item in dialogues:
        print(item.start_time)
        print(item.end_time)
        print(item.style)
        if isinstance(item.text, list):
            for word in item.text:
                print(word.start_time.strftime("%Y-%m-%d %H:%M:%S.%f") + ' ' + word.word)
        else:
            print(item.text)
