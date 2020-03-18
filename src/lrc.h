#ifndef _LRC_H_
#define _LRC_H_

typedef enum _ASS_EFFECT_TOKEN
{
    TEXT,
    KARAOKE,
    EFFECT,
    END
} ASS_EFFECT_TOKEN;

lrc_file *lrc_parse_ass_subtitle(ASS_Track *ass_track, long long split_timespan);
void lrc_file_free(lrc_file *lrc);

#endif