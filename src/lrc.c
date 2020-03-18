#include "lrc_type.h"
#include <ass/ass_types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
        *token_info = 0;
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

                *token_info = 0;
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

static lrc_file *lrc_parse_ass_subtitle(ASS_Track *ass_track, long long split_timespan)
{
    for (int i = 0; i < ass_track->n_events; i++)
    {
        printf("%d: %s\n", i + 1, ass_track->events[i].Text);
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
                printf("TEXT: %s\n", (char *)token_info);
                free(token_info);
                break;
            case KARAOKE:
                printf("KARAOKE: %lldms\n", *(long long *)token_info);
                free(token_info);
                break;
            case EFFECT:
                printf("EFFECT\n");
                break;
            default:
                break;
            }
        }
        printf("\n");
    }
}