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
#include <debugging.h>
#include <defines.h>
#include <enums.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stringmatch.h>
#include <structures.h>
#include <xmlcomm.h>
#include <xmlbase.h>


extern gint dbg_lvl;


void load_comm_xml(gchar *filename, gpointer data)
{
	//extern GObject * global_data;
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;
	GHashTable *arguments = NULL;
	extern GObject *global_data;
	extern gboolean interrogated;

	if (!interrogated)
		return;
	if (filename == NULL)
		return;

	LIBXML_TEST_VERSION

		/*parse the file and get the DOM */
		doc = xmlReadFile(filename, NULL, 0);

	if (doc == NULL)
	{
		printf("error: could not parse file %s\n",filename);
		return;
	}

	/*Get the root element node */
	root_element = xmlDocGetRootElement(doc);
	arguments = (GHashTable *)g_object_get_data(G_OBJECT(global_data),"potential_arguments");
	load_xmlcomm_elements(arguments, root_element);
	xmlFreeDoc(doc);
	xmlCleanupParser();


}

void load_xmlcomm_elements(GHashTable *arguments, xmlNode *a_node)
{
	GHashTable *commands_hash = NULL;
	xmlNode *cur_node = NULL;
	extern GObject *global_data;

	/* Iterate though all nodes... */
	for (cur_node = a_node;cur_node;cur_node = cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"potential_arguments") == 0)
				get_potential_arg_name(arguments,cur_node);
			if (g_strcasecmp((gchar *)cur_node->name,"commands") == 0)
			{
				commands_hash = (GHashTable *)g_object_get_data(G_OBJECT(global_data),"commands_hash");
				process_commands_section(commands_hash,cur_node);
			}
		}
		load_xmlcomm_elements(arguments,cur_node->children);
	}
}

void get_potential_arg_name(GHashTable *arguments, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	PotentialArg *arg = NULL;
	extern GObject *global_data;

	if (!node->children)
	{
		printf("ERROR, get_potential_arg_name, xml node is empty!!\n");
		return;
	}
	cur_node = node->children;
	while (cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"arg") == 0)
			{
				arg = g_new0(PotentialArg, 1);
				load_potential_args(arg, cur_node);
				g_hash_table_insert(arguments,g_strdup(arg->name),arg);
			}
		}
		cur_node = cur_node->next;

	}
}


void process_commands_section(GHashTable *commands_hash, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	Command *cmd = NULL;

	if (!node->children)
	{
		printf("ERROR, get_potential_arg_name, xml node is empty!!\n");
		return;
	}
	cur_node = node->children;
	while (cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"cmd") == 0)
			{
				cmd = g_new0(Command, 1);
				load_command_section(cmd, cur_node);
				g_hash_table_insert(commands_hash,g_strdup(cmd->name),cmd);
			}
		}
		cur_node = cur_node->next;

	}
}


void load_potential_args(PotentialArg *arg, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	gchar *tmpbuf = NULL;

	if (!node->children)
	{
		printf("ERROR, load_potential_args, xml node is empty!!\n");
		return;
	}
	cur_node = node->children;
	while (cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"name") == 0)
				generic_xml_gchar_import(cur_node,&arg->name);
			if (g_strcasecmp((gchar *)cur_node->name,"desc") == 0)
				generic_xml_gchar_import(cur_node,&arg->desc);
			if (g_strcasecmp((gchar *)cur_node->name,"internal_name") == 0)
				generic_xml_gchar_import(cur_node,&arg->internal_name);
			if (g_strcasecmp((gchar *)cur_node->name,"size") == 0)
			{
				generic_xml_gchar_import(cur_node,&tmpbuf);
				arg->size = translate_string(tmpbuf);
			}
		}
		cur_node = cur_node->next;
	}
}


void load_command_section(Command *cmd, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	gchar *tmpbuf = NULL;

	if (!node->children)
	{
		printf("ERROR, load_command_section, xml node is empty!!\n");
		return;
	}
	cur_node = node->children;
	while (cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"name") == 0)
				generic_xml_gchar_import(cur_node,&cmd->name);
			if (g_strcasecmp((gchar *)cur_node->name,"desc") == 0)
				generic_xml_gchar_import(cur_node,&cmd->desc);
			if (g_strcasecmp((gchar *)cur_node->name,"base") == 0)
				generic_xml_gchar_import(cur_node,&cmd->base);
			if (g_strcasecmp((gchar *)cur_node->name,"size") == 0)
			{
				generic_xml_gchar_import(cur_node,&tmpbuf);
				cmd->type = translate_string(tmpbuf);
			}
			if (g_strcasecmp((gchar *)cur_node->name,"arguments") == 0)
				load_cmd_arguments(cmd,cur_node);
		}
		cur_node = cur_node->next;
	}
}


void load_cmd_arguments(Command *cmd, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	cmd->arg_sequence = g_array_new(FALSE,TRUE,sizeof(PotentialArg *));
	PotentialArg *arg = NULL;

	arg = g_new0(PotentialArg, 1);

	if (!node->children)
	{
		printf("ERROR, load_cmd_arguments, xml node is empty!!\n");
		return;
	}
	cur_node = node->children;
	while (cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"arg") == 0)
			{
				generic_xml_gchar_import(cur_node,&arg->name);
				if (cur_node->children)
					load_arg_attrs(arg,cur_node);
			}
		}
		cur_node = cur_node->next;
	}
	g_array_append_val(cmd->arg_sequence,arg);
}


void load_arg_attrs(PotentialArg *arg, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	gchar *tmpbuf = NULL;

	if (!node->children)
	{
		printf("ERROR, load_arg_attrs, xml node is empty!!\n");
		return;
	}
	cur_node = node->children;
	while (cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"count") == 0)
				generic_xml_gint_import(cur_node,&arg->count);
			if (g_strcasecmp((gchar *)cur_node->name,"size") == 0)
			{
				generic_xml_gchar_import(cur_node,&tmpbuf);
				arg->size = translate_string(tmpbuf);
			}
		}
		cur_node = cur_node->next;
	}
}



