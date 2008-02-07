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

#ifndef __XMLCOMM_H__
#define __XMLCOMM_H__

#include <defines.h>
#include <enums.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


/* Prototypes */
void load_comm_xml(gchar *, gpointer );
void load_xmlcomm_elements(GHashTable *, xmlNode *);
void get_potential_arg_name(GHashTable *, xmlNode *);
void load_potential_args(PotentialArg *, xmlNode *);
void process_commands_section(GHashTable *, xmlNode *);
void load_command_args(Command *, xmlNode *);



/* Prototypes */

#endif
