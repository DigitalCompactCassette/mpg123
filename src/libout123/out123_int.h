/*
	out123_int: internal header for libout123

	copyright ?-2015 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written by Michael Hipp (some traces left)
*/

#ifndef _MPG123_OUT123_INT_H_
#define _MPG123_OUT123_INT_H_

#include "config.h"
#include "out123_intsym.h"
#include "compat.h"
#include "out123.h"
#include "module.h"

#ifndef NOXFERMEM
#include "xfermem.h"
#endif

/* 3% rate tolerance */
#define AUDIO_RATE_TOLERANCE	  3

/* Keep those internally? To the outside, it's just a selection of
   driver modules. */
enum {
	DECODE_TEST,  /* "test" */
	DECODE_AUDIO, /* gone */
	DECODE_FILE,  /* "raw" */
	DECODE_BUFFER, /* internal use only, if at all */
	DECODE_WAV,    /* wav */
	DECODE_AU,     /* au */
	DECODE_CDR,    /* cdr */
	DECODE_AUDIOFILE /* internal use only, if at all */
};

/* Playback states mostly for the buffer process.
   Maybe also used in main program. */
enum playstate
{
	play_dead = 0 /* nothing playing, nothing loaded */
,	play_stopped  /* driver present, but no device configured/opened */
,	play_paused   /* paused, ready to continue, device still active */
,	play_live     /* playing right now */
};

struct audio_output_struct
{
	enum out123_error errcode;
#ifndef NOXFERMEM
	/* If buffer_pid >= 0, there is a separate buffer process actually
	   handling everything, this instance here is then only a proxy. */
	int buffer_pid;
	int buffer_fd[2];
	txfermem *buffermem;
#endif

	int fn;			/* filenumber */
	void *userptr;	/* driver specific pointer */
	
	/* Callbacks */
	int (*open)(struct audio_output_struct *);
	int (*get_formats)(struct audio_output_struct *);
	int (*write)(struct audio_output_struct *, unsigned char *,int);
	void (*flush)(struct audio_output_struct *); /* flush == drop != drain */
	void (*drain)(struct audio_output_struct *);
	int (*close)(struct audio_output_struct *);
	int (*deinit)(struct audio_output_struct *);
	
	/* the loaded that has set the above */
	mpg123_module_t *module;

	char *driver;	/* driver (module) name */
	char *device;	/* device name */
	int   flags;	/* some bits; namely headphone/speaker/line */
	long rate;		/* sample rate */
	long gain;		/* output gain */
	int channels;	/* number of channels */
	int format;		/* encoding (TODO: rename this to "encoding"!) */
	int framesize;	/* Output needs data in chunks of framesize bytes. */
	enum playstate state; /* ... */
	int auxflags;	/* For now just one: quiet mode (for probing). */
	double preload;	/* buffer fraction to preload before play */
	int verbose;	/* verbosity to stderr */
/* TODO int intflag;   ... is it really useful/necessary from the outside? */
};

/* Lazy. */
#define AOQUIET ((ao->auxflags | ao->flags) & OUT123_QUIET)
#define AOVERBOSE(v) (!AOQUIET && ao->verbose >= (v))
#define GOOD_WRITEVAL(fd, val)     (unintr_write(fd, &(val), sizeof((val))) == sizeof((val)))
#define GOOD_WRITEBUF(fd, addr, n) (unintr_write(fd, (addr), (n)) == (n))
#define GOOD_READVAL(fd, val)      (unintr_read(fd, &(val), sizeof((val))) == sizeof((val)))
#define GOOD_READBUF(fd, addr, n)  (unintr_read(fd, (addr), (n)) == (n))

struct audio_format_name {
	int  val;
	char *name;
	char *sname;
};

#endif
