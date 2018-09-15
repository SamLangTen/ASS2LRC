from ass_interaction import *

if __name__ == "__main__":
    #filename = sys.argv[1]
    filename = "test.ass"
    dialogues = ass_dialogue.load_lrc(filename)
    print(convert_to_normal_lrc(dialogues, 10))
    
