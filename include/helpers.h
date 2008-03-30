/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
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

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <defines.h>
#include <gtk/gtk.h>

/* Prototypes */
void enable_interrogation_button(void);
void start_statuscounts(void);
void spawn_read_ve_const(void);
void enable_get_data_buttons(void);
void conditional_start_rtv_tickler(void);
void set_store_black(void);
void enable_3d_buttons(void);
void simple_read_callback(XmlCmdType, void *);
/* Prototypes */

#endif
