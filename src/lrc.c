#include <stdlib.h>
#include "lrc.h"



void lrc_file_free(lrc_file *lrc)
{
    for (int i = 0; i < lrc->n_sentence; i++)
    {
        if (lrc->sentences[i].is_accurate == 0)
        {
            free(lrc->sentences[i].content.text);
            continue;
        }

        /* Free Accurate Mode Resource */
        for (int j = 0; j < lrc->sentences[i].content.word.n_word; j++)
        {
            free(lrc->sentences[i].content.word.words[j].text);
        }
        free(lrc->sentences[i].content.word.words);
    }
    free(lrc->sentences);
    if (!lrc->album)
        free(lrc->album);
    if (!lrc->artist)
        free(lrc->artist);
    if (!lrc->title)
        free(lrc->title);
    if (!lrc->file_maker)
        free(lrc->file_maker);
    if (!lrc->file_program_name)
        free(lrc->file_program_name);
    if (!lrc->file_program_version)
        free(lrc->file_program_version);
    if (!lrc->title)
        free(lrc->title);
}