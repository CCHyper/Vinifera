#pragma once

#include <vorbisfile.h>


/**
 *  Is the Vorbis library initalised and ready to use?
 */
extern bool VorbisInitialised;


/**
 *  Function pointers to the Vorbis exports required.
 */
extern LPOVCLEAR ov_clear;
extern LPOVFOPEN ov_fopen;
extern LPOVOPEN ov_open;
extern LPOVOPENCALLBACKS ov_open_callbacks;
extern LPOVTEST ov_test;
extern LPOVTESTCALLBACKS ov_test_callbacks;
extern LPOVTESTOPEN ov_test_open;
extern LPOVBITRATE ov_bitrate;
extern LPOVBITRATE_INSTANT ov_bitrate_instant;
extern LPOVSTREAMS ov_streams;
extern LPOVSEEKABLE ov_seekable;
extern LPOVSERIALNUMBER ov_serialnumber;
extern LPOVRAWTOTAL ov_raw_total;
extern LPOVPCMTOTAL ov_pcm_total;
extern LPOVTIMETOTAL ov_time_total;
extern LPOVRAWSEEK ov_raw_seek;
extern LPOVPCMSEEK ov_pcm_seek;
extern LPOVPCMSEEK_PAGE ov_pcm_seek_page;
extern LPOVTIMESEEK ov_time_seek;
extern LPOVTIMESEEKPAGE ov_time_seek_page;
extern LPOVRAWSEEKLAP ov_raw_seek_lap;
extern LPOVPCMSEEKLAP ov_pcm_seek_lap;
extern LPOVPCMSEEKPAGELAP ov_pcm_seek_page_lap;
extern LPOVTIMESEEKLAP ov_time_seek_lap;
extern LPOVTIMESEEKPAGELAP ov_time_seek_page_lap;
extern LPOVRAWTELL ov_raw_tell;
extern LPOVPCMTELL ov_pcm_tell;
extern LPOVTIMETELL ov_time_tell;
extern LPOVINFO ov_info;
extern LPOVCOMMENT ov_comment;
extern LPOVREADFLOAT ov_read_float;
extern LPOVREADFILTER ov_read_filter;
extern LPOVREAD ov_read;
extern LPOVCROSSLAP ov_crosslap;
extern LPOVHALFRATE ov_halfrate;
extern LPOVHALFRATE_P ov_halfrate_p;
