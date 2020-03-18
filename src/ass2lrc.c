#include <stdlib.h>
#include <ass/ass.h>
#include <ass/ass_types.h>

#include "lrc_type.h"
#include "lrc.h"

int ass_event_asccmp(const ASS_Event *a, const ASS_Event *b)
{
    return (a->Start > b->Start) ? 1 : 0;
}

int main(int argc, char *argv[])
{
    ASS_Library *ass_library = ass_library_init();
    if (!ass_library)
    {
        printf("Failed to load ass_library\n");
        exit(1);
    }

    ASS_Track *track = ass_read_file(ass_library, argv[1], NULL);
    if (!track)
    {
        printf("Failed to load ASS file\n");
        exit(1);
    }

    qsort(track->events, track->n_events, sizeof(ASS_Event), ass_event_asccmp);
    lrc_file *lrc = lrc_parse_ass_subtitle(track, 1000);
    for (int i = 0; i < lrc->n_sentence; i++)
    {
        printf("%d: %lld, %s\n", i + 1, lrc->sentences[i].start, lrc->sentences[i].is_accurate ? "AC" : "NA");
        if (!lrc->sentences[i].is_accurate)
        {
            printf("  %s\n\n", lrc->sentences[i].content.text);
            continue;
        }

        for (int j = 0; j < lrc->sentences[i].content.word.n_word; j++)
        {
            printf("  %s for %lldms\n", lrc->sentences[i].content.word.words[j].text, lrc->sentences[i].content.word.words[j].duration);
        }
        printf("\n");
    }

    ass_free_track(track);
    ass_library_done(ass_library);
    return 0;
}