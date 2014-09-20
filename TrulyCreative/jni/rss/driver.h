#ifndef DRIVER_H
#define DRIVER_H

//#include "gg.h"

#include "config.h"

//#include <X11/Intrinsic.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#else
#ifdef HAVE_GETOPT
#include <unistd.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif 

#define True 1
#define False 0

typedef struct xstuff {
	char *commandLineName;

//	Display *display;

	int screen_num;

//	Window rootWindow;
//	Window window;
//	Window existingWindow;

	unsigned int windowWidth, windowHeight;	/* dimensions in pixels */

//	GC gc;			/* Graphics context. */

//	Colormap colourMap;

	void *hackstuff;
} xstuff_t;

#define DRIVER_OPTIONS_LONG {"root", 0, 0, 'r'}, {"maxfps", 1, 0, 'x'}, {"vsync", 1, 0, 'y'}, {"dpms", 1, 0, 'M'},
#define DRIVER_OPTIONS_SHORT "rx:y:M:"
#define DRIVER_OPTIONS_HELP "\t--root/-r\n" "\t--maxfps/-x <arg>\n" "\t--vsync/-y <arg>\n" "\t--dpms/-M <arg>\n"
#define DRIVER_OPTIONS_CASES case 'r': case 'x': case 'y': case 'M': handle_global_opts(c); break;

 void handle_global_opts (int c);
int strtol_minmaxdef(const char *optarg, const int base, const int min, const int max, const int type, const int def, const char *errmsg);

void ht_hack_handle_opts (int argc, char **argv);
void ht_hack_init (xstuff_t *);
void ht_hack_reshape (xstuff_t *);
void ht_hack_draw (xstuff_t *, double, float);
void ht_hack_cleanup (xstuff_t *);

void sd_hack_handle_opts (int argc, char **argv);
void sd_hack_init (xstuff_t *);
void sd_hack_reshape (xstuff_t *);
void sd_hack_draw (xstuff_t *, double, float);
void sd_hack_cleanup (xstuff_t *);

void lt_hack_handle_opts (int argc, char **argv);
void lt_hack_init (xstuff_t *);
void lt_hack_reshape (xstuff_t *);
void lt_hack_draw (xstuff_t *, double, float);
void lt_hack_cleanup (xstuff_t *);

void hack_handle_opts (int argc, char **argv);
void hack_init (xstuff_t *);
void hack_reshape (xstuff_t *);
void hack_draw (xstuff_t *, double, float);
void hack_cleanup (xstuff_t *);

#ifdef __cplusplus
}
#endif 

extern int glewInitialized;

#endif
