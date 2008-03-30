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

#include <comms.h>
#include <config.h>
#include <dataio.h>
#include <datamgmt.h>
#include <defines.h>
#include <debugging.h>
#include <enums.h>
#include <errno.h>
#include <fcntl.h>
#include <firmware.h>
#include <gui_handlers.h>
#include <notifications.h>
#include <serialio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <timeout_handlers.h>
#include <termios.h>
#include <unistd.h>
#include <3d_vetable.h>

extern GStaticMutex serio_mutex;
extern gint dbg_lvl;
extern GObject *global_data;

/*!
 \brief update_comms_status updates the Gui with the results of the comms
 test.  This is decoupled from the comms_test due to threading constraints.
 \see comms_test
 */
void update_comms_status(void)
{
	extern gboolean connected;
	extern GHashTable *dynamic_widgets;
	GtkWidget *widget = NULL;

	if (NULL != (widget = g_hash_table_lookup(dynamic_widgets,"runtime_connected_label")))
		gtk_widget_set_sensitive(GTK_WIDGET(widget),connected);
	if (NULL != (widget = g_hash_table_lookup(dynamic_widgets,"ww_connected_label")))
		gtk_widget_set_sensitive(GTK_WIDGET(widget),connected);
	return;
}

/*! 
 \brief comms_test sends the clock_request command ("C") to the ECU and
 checks the response.  if nothing comes back, MegaTunix assumes the ecu isn't
 connected or powered down. NO Gui updates are done from this function as it
 gets called from a thread. update_comms_status is dispatched after this
 function ends from the main context to update the GUI.
 \see update_comms_status
 */

gint comms_test()
{
	gboolean result = FALSE;
	gchar * err_text = NULL;
	static gint errcount = 0;
	extern Serial_Params *serial_params;
	extern gboolean connected;

	if (dbg_lvl & SERIAL_RD)
		dbg_func(g_strdup(__FILE__": comms_test()\n\t Entered...\n"));
	if (!serial_params)
		return FALSE;

	if (dbg_lvl & SERIAL_RD)
		dbg_func(g_strdup(__FILE__": comms_test()\n\tRequesting ECU Clock (\"C\" cmd)\n"));
	if (write(serial_params->fd,"C",1) != 1)
	{
		err_text = (gchar *)g_strerror(errno);
		if (dbg_lvl & (SERIAL_RD|CRITICAL))
			dbg_func(g_strdup_printf(__FILE__": comms_test()\n\tError writing \"C\" to the ecu, ERROR \"%s\" in comms_test()\n",err_text));
		thread_update_logbar("comms_view","warning",g_strdup_printf("Error writing \"C\" to the ecu, ERROR \"%s\" in comms_test()\n",err_text),FALSE,FALSE);
		connected = FALSE;
		return connected;
	}
	result = handle_ecu_data(C_TEST,NULL);
	if (!result) /* Failure,  Attempt MS-II method */
	{
		if (write(serial_params->fd,"c",1) != 1)
		{
			err_text = (gchar *)g_strerror(errno);
			if (dbg_lvl & (SERIAL_RD|CRITICAL))
				dbg_func(g_strdup_printf(__FILE__": comms_test()\n\tError writing \"c\" (MS-II clock test) to the ecu, ERROR \"%s\" in comms_test()\n",err_text));
			thread_update_logbar("comms_view","warning",g_strdup_printf("Error writing \"c\" (MS-II clock test) to the ecu, ERROR \"%s\" in comms_test()\n",err_text),FALSE,FALSE);
			connected = FALSE;
			return connected;
		}
		result = handle_ecu_data(C_TEST,NULL);
	}
	if (result)	/* Success */
	{
		connected = TRUE;
		errcount=0;
		if (dbg_lvl & SERIAL_RD)
			dbg_func(g_strdup(__FILE__": comms_test()\n\tECU Comms Test Successfull\n"));
		queue_function(g_strdup("kill_conn_warning"));
		thread_update_widget(g_strdup("titlebar"),MTX_TITLE,g_strdup("ECU Connected..."));
		thread_update_logbar("comms_view",NULL,g_strdup_printf("ECU Comms Test Successfull\n"),FALSE,FALSE);

	}
	else
	{
		/* An I/O Error occurred with the MegaSquirt ECU  */
		connected = FALSE;
		errcount++;
		if (errcount > 2 )
			queue_function(g_strdup("conn_warning"));
		thread_update_widget(g_strdup("titlebar"),MTX_TITLE,g_strdup_printf("COMMS ISSUES: Check COMMS tab"));
		if (dbg_lvl & (SERIAL_RD|IO_PROCESS))
			dbg_func(g_strdup(__FILE__": comms_test()\n\tI/O with ECU Timeout\n"));
		thread_update_logbar("comms_view","warning",g_strdup_printf("I/O with ECU Timeout\n"),FALSE,FALSE);
	}
	return connected;
}


/*!
 \brief update_write_status() checks the differences between the current ECU
 data snapshot and the last one, if there are any differences (things need to
 be burnt) then it turns all the widgets in the "burners" group to RED
 \param data (Output_Data *) pointer to data sent to ECU used to
 update other widgets that refer to that Page/Offset
 */
void update_write_status(Output_Data *data)
{
	extern Firmware_Details *firmware;
	guint8 **ecu_data = firmware->ecu_data;
	guint8 **ecu_data_last = firmware->ecu_data_last;
	gint i = 0;
	extern GList ***ve_widgets;
	extern gboolean paused_handlers;


	if ((data->data) && (data->mode == MTX_CHUNK_WRITE))
	{
		g_free(data->data);
		return;
	}

	paused_handlers = TRUE;

	/*printf ("page %i, offset %i\n",data->page,data->offset); */
	for (i=0;i<g_list_length(ve_widgets[data->page][data->offset]);i++)
	{
		if ((gint)OBJ_GET(g_list_nth_data(ve_widgets[data->page][data->offset],i),"dl_type") != DEFERRED)
		{
			/*printf("updating widget %s\n",(gchar *)glade_get_widget_name(g_list_nth_data(ve_widgets[data->page][data->offset],i))); */
			update_widget(g_list_nth_data(ve_widgets[data->page][data->offset],i),NULL);
		}
		/*	else
		printf("NOT updating widget %s because it's defered\n",(gchar *)glade_get_widget_name(g_list_nth_data(ve_widgets[data->page][data->offset],i)));
		*/
	}

	update_ve3d_if_necessary(data->page,data->offset);

	paused_handlers = FALSE;
	/* We check to see if the last burn copy of the VE/constants matches 
	 * the currently set, if so take away the "burn now" notification.
	 * avoid unnecessary burns to the FLASH 
	 */

	for (i=0;i<firmware->total_pages;i++)
	{

		if(memcmp(ecu_data_last[i],ecu_data[i],firmware->page_params[i]->length) != 0)
		{
			set_group_color(RED,"burners");
			return;
		}
	}
	/* If pchunk dwrite data bound,  FREE IT */
	set_group_color(BLACK,"burners");

	return;
}


/*!
 \brief burn_ecu_flash() issues the commands to the ECU to burn the contents
 of RAM to flash.
 */
void burn_ms1_ecu_flash()
{
	gint res = 0;
	gint i = 0;
	gchar * err_text = NULL;
	extern Firmware_Details * firmware;
	extern Serial_Params *serial_params;
	extern volatile gboolean offline;
	extern gboolean connected;
	static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

	g_static_mutex_lock(&serio_mutex);
	g_static_mutex_lock(&mutex);

	if (offline)
		goto copyover;

	if (!connected)
	{
		if (dbg_lvl & (SERIAL_WR|CRITICAL))
			dbg_func(g_strdup(__FILE__": burn_ms_flahs()\n\t NOT connected, can't burn flash, returning immediately\n"));
		g_static_mutex_unlock(&serio_mutex);
		g_static_mutex_unlock(&mutex);
		return;
	}
	g_static_mutex_unlock(&serio_mutex);
	flush_serial(serial_params->fd, TCIOFLUSH);
	g_static_mutex_lock(&serio_mutex);
	res = write (serial_params->fd,firmware->burn_command,1);  /* Send Burn command */
	if (res != 1)
	{
		err_text = (gchar *)g_strerror(errno);
		if (dbg_lvl & (SERIAL_WR|CRITICAL))
			dbg_func(g_strdup_printf(__FILE__": burn_ecu_flash()\n\tBurn Failure, ERROR \"%s\"\n",err_text));
	}
	g_usleep(250000);

	if (dbg_lvl & SERIAL_WR)
		dbg_func(g_strdup(__FILE__": burn_ecu_flash()\n\tBurn to Flash\n"));
	g_static_mutex_unlock(&serio_mutex);
	flush_serial(serial_params->fd, TCIOFLUSH);
	g_static_mutex_lock(&serio_mutex);
copyover:
	/* sync temp buffer with current burned settings */
	for (i=0;i<firmware->total_pages;i++)
		backup_current_data(firmware->canID,i);

	g_static_mutex_unlock(&serio_mutex);
	g_static_mutex_unlock(&mutex);
	return;
}



/*!
 \brief set_ms_page() is called to change the current page being accessed in
 the firmware. set_ms_page will check to see if any outstanding data has 
 been sent to the current page, but NOT burned to flash befpre changing pages
 in that case it will burn the flash before changing the page. 
 \param ms_page (guint8) the page to set to
 */
void set_ms_page(guint8 ms_page)
{
	extern Serial_Params *serial_params;
	extern Firmware_Details *firmware;
	guint8 **ecu_data = firmware->ecu_data;
	guint8 **ecu_data_last = firmware->ecu_data_last;
	extern gboolean force_page_change;
	static gint last_page = -1;
	gint res = 0;
	gchar * err_text = NULL;
	static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

	/*printf("fed_page %i, last_page %i\n",ms_page,last_page); */
	g_static_mutex_lock(&serio_mutex);
	g_static_mutex_lock(&mutex);

	/* put int to make sure page is SET on FIRST RUN after interrogation.
	 * Found that wihtout it was getting a corrupted first page
	 */
	if (last_page == -1)
		goto force_change;

	if ((ms_page > firmware->ro_above) && (last_page <= firmware->ro_above))
	{
		g_static_mutex_unlock(&serio_mutex);
		burn_ms1_ecu_flash();
		g_static_mutex_lock(&serio_mutex);
		goto force_change;
	}
	if ((ms_page > firmware->ro_above) || (last_page > firmware->ro_above))
		goto skip_change;

	if (((ms_page != last_page) && (((memcmp(ecu_data_last[last_page],ecu_data[last_page],firmware->page_params[last_page]->length) != 0)) || ((memcmp(ecu_data_last[ms_page],ecu_data[ms_page],firmware->page_params[ms_page]->length) != 0)))))
	{
		g_static_mutex_unlock(&serio_mutex);
		burn_ms1_ecu_flash();
		g_static_mutex_lock(&serio_mutex);
	}
skip_change:
	if ((ms_page == last_page) && (!force_page_change))
	{
		/*	printf("no need to change the page again as it's already %i\n",ms_page); */
		g_static_mutex_unlock(&serio_mutex);
		g_static_mutex_unlock(&mutex);
		return;
	}

force_change:
	if (dbg_lvl & SERIAL_WR)
		dbg_func(g_strdup_printf(__FILE__": set_ms_page()\n\tSetting Page to \"%i\" with \"%s\" command...\n",ms_page,firmware->page_cmd));

	res = write(serial_params->fd,firmware->page_cmd,1);
	if (res != 1)
	{
		err_text = (gchar *)g_strerror(errno);
		if (dbg_lvl & (SERIAL_WR|CRITICAL))
			dbg_func(g_strdup_printf(__FILE__": set_ms_page()\n\tFAILURE sending \"%s\" (change page) command to ECU, ERROR \"%s\" \n",firmware->page_cmd,err_text));
	}
	res = write(serial_params->fd,&ms_page,1);
	g_usleep(100000);
	if (res != 1)
	{
		err_text = (gchar *)g_strerror(errno);
		if (dbg_lvl & (SERIAL_WR|CRITICAL))
			dbg_func(g_strdup_printf(__FILE__": set_ms_page()\n\tFAILURE changing page on ECU to %i, ERROR \"%s\"\n",ms_page,err_text));
	}

	last_page = ms_page;

	g_static_mutex_unlock(&serio_mutex);
	g_static_mutex_unlock(&mutex);
	force_page_change = FALSE;
	return;

}



/*!
 \brief writeto_ecu() physiclaly sends the data to the ECU.
 \param message (Io_Message *) a pointer to a Io_Message
 */
void write_data(Io_Message *message)
{
	extern gboolean connected;
	Output_Data *output = message->payload;

	gint res = 0;
	gchar * err_text = NULL;
	gint i = 0;
	DBlock *block = NULL;
	extern Serial_Params *serial_params;
	extern Firmware_Details *firmware;
	extern volatile gboolean offline;
	static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

	g_static_mutex_lock(&serio_mutex);
	g_static_mutex_lock(&mutex);

	if (offline)
	{
		/*printf ("OFFLINE writing value at %i,%i [%i]\n",page,offset,value); */
		switch (output->mode)
		{
			case MTX_SIMPLE_WRITE:
				set_ecu_data(output->canID,output->page,output->offset,output->size,output->value);
				break;
			case MTX_CHUNK_WRITE:
				store_new_block(output->canID,output->page,output->offset,output->data,output->len);
				break;
		}
		g_static_mutex_unlock(&serio_mutex);
		g_static_mutex_unlock(&mutex);
		return;		/* can't write anything if offline */
	}
	if (!connected)
	{
		g_static_mutex_unlock(&serio_mutex);
		g_static_mutex_unlock(&mutex);
		return;		/* can't write anything if disconnected */
	}


	if (output)
	{
		if ((firmware->multi_page ) && (output->need_page_change)) 
		{
			g_static_mutex_unlock(&serio_mutex);
			set_ms_page(output->truepgnum);
			g_static_mutex_lock(&serio_mutex);
		}
	}

	for (i=0;i<message->sequence->len;i++)
	{
		block = g_array_index(message->sequence,DBlock *,i);
		if (block->type == DATA)

			res = write (serial_params->fd,block->str,block->len);	/* Send write command */
		if (res != 1 )
		{
			err_text = (gchar *)g_strerror(errno);
			if (dbg_lvl & (SERIAL_WR|CRITICAL))
				dbg_func(g_strdup_printf(__FILE__": write_data()\n\tError writing block %i, ERROR \"%s\"!!!\n",i,err_text));
		}
		else
		{
			if (dbg_lvl & SERIAL_WR)
				dbg_func(g_strdup_printf(__FILE__": writeto_ecu()\n\tWrite of block %i to ECU succeeded\n",i));
		}

	}

	if (output)
	{
	if (output->mode == MTX_SIMPLE_WRITE)
		set_ecu_data(output->canID,output->page,output->offset,output->size,output->value);
	else if (output->mode == MTX_CHUNK_WRITE)
		store_new_block(output->canID,output->page,output->offset,output->data,output->len);
	}

	g_static_mutex_unlock(&serio_mutex);
	g_static_mutex_unlock(&mutex);
	return;
}


