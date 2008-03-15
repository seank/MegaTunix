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


typedef struct _PotentialArg PotentialArg;
typedef struct _Command Command;

/*!
 * \brief _PotentialArgs struct holds information read from the 
 * communications XML, one PotentialArg per command
 */
struct _PotentialArg
{
	gchar *name;		/* Potential arg name */
	gchar *desc;		/* Description */
	gchar *internal_name;	/* Internal name used for linking */
	gint count;		/* Number of elements to xfer */
	DataSize size;		/* Size of data */
};


/*!
 * \brief _Command struct holds information read from the 
 * communications XML, describing each possible command. Their names
 * are used as internal keys to link firmware stuf to the XML definitions.
 */
struct _Command
{
	gchar *name;		/* Command Name */
	gchar *desc;		/* Command Description */
	gchar *base;		/* Base command charactor(s) */
	CmdType type;		/* Command type enumeration */
	GArray *arg_sequence;	/* Argument list array of PotentialArgs */
};
/* Prototypes */
void load_comm_xml(gchar *, gpointer );
void load_xmlcomm_elements(GHashTable *, xmlNode *);
void get_potential_arg_name(GHashTable *, xmlNode *);
void load_potential_args(PotentialArg *, xmlNode *);
void process_commands_section(GHashTable *, xmlNode *);
void load_command_args(Command *, xmlNode *);

void load_command_section(Command *, xmlNode *);
void load_cmd_arguments(Command *, xmlNode *);
void load_arg_attrs(PotentialArg *, xmlNode *);



/* Prototypes */

#endif
