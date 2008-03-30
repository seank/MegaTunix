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
#include <conversions.h>
#include <dataio.h>
#include <datamgmt.h>
#include <defines.h>
#include <debugging.h>
#include <enums.h>
#include <firmware.h>
#include <listmgmt.h>
#include <mode_select.h>
#include <notifications.h>
#include <rtv_processor.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <timeout_handlers.h>
#include <widgetmgmt.h>


extern gint dbg_lvl;
extern GObject *global_data;


void enable_interrogation_button_cb(void)
{
	extern GHashTable *dynamic_widgets;
	extern volatile gboolean offline;
	if(!offline)
		gtk_widget_set_sensitive(GTK_WIDGET(g_hash_table_lookup(dynamic_widgets, "interrogate_button")),TRUE);
}


void start_statuscounts_cb(void)
{
	start_tickler(SCOUNTS_TICKLER);
}


void spawn_read_ve_const_cb(void)
{
	extern Firmware_Details *firmware;
	
	if (firmware->capabilities & MS2_STD)
		io_cmd("ms2_read_ve_const",NULL);
	else
		io_cmd("ms1_read_ve_const",NULL);
}

void read_ve_const(void *data, XmlEcuType type)
{
	extern Firmware_Details *firmware;
	extern volatile gboolean offline;
	Output_Data *output = NULL;
	Command *command = NULL;
	gint i = 0;

	g_list_foreach(get_list("get_data_buttons"),set_widget_sensitive,GINT_TO_POINTER(FALSE));
	if (type == MS2)
		printf("MS2 read_ve_const not written yet\n");
	if (type == MS1)
	{
		if (!offline)
		{
			for (i=0;i<=firmware->ro_above;i++)
			{
				output = g_new0(Output_Data,1);
				output->page=i;
				output->truepgnum = firmware->page_params[i]->truepgnum;
				output->need_page_change = TRUE;
				io_cmd(firmware->ve_command,output);
			}
		}
		command = (Command *)data;
		io_cmd(NULL,command->post_functions);
	}
}


void enable_get_data_buttons_cb(void)
{
	g_list_foreach(get_list("get_data_buttons"),set_widget_sensitive,GINT_TO_POINTER(TRUE));
}


void conditional_start_rtv_tickler_cb(void)
{
	start_tickler(RTV_TICKLER);
}


void set_store_black_cb(void)
{
	gint j = 0;
	extern Firmware_Details *firmware;

	set_group_color(BLACK,"burners");
	for (j=0;j<firmware->total_tables;j++)
		set_reqfuel_color(BLACK,j);
}

void enable_3d_buttons_cb(void)
{
	g_list_foreach(get_list("3d_buttons"),set_widget_sensitive,GINT_TO_POINTER(TRUE));
}

void reset_temps_cb(void)
{
	reset_temps(OBJ_GET(global_data,"temp_units"));
}

void simple_read_cb(XmlCmdType type, void * data)
{
	Io_Message *message  = NULL;
	Output_Data *output  = NULL;
	gboolean res = FALSE;
	static gint lastcount = 0;
	extern Firmware_Details *firmware;
	extern gint ms_ve_goodread_count;
	extern gint ms_reset_count;
	extern gint ms_goodread_count;
	guchar *ptr = NULL;
	static gboolean just_starting = TRUE;


	message = (Io_Message *)data;
	output = (Output_Data *)message->payload;

	switch (type)
	{
		case WRITE_VERIFY:
			printf("MS2_WRITE_VERIFY not written yet\n");
			break;
		case MISMATCH_COUNT:
			printf("MS2_MISMATCH_COUNT not written yet\n");
			break;
		case MS1_CLOCK:
			printf("MS1_CLOCK not written yet\n");
			break;
		case MS2_CLOCK:
			printf("MS2_CLOCK not written yet\n");
			break;
		case REVISION:
			printf("REVISON not written yet\n");
			break;
		case SIGNATURE:
			printf("SIGNATURE not written yet\n");
			break;
		case MS1_VECONST:
			res = read_data(firmware->page_params[output->page]->length,&message->recv_buf);
			if (!res)
			{
				printf("ERROR receive failure! (MS1 VECONST)!!!\n");
				break;
			}
			store_new_block(output->canID,output->page,0,
					message->recv_buf,
					firmware->page_params[output->page]->length);
			backup_current_data(0,message->page);
			ms_ve_goodread_count++;
			dump_output(firmware->page_params[output->page]->length,message->recv_buf);
			break;
		case MS2_VECONST:
			printf("MS2_VECONST handler not written yet\n");
			break;
		case MS1_RT_VARS:
			res = read_data(firmware->rtvars_size,&message->recv_buf);
			if (!res)
			{
				printf("ERROR receive failure! (RTVARS)!!!\n");
				break;
			}
			ptr = (guchar *)message->recv_buf;
			/* Test for MS reset */
			if (just_starting)
			{
				lastcount = ptr[0];
				just_starting = FALSE;
			}
			/* Check for clock jump from the MS, a 
			 * jump in time from the MS clock indicates 
			 * a reset due to power and/or noise.
			 */
			if ((lastcount - ptr[0] > 1) && \
					(lastcount - ptr[0] != 255))
			{
				ms_reset_count++;
				gdk_beep();
			}
			else
				ms_goodread_count++;

			lastcount = ptr[0];

			/* Feed raw buffer over to post_process()
			 * as a void * and pass it a pointer to the new
			 * area for the parsed data...
			 */
			dump_output(firmware->rtvars_size,message->recv_buf);
			process_rt_vars((void *)message->recv_buf);
			break;
		case MS2_RT_VARS:
			printf("MS2_RTVARS_CALLBACK not written yet\n");
			break;
		case MS2_BOOTLOADER:
			printf("MS2_BOOTLOADER not written yet\n");
			break;
	}
}
