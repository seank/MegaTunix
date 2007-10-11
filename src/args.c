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


#include <args.h>
#include <config.h>
#include <glib.h>
#include <unistd.h>


gboolean be_quiet = FALSE;
gboolean autolog_dump = FALSE;
gboolean hide_rtvars = FALSE;
gboolean hide_status = FALSE;
gboolean hide_maingui = FALSE;
gint autolog_minutes = 5;
gchar * autolog_dump_dir = NULL;
gchar *autolog_basename = NULL;


static GOptionEntry entries[] =
{
	{"quiet",'q',0,G_OPTION_ARG_NONE,&be_quiet,"Suppress all GUI error notifications",NULL},
	{"no-rtvars",'r',0,G_OPTION_ARG_NONE,&hide_rtvars,"Hide RealTime Vars window",NULL},
	{"no-status",'s',0,G_OPTION_ARG_NONE,&hide_status,"Hide ECU Status window",NULL},
	{"no-maingui",'g',0,G_OPTION_ARG_NONE,&hide_maingui,"Hide Main Gui window (i.e, dash only)",NULL},
	{"autolog",'l',0,G_OPTION_ARG_NONE,&autolog_dump,"Automatically dump datalog to file every N minutes",NULL},
	{"minutes",'m',0,G_OPTION_ARG_INT,&autolog_minutes,"How many minutes of data logged per logfile","N"},
	{"log_dir",'d',0,G_OPTION_ARG_FILENAME,&autolog_dump_dir,"Directory to put datalogs into",NULL},
	{"log_basename",'b',0,G_OPTION_ARG_FILENAME,&autolog_basename,"Base filename for logs.",NULL},
	  { NULL },
};


/*!
 \brief handle_args() handles parsing of cmd line arguments to megatunix
 \param argc (gint) count of command line arguments
 \param argv (char **) array of command line args
 \returns void
 */
void handle_args(gint argc, gchar * argv[])
{
	GError *error = NULL;
	GOptionContext *context = NULL;

	context = g_option_context_new ("- MegaTunix options");
	g_option_context_add_main_entries (context, entries, NULL);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	g_option_context_parse (context, &argc, &argv, &error);

	printf("quiet option %i\n",be_quiet);
	printf("no rtvars option %i\n",hide_rtvars);
	printf("no status option %i\n",hide_status);
	printf("no maingui option %i\n",hide_maingui);
	printf("autolog_dump %i\n",autolog_dump);
	printf("autolog_minutes %i\n",autolog_minutes);
	printf("autolog_dump_dir %s\n",autolog_dump_dir);
	printf("autolog_basename %s\n",autolog_basename);
}
