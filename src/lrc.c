#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ass/ass_types.h>
#include <ctype.h>

#include "lrc_type.h"
#include "lrc.h"

typedef enum _ASS_EFFECT_TOKEN
{
    TEXT,
    KARAOKE,
    EFFECT,
    END
} ASS_EFFECT_TOKEN;

/*
 * An token analyze for ass event.
 * If current token is TEXT, *token_info point to a *char.
 * If current token is KARAOKE, *token_info point to a *long long.
 * If current token is EFFECT, *token_info set to NULL.
 */
static ASS_EFFECT_TOKEN ass_event_analyze(char *text, int *pos, void **token_info)
{
    char *buffer;

    if (text[*pos] == 0)
    {
        *token_info = NULL;
        return END;
    }

    if (text[*pos] == '{')
    {
        (*pos)++;
        if (text[*pos] == '\\')
        {
            (*pos)++;
            if (text[*pos] == 'k' || text[*pos] == 'K') /* Karaoke Effect */
            {
                long long karaoke_duration = 0;
                (*pos)++;

                int moved = 0;
                if (text[*pos] == 'o' || text[*pos] == 'f')
                {
                    (*pos)++;
                    moved++;
                }

                while (text[*pos] != '}')
                {
                    if (text[*pos] == 0)
                    {
                        (*pos) -= 2 + moved;
                        goto normal_text;
                    }

                    karaoke_duration = karaoke_duration * 10 + (text[*pos] - '0');
                    (*pos)++;
                    moved++;
                }

                (*pos)++;

                long long *p_karaoke_duration = (long long *)malloc(sizeof(long long));
                *p_karaoke_duration = karaoke_duration * 10;
                *token_info = p_karaoke_duration;
                return KARAOKE;
            }
            else /* Other Effect */
            {

                int moved = 0;
                /* Simply ignore */
                while (text[*pos] != '}')
                {

                    if (text[*pos] == 0)
                    {
                        (*pos) -= 2 + moved;
                        goto normal_text;
                    }

                    (*pos)++;
                    moved++;
                }
                (*pos)++;

                *token_info = NULL;
                return EFFECT;
            }
        }

        /* Start with { but no command, treated as normal text, read until next { */
        (*pos)--;
    }

normal_text:

    /* Normal Text */
    buffer = (char *)malloc(sizeof(char) * (strlen(text) + 1));
    memset(buffer, 0, strlen(text) + 1);
    int pos_buffer = 0;

    do
    {
        buffer[pos_buffer++] = text[*pos];
        (*pos)++;
    } while (text[*pos] != '{' && text[*pos] != 0);
    *token_info = buffer;
    return TEXT;
}

static int ass_event_asccmp(const ASS_Event *a, const ASS_Event *b)
{
    return (a->Start > b->Start) ? 1 : 0;
}

lrc_file *lrc_parse_ass_subtitle(ASS_Track *ass_track, long long split_timespan)
{
    lrc_file *lrc = (lrc_file *)malloc(sizeof(lrc_file));
    lrc->album = NULL;
    lrc->artist = NULL;
    lrc->author = NULL;
    lrc->file_maker = NULL;
    lrc->file_program_name = NULL;
    lrc->file_program_version = NULL;
    lrc->offset = 0;
    lrc->title = NULL;

    lrc->sentences = (lrc_sentence *)malloc(sizeof(lrc_sentence) * (ass_track->n_events * 2));
    lrc->n_sentence = 0;

    qsort(ass_track->events, ass_track->n_events, sizeof(ASS_Event), ass_event_asccmp);
    for (int i = 0; i < ass_track->n_events; i++)
    {

        int word_max = 5;
        int word_count = 0;
        lrc_word *words = (lrc_word *)malloc(sizeof(lrc_word) * word_max);
        words[0].duration = 0;

        int pos = 0;
        void *token_info;

        while (1)
        {

            ASS_EFFECT_TOKEN token = ass_event_analyze(ass_track->events[i].Text, &pos, &token_info);

            if (token == END)
                break;

            switch (token)
            {
            case TEXT:
                words[word_count].text = (char *)token_info;
                word_count++;

                if (word_count >= word_max)
                {
                    words = realloc(words, sizeof(lrc_word) * (word_max += 5));
                }

                break;
            case KARAOKE:
                words[word_count].duration = *(long long *)token_info;
                free(token_info);
                break;
            case EFFECT:
                break;
            default:
                break;
            }
        }

        /* Construct lrc_sentence */
        lrc_sentence *sen = &lrc->sentences[lrc->n_sentence];
        sen->start = ass_track->events[i].Start;

        if (word_count == 1 && words[0].duration == 0) /* No karaoke effect */
        {
            sen->content.text = words[0].text;
            sen->is_accurate = 0;
            free(words);
        }
        else
        {
            sen->content.word.n_word = word_count;
            sen->content.word.words = words;
            sen->is_accurate = 1;
        }
        lrc->n_sentence++;

        /* Check timespan */
        if (i == ass_track->n_events - 1 || ass_track->events[i + 1].Start - (ass_track->events[i].Start + ass_track->events[i].Duration) > split_timespan)
        {
            sen = &lrc->sentences[lrc->n_sentence];
            sen->is_accurate = 0;
            sen->start = ass_track->events[i].Start + ass_track->events[i].Duration;
            char *empty_str = (char *)malloc(sizeof(char));
            empty_str[0] = 0;
            sen->content.text = empty_str;
            lrc->n_sentence++;
        }
    }

    return lrc;
}

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
