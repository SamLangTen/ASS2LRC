#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <ass/ass.h>
#include <ass/ass_types.h>

#include "ass_lrc_parse.h"
#include "lrc.h"

int main(int argc, char *argv[])
{
    int option;
    char *inputfile = NULL;
    char *outputfile = NULL;
    long long split_time = 1000;
    while ((option = getopt(argc, argv, "i:o:t:")) != -1)
    {
        switch (option)
        {
        case 'i':
            inputfile = optarg;
            break;
        case 'o':
            outputfile = optarg;
            break;
        case 't':
            if (optarg != NULL)
                split_time = atoi(optarg);
            break;
        default:
            break;
        }
    }
    if (inputfile == NULL)
    {
        fprintf(stderr, "Input file must be specified.\n");
        exit(1);
    }
    if (outputfile == NULL)
    {
        fprintf(stderr, "Output file must be specified.\n");
        exit(1);
    }

    ASS_Library *ass_library = ass_library_init();
    if (!ass_library)
    {
        fprintf(stderr, "Failed to load ass_library.\n");
        exit(1);
    }

    ASS_Track *track = ass_read_file(ass_library, inputfile, NULL);
    if (!track)
    {
        fprintf(stderr, "Failed to load ASS file.\n");
        exit(1);
    }

    lrc_file *lrc = lrc_parse_ass_subtitle(track, split_time);

    /* Output */
    FILE *outfile = fopen(outputfile, "w+");
    for (int i = 0; i < lrc->n_sentence; i++)
    {
        int centisecond = (int)(lrc->sentences[i].start % 1000);
        centisecond /= 10;
        int second = (int)((lrc->sentences[i].start / 1000) % 60);
        int minute = (int)((lrc->sentences[i].start / 1000 / 60) % 60);
        fprintf(outfile, "[%02d:%02d.%02d]", minute, second, centisecond);

        if (lrc->sentences[i].is_accurate)
        {
            for (int j = 0; j < lrc->sentences[i].content.word.n_word; j++)
            {
                fprintf(outfile, "%s", lrc->sentences[i].content.word.words[j].text);
            }
        }
        else
        {
            fprintf(outfile, "%s", lrc->sentences[i].content.text);
        }
        fprintf(outfile, "\n");
    }

    lrc_file_free(lrc);
    ass_free_track(track);
    ass_library_done(ass_library);
    return 0;
}