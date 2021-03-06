/*
 * Copyright (C) 2002-2011 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute, etc. this as long as all the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#ifndef __MS2_TLOGGER_H__
#define __MS2_TLOGGER_H__

#include <defines.h>
#include <gtk/gtk.h>
#include <watches.h>

typedef struct _MS2_TTMon_Data MS2_TTMon_Data;
/*!
 * \brief _MS2_TTMon_Data struct is a container used to hold private data
 * for the Trigger and Tooth and Composite Loggers (MS2-Extra only)
 */
struct _MS2_TTMon_Data
{
	gboolean stop;		/*! Stop display */
	gfloat zoom;		/*! Zoom */
	gint page;		/*! page used to discern them apart */
	GdkPixmap *pixmap;	/*! Pixmap */
	GtkWidget *darea;	/*! Pointer to drawing area */
	gint min_time;		/*! Minimum, trigger/tooth time */
	gint num_maxes;		/*! Hot many long pips per block */
	gint mins_inbetween;	/*! How many normal teeth */
	gint max_time;		/*! Maximum, trigger/tooth time */
	gint midpoint_time;	/*! avg between min and max */
	gint est_teeth;		/*! Estimated number of teeth */
	gint units;		/*! Units multiplier */
	gint missing;		/*! Number of missing teeth */
	gint sample_time;	/*! Time delay between reads.. */
	gint capabilities;	/*! Enum of ECU capabilities */
	gfloat usable_begin;	/*! Usable begin point for bars */
	gfloat font_height;	/*! Font height needed for some calcs */
	gfloat rpm;		/*! Current RPM */
	gulong *current;	/*! Current block of times */
	gulong *last;		/*! Last block of times */
	gulong *captures;	/*! Array of capture points */
	gint wrap_pt;		/*! Wrap point */
	gint vdivisor;		/*! Vertical scaling divisor */
	gfloat peak;		/*! Vertical Peak Value */
	PangoFontDescription *font_desc;	/*! Pango Font Descr */
	PangoLayout *layout;	/*! Pango Layout */
	GdkGC *axis_gc;		/*! axis graphics context */
	GdkGC *trace_gc;	/*! axis graphics context */
};

/* Prototypes */
void bind_ttm_to_page(gint);
void ms2_setup_ms2_logger_display(GtkWidget *);
gboolean ms2_logger_display_config_event(GtkWidget *, GdkEventConfigure *, gpointer);
gboolean logger_display_expose_event(GtkWidget *, GdkEventExpose *, gpointer);
gboolean ms2_ttm_zoom(GtkWidget *, gpointer);
void ms2_ttm_update(DataWatch *);
void _ms2_crunch_trigtooth_data(gint);
void ms2_update_trigtooth_display(gint);
void reset_ttm_buttons(void);

/* Prototypes */

#endif
