/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation https://xiph.org/                     *
 *                                                                  *
 ********************************************************************

 function: stdio-based convenience library for opening/seeking/decoding

 ********************************************************************/

#ifndef _OV_FILE_H_
#define _OV_FILE_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <vorbiscodec.h>

#if !defined(VORBIS_APIENTRY)
  #define VORBIS_APIENTRY __cdecl
#endif

/* The function prototypes for the callbacks are basically the same as for
 * the stdio functions fread, fseek, fclose, ftell.
 * The one difference is that the FILE * arguments have been replaced with
 * a void * - this is to be used as a pointer to whatever internal data these
 * functions might need. In the stdio case, it's just a FILE * cast to a void *
 *
 * If you use other functions, check the docs for these functions and return
 * the right values. For seek_func(), you *MUST* return -1 if the stream is
 * unseekable
 */
typedef struct {
  size_t (VORBIS_APIENTRY *read_func)  (void *ptr, size_t size, size_t nmemb, void *datasource);
  int    (VORBIS_APIENTRY *seek_func)  (void *datasource, ogg_int64_t offset, int whence);
  int    (VORBIS_APIENTRY *close_func) (void *datasource);
  long   (VORBIS_APIENTRY *tell_func)  (void *datasource);
} ov_callbacks;

#ifndef OV_EXCLUDE_STATIC_CALLBACKS

/* a few sets of convenient callbacks, especially for use under
 * Windows where ov_open_callbacks() should always be used instead of
 * ov_open() to avoid problems with incompatible crt.o version linking
 * issues. */

static int _ov_header_fseek_wrap(FILE *f,ogg_int64_t off,int whence){
  if(f==NULL)return(-1);

#ifdef __MINGW32__
  return fseeko64(f,off,whence);
#elif defined (_WIN32)
  return _fseeki64(f,off,whence);
#else
  return fseek(f,off,whence);
#endif
}

/* These structs below (OV_CALLBACKS_DEFAULT etc) are defined here as
 * static data. That means that every file which includes this header
 * will get its own copy of these structs whether it uses them or
 * not unless it #defines OV_EXCLUDE_STATIC_CALLBACKS.
 * These static symbols are essential on platforms such as Windows on
 * which several different versions of stdio support may be linked to
 * by different DLLs, and we need to be certain we know which one
 * we're using (the same one as the main application).
 */

static ov_callbacks OV_CALLBACKS_DEFAULT = {
  (size_t (VORBIS_APIENTRY *)(void *, size_t, size_t, void *))  fread,
  (int (VORBIS_APIENTRY *)(void *, ogg_int64_t, int))           _ov_header_fseek_wrap,
  (int (VORBIS_APIENTRY *)(void *))                             fclose,
  (long (VORBIS_APIENTRY *)(void *))                            ftell
};

static ov_callbacks OV_CALLBACKS_NOCLOSE = {
  (size_t (VORBIS_APIENTRY *)(void *, size_t, size_t, void *))  fread,
  (int (VORBIS_APIENTRY *)(void *, ogg_int64_t, int))           _ov_header_fseek_wrap,
  (int (VORBIS_APIENTRY *)(void *))                             NULL,
  (long (VORBIS_APIENTRY *)(void *))                            ftell
};

static ov_callbacks OV_CALLBACKS_STREAMONLY = {
  (size_t (VORBIS_APIENTRY *)(void *, size_t, size_t, void *))  fread,
  (int (VORBIS_APIENTRY *)(void *, ogg_int64_t, int))           NULL,
  (int (VORBIS_APIENTRY *)(void *))                             fclose,
  (long (VORBIS_APIENTRY *)(void *))                            NULL
};

static ov_callbacks OV_CALLBACKS_STREAMONLY_NOCLOSE = {
  (size_t (VORBIS_APIENTRY *)(void *, size_t, size_t, void *))  fread,
  (int (VORBIS_APIENTRY *)(void *, ogg_int64_t, int))           NULL,
  (int (VORBIS_APIENTRY *)(void *))                             NULL,
  (long (VORBIS_APIENTRY *)(void *))                            NULL
};

#endif

#define  NOTOPEN   0
#define  PARTOPEN  1
#define  OPENED    2
#define  STREAMSET 3
#define  INITSET   4

typedef struct OggVorbis_File {
  void            *datasource; /* Pointer to a FILE *, etc. */
  int              seekable;
  ogg_int64_t      offset;
  ogg_int64_t      end;
  ogg_sync_state   oy;

  /* If the FILE handle isn't seekable (eg, a pipe), only the current
     stream appears */
  int              links;
  ogg_int64_t     *offsets;
  ogg_int64_t     *dataoffsets;
  long            *serialnos;
  ogg_int64_t     *pcmlengths; /* overloaded to maintain binary
                                  compatibility; x2 size, stores both
                                  beginning and end values */
  vorbis_info     *vi;
  vorbis_comment  *vc;

  /* Decoding working state local storage */
  ogg_int64_t      pcm_offset;
  int              ready_state;
  long             current_serialno;
  int              current_link;

  double           bittrack;
  double           samptrack;

  ogg_stream_state os; /* take physical pages, weld into a logical
                          stream of packets */
  vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
  vorbis_block     vb; /* local working space for packet->PCM decode */

  ov_callbacks callbacks;

} OggVorbis_File;

typedef int (VORBIS_APIENTRY *LPOVCLEAR)(OggVorbis_File *vf);                                          // ov_clear
typedef int (VORBIS_APIENTRY *LPOVFOPEN)(const char *path,OggVorbis_File *vf);                         // ov_fopen
typedef int (VORBIS_APIENTRY *LPOVOPEN)(FILE *f,OggVorbis_File *vf,const char *initial,long ibytes);   // ov_open
typedef int (VORBIS_APIENTRY *LPOVOPENCALLBACKS)(void *datasource, OggVorbis_File *vf,                 // ov_open_callbacks
                const char *initial, long ibytes, ov_callbacks callbacks);

typedef int (VORBIS_APIENTRY *LPOVTEST)(FILE *f,OggVorbis_File *vf,const char *initial,long ibytes);   // ov_test
typedef int (VORBIS_APIENTRY *LPOVTESTCALLBACKS)(void *datasource, OggVorbis_File *vf,                 // ov_test_callbacks
                const char *initial, long ibytes, ov_callbacks callbacks);
typedef int (VORBIS_APIENTRY *LPOVTESTOPEN)(OggVorbis_File *vf);                                       // ov_test_open

typedef long (VORBIS_APIENTRY *LPOVBITRATE)(OggVorbis_File *vf,int i);                                 // ov_bitrate
typedef long (VORBIS_APIENTRY *LPOVBITRATE_INSTANT)(OggVorbis_File *vf);                               // ov_bitrate_instant
typedef long (VORBIS_APIENTRY *LPOVSTREAMS)(OggVorbis_File *vf);                                       // ov_streams
typedef long (VORBIS_APIENTRY *LPOVSEEKABLE)(OggVorbis_File *vf);                                      // ov_seekable
typedef long (VORBIS_APIENTRY *LPOVSERIALNUMBER)(OggVorbis_File *vf,int i);                            // ov_serialnumber

typedef ogg_int64_t (VORBIS_APIENTRY *LPOVRAWTOTAL)(OggVorbis_File *vf,int i);                         // ov_raw_total
typedef ogg_int64_t (VORBIS_APIENTRY *LPOVPCMTOTAL)(OggVorbis_File *vf,int i);                         // ov_pcm_total
typedef double (VORBIS_APIENTRY *LPOVTIMETOTAL)(OggVorbis_File *vf,int i);                             // ov_time_total

typedef int (VORBIS_APIENTRY *LPOVRAWSEEK)(OggVorbis_File *vf,ogg_int64_t pos);                        // ov_raw_seek
typedef int (VORBIS_APIENTRY *LPOVPCMSEEK)(OggVorbis_File *vf,ogg_int64_t pos);                        // ov_pcm_seek
typedef int (VORBIS_APIENTRY *LPOVPCMSEEK_PAGE)(OggVorbis_File *vf,ogg_int64_t pos);                   // ov_pcm_seek_page
typedef int (VORBIS_APIENTRY *LPOVTIMESEEK)(OggVorbis_File *vf,double pos);                            // ov_time_seek
typedef int (VORBIS_APIENTRY *LPOVTIMESEEKPAGE)(OggVorbis_File *vf,double pos);                        // ov_time_seek_page

typedef int (VORBIS_APIENTRY *LPOVRAWSEEKLAP)(OggVorbis_File *vf,ogg_int64_t pos);                     // ov_raw_seek_lap
typedef int (VORBIS_APIENTRY *LPOVPCMSEEKLAP)(OggVorbis_File *vf,ogg_int64_t pos);                     // ov_pcm_seek_lap
typedef int (VORBIS_APIENTRY *LPOVPCMSEEKPAGELAP)(OggVorbis_File *vf,ogg_int64_t pos);                 // ov_pcm_seek_page_lap
typedef int (VORBIS_APIENTRY *LPOVTIMESEEKLAP)(OggVorbis_File *vf,double pos);                         // ov_time_seek_lap
typedef int (VORBIS_APIENTRY *LPOVTIMESEEKPAGELAP)(OggVorbis_File *vf,double pos);                     // ov_time_seek_page_lap

typedef ogg_int64_t (VORBIS_APIENTRY *LPOVRAWTELL)(OggVorbis_File *vf);                                // ov_raw_tell
typedef ogg_int64_t (VORBIS_APIENTRY *LPOVPCMTELL)(OggVorbis_File *vf);                                // ov_pcm_tell
typedef double (VORBIS_APIENTRY *LPOVTIMETELL)(OggVorbis_File *vf);                                    // ov_time_tell

typedef vorbis_info *(VORBIS_APIENTRY *LPOVINFO)(OggVorbis_File *vf,int link);                         // ov_info
typedef vorbis_comment *(VORBIS_APIENTRY *LPOVCOMMENT)(OggVorbis_File *vf,int link);                   // ov_comment

typedef long (VORBIS_APIENTRY *LPOVREADFLOAT)(OggVorbis_File *vf,float ***pcm_channels,int samples,    // ov_read_float
                          int *bitstream);
typedef long (VORBIS_APIENTRY *LPOVREADFILTER)(OggVorbis_File *vf,char *buffer,int length,             // ov_read_filter
                          int bigendianp,int word,int sgned,int *bitstream,
                          void (*filter)(float **pcm,long channels,long samples,void *filter_param),void *filter_param);
typedef long (VORBIS_APIENTRY *LPOVREAD)(OggVorbis_File *vf,char *buffer,int length,                   // ov_read
                    int bigendianp,int word,int sgned,int *bitstream);
typedef int (VORBIS_APIENTRY *LPOVCROSSLAP)(OggVorbis_File *vf1,OggVorbis_File *vf2);                  // ov_crosslap

typedef int (VORBIS_APIENTRY *LPOVHALFRATE)(OggVorbis_File *vf,int flag);                              // ov_halfrate
typedef int (VORBIS_APIENTRY *LPOVHALFRATE_P)(OggVorbis_File *vf);                                     // ov_halfrate_p

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

