/*
 * Copyright (C) 2002-2011 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
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
#include <defines.h>
#include <debugging.h>
#include <enums.h>
#include <firmware.h>
#include <gui_handlers.h>
#include <math.h>
#include <menu_handlers.h>
#include <plugin.h>
#include <runtime_text.h>
#include <stdlib.h>
#include <tabloader.h>
#include <threads.h>
#include <widgetmgmt.h>
#include <mtxmatheval.h>

extern gconstpointer *global_data;
static struct 
{
	const gchar *item;
	TabIdent tab;
}items[] = {
	{"vetable_tuning_menuitem",VETABLES_TAB},
	{"spark_tuning_menuitem",SPARKTABLES_TAB},
	{"afr_tuning_menuitem",AFRTABLES_TAB},
	{"runtime_vars_menuitem",RUNTIME_TAB},
};

static struct 
{
	const gchar *item;
	FioAction action;
}fio_items[] = {
	{"import_tables_menuitem",VEX_IMPORT},
	{"export_tables_menuitem",VEX_EXPORT},
	{"restore_ecu_menuitem",ECU_RESTORE},
	{"backup_ecu_menuitem",ECU_BACKUP},
};

G_MODULE_EXPORT void setup_menu_handlers_pf(void)
{
	void (*common_plugin_menu_setup)(GladeXML *);
	GtkWidget *item = NULL;
	guint i = 0;
	GladeXML *xml = NULL;
	Firmware_Details *firmware = NULL;

	firmware = DATA_GET(global_data,"firmware");

	xml = (GladeXML *)DATA_GET(global_data,"main_xml");
	if ((!xml) || (DATA_GET(global_data,"leaving")))
		return;

	if (get_symbol ("common_plugin_menu_setup",(void *)&common_plugin_menu_setup))
		common_plugin_menu_setup(xml);

	gdk_threads_enter();

	item = glade_xml_get_widget(xml,"show_tab_visibility_menuitem");
	gtk_widget_set_sensitive(item,TRUE);
	
	for (i=0;i< (sizeof(items)/sizeof(items[0]));i++)
	{
		item = glade_xml_get_widget(xml,items[i].item);
		if (GTK_IS_WIDGET(item))
			OBJ_SET(item,"target_tab",
					GINT_TO_POINTER(items[i].tab));
		if (!check_tab_existance(items[i].tab))
			gtk_widget_set_sensitive(item,FALSE);
		else
			gtk_widget_set_sensitive(item,TRUE);
	}
	for (i=0;i< (sizeof(fio_items)/sizeof(fio_items[0]));i++)
	{
		item = glade_xml_get_widget(xml,fio_items[i].item);
		if (GTK_IS_WIDGET(item))
		{
			OBJ_SET(item,"fio_action",
					GINT_TO_POINTER(fio_items[i].action));
			gtk_widget_set_sensitive(item,TRUE);
		}
	}
	gdk_threads_leave();
	return;
}

/*!
 \brief switches to tab encoded into the widget
 */
G_MODULE_EXPORT gboolean jump_to_tab(GtkWidget *widget, gpointer data)
{
	GtkWidget *notebook = NULL;
	TabIdent target = -1;
	TabIdent c_tab = 0;
	gint total = 0;
	GtkWidget * child = NULL;
	gint i = 0;
	
	notebook = lookup_widget( "toplevel_notebook");
	if (!GTK_IS_NOTEBOOK(notebook))
		return FALSE;
	if (!OBJ_GET(widget,"target_tab"))
		return FALSE;
	target = (TabIdent)OBJ_GET(widget,"target_tab");
	total = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	for (i=0;i<total;i++)
	{
		child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook),i);
		if (!OBJ_GET(child,"tab_ident"))
			continue;
		c_tab = (TabIdent)OBJ_GET(child,"tab_ident");
		if (c_tab == target)
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook),i);
			return TRUE;
		}
	}

	return FALSE;
}

/*!
 \brief General purpose handler to take care of menu initiated settings 
 transfers like VEX import/export and ECU backup/restore
 */
G_MODULE_EXPORT gboolean settings_transfer(GtkWidget *widget, gpointer data)
{
	FioAction action = -1;
	action = (FioAction)OBJ_GET(widget,"fio_action");
	void (*do_backup)(GtkWidget *, gpointer) = NULL;
	void (*do_restore)(GtkWidget *, gpointer) = NULL;
	void (*vex_import)(GtkWidget *, gpointer) = NULL;
	void (*vex_export)(GtkWidget *, gpointer) = NULL;

	switch (action)
	{
		case VEX_IMPORT:
			if (get_symbol("select_vex_for_import",(void*)&vex_import))
				vex_import(NULL,NULL);
			break;
		case VEX_EXPORT:
			if (get_symbol("select_vex_for_export",(void*)&vex_export))
				vex_export(NULL,NULL);
			break;
		case ECU_BACKUP:
			if (get_symbol("select_file_for_ecu_backup",(void*)&do_backup))
				do_backup(NULL,NULL);
			break;
		case ECU_RESTORE:
			if (get_symbol("select_file_for_ecu_restore",(void*)&do_restore))
				do_restore(NULL,NULL);
			break;
	}
	return TRUE;
}

/*!
 \brief General purpose handler to take care of menu initiated settings 
 transfers like VEX import/export and ECU backup/restore
 */
G_MODULE_EXPORT gboolean check_tab_existance(TabIdent target)
{
	GtkWidget *notebook = NULL;
	TabIdent c_tab = 0;
	gint total = 0;
	GtkWidget * child = NULL;
	gint i = 0;
	
	notebook = lookup_widget( "toplevel_notebook");
	if (!GTK_IS_NOTEBOOK(notebook))
		return FALSE;
	total = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	for (i=0;i<total;i++)
	{
		child = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook),i);
		if (!OBJ_GET(child,"tab_ident"))
			continue;
		c_tab = (TabIdent)OBJ_GET(child,"tab_ident");
		if (c_tab == target)
		{
			return TRUE;
		}
	}
	return FALSE;
}


