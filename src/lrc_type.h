#ifndef _LRC_TYPE_H_
#define _LRC_TYPE_H_

typedef struct _lrc_word
{
    long long start;
    char *text;
} lrc_word;

typedef struct _lrc_sentence
{
    union _content {
        lrc_word word;
        char *text;
    } content;
    unsigned int is_accurate;
    long long start;
} lrc_sentence;

typedef struct _lrc_file
{
    int n_sentence;
    lrc_sentence *sentences;
    char *album;
    char *artist;
    char *author;
    char *file_maker;
    long long offset;
    char *file_program_name;
    char *title;
    char *file_program_version;
} lrc_file;

#endif