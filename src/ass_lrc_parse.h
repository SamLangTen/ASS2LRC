#include <ass/ass_types.h>
#include "lrc.h"

#ifndef _ASS_LRC_PARSE_H_
#define _ASS_LRC_PARSE_H_

lrc_file *lrc_parse_ass_subtitle(ASS_Track *ass_track, long long split_timespan);

#endif