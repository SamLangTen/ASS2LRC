#ifndef _LRC_H_
#define _LRC_H_

typedef struct _lrc_word
{
    long long duration;
    char *text;
} lrc_word;

typedef struct _lrc_word_list
{
    lrc_word *words;
    int n_word;
} lrc_word_list;

typedef struct _lrc_sentence
{
    union _content {
        lrc_word_list word;
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


void lrc_file_free(lrc_file *lrc);

#endif