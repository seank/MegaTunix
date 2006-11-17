/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <config.h>
#include <configfile.h>
#include <debugging.h>
#include <defines.h>
#include <dep_processor.h>
#include <enums.h>
#include <getfiles.h>
#include <lookuptables.h>
#include <stdlib.h>
#include <structures.h>

GHashTable *lookuptables = NULL;


/*!
 \brief load_lookuptables() loads the lookuptables specified in the ECU's
 interrogation profile. Any number of lookuptables are allowed and they 
 are referenced internally by a string name to a hashtable.
 \param canidate (struct Canidate *) pointer to the canidate 
 interrogation profile struct
 */
void load_lookuptables(struct Canidate *canidate)
{
	g_hash_table_foreach(canidate->lookuptables,get_table,NULL);
}


/*!
 \brief get_table() gets a valid filehandle of the lookuptable from 
 get_file and passes it to load_table()
 \see load_table
 \see get_File
 \param table_name (gpointer) textual name of the table to use as the key
 to the lookuptables hashtable
 \param fname (gpointer) textual name of the filename to load
 \param user_data (gpointer) unused
 */
void get_table(gpointer table_name, gpointer fname, gpointer user_data)
{
	gboolean status = FALSE;
	gchar * filename = NULL;
	gchar ** vector = NULL;
	
	vector = g_strsplit(fname,".",2);

	filename = get_file(g_strconcat(LOOKUPTABLE_DIR,"/",vector[0],NULL),g_strdup(vector[1]));
	g_strfreev(vector);

	if (filename)
	{
		status = load_table((gchar *)table_name,filename);
		g_free(filename);
	}
	if (!status)
	{
		dbg_func(g_strdup_printf(__FILE__": load_lookuptables()\n\tFAILURE loading \"%s\" lookuptable, EXITING!!\n",(gchar *)table_name),CRITICAL);
		exit (-2);
	}

}


/*!
 \brief load_table() physically handles load ingthe table datafrom disk, 
 populating and array and sotring a pointer to that array in the lookuptables
 hashtable referenced by the table_name passed
 \param table_name (gchar *) key to lookuptables hashtable
 \param filename (gchar *) filename to load table data from
 \returns TRUE on success, FALSE on failure
 */
gboolean load_table(gchar *table_name, gchar *filename)
{
	GIOStatus status;
	GIOChannel *iochannel;
	gboolean go = TRUE;
	gchar * str = NULL;
	gchar * tmp = NULL;
	gchar * end = NULL;
	GString *a_line; 
	gint *array = NULL;
	gint tmparray[2048]; // bad idea being static!!
	gint i = 0;

	iochannel = g_io_channel_new_file(filename,"r", NULL);
	status = g_io_channel_seek_position(iochannel,0,G_SEEK_SET,NULL);
	if (status != G_IO_STATUS_NORMAL)
		dbg_func(g_strdup(__FILE__": load_lookuptables()\n\tError seeking to beginning of the file\n"),CRITICAL);
	while (go)	
	{
		a_line = g_string_new("\0");
		status = g_io_channel_read_line_string(iochannel, a_line, NULL, NULL);
		if (status == G_IO_STATUS_EOF)
			go = FALSE;
		else
		{
			str = g_strchug(g_strdup(a_line->str));
			if (g_str_has_prefix(str,"DB"))
			{
				str+=2; // move 2 places in	
				end = g_strrstr(str,"T");
				tmp = g_strndup(str,end-str);
				tmparray[i]=atoi(tmp);
				g_free(tmp);
				i++;
			}
			//g_free(str);
		}
		g_string_free(a_line,TRUE);
	}
	g_io_channel_shutdown(iochannel,FALSE,NULL);

	array = g_memdup(&tmparray,i*sizeof(gint));
	if (!lookuptables)
		lookuptables = g_hash_table_new(g_str_hash,g_str_equal);
	g_hash_table_insert(lookuptables,table_name,array);

	return TRUE;
}


/*!
 \brief reverse_lookup() looks for the INDEX of this value in the specified
 lookuptable.  This has a couple potentiela faults for tables that are 
 NOT 1:1 relationships and can mistakenly give the wrong value in that case
 \param value (gint) value to lookup
 \param table (gint *) pointer to lookuptable array
 \returns the index closest to that data
 */
gint reverse_lookup(gint value, gint *table)
{
	gint i = 0;
	gint closest_index = 0;
	gint minimum = 65535;

	while (table[i])
	{
		if (abs(table[i]-value) < minimum)
		{
			minimum = abs(table[i]-value);
			closest_index = i;
		}
		i++;
	}

	return closest_index;
}


/*!
 \brief lookup_data() returns the value represented by the lookuptable 
 associated with the passed object and offset
 \param object (GObject *) container of parameters we need to do the lookup
 \param offset (gint) offset into lookuptable
 \returns the value at that offset of the lokuptable
 */
gfloat lookup_data(GObject *object, gint offset)
{
	extern GHashTable *lookuptables;
	GObject *dep_obj = NULL;
	gint *lookuptable = NULL;
	gchar *table = NULL;
	gchar *alt_table = NULL;
	gboolean state = FALSE;

	table = (gchar *)g_object_get_data(object,"lookuptable");
	alt_table = (gchar *)g_object_get_data(object,"alt_lookuptable");
	dep_obj = (GObject *)g_object_get_data(object,"dep_object");
	if (dep_obj)
		state = check_dependancies(dep_obj);
	if (state)
		lookuptable = (gint *)g_hash_table_lookup(lookuptables,alt_table);	
	else
		lookuptable = (gint *)g_hash_table_lookup(lookuptables,table);	
	//assert(lookuptable);
	if (!lookuptable)
	{
		dbg_func(g_strdup_printf(__FILE__": lookup_data()\n\t Lookuptable is NULL for control %s\n",(gchar *) g_object_get_data(object,"internal_name")),CRITICAL);
		return 0.0;
	}
	return lookuptable[offset];
}

