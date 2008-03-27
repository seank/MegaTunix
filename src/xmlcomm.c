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
#include <xmlcomm.h>
#include <xmlbase.h>


extern gint dbg_lvl;
extern GObject *global_data;

void load_comm_xml(gchar *filename)
{
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	if (filename == NULL)
	{
		printf("xmlcomm filename is NULL!\n");
		return;
	}

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
	load_xmlcomm_elements(root_element);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	g_hash_table_foreach((GHashTable *)OBJ_GET(global_data,"commands_hash"),xmlcomm_dump_commands,NULL);

}

void load_xmlcomm_elements(xmlNode *a_node)
{
	static GHashTable *arguments = NULL;
	static GHashTable *commands = NULL;
	xmlNode *cur_node = NULL;

	if (!arguments)
		arguments = (GHashTable *)OBJ_GET(global_data,"potential_arguments");
	if (!commands)
		commands = (GHashTable *)OBJ_GET(global_data,"commands_hash");

	/* Iterate though all nodes... */
	for (cur_node = a_node;cur_node;cur_node = cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"potential_args") == 0)
				load_potential_args(arguments,cur_node);
			if (g_strcasecmp((gchar *)cur_node->name,"commands") == 0)
				load_commands(commands,cur_node);
		}
		load_xmlcomm_elements(cur_node->children);
	}
}

void load_potential_args(GHashTable *arguments, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	PotentialArg *arg = NULL;

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
				load_arg_details(arg, cur_node);
				g_hash_table_insert(arguments,g_strdup(arg->name),arg);
			}
		}
		cur_node = cur_node->next;

	}
}


void load_commands(GHashTable *commands_hash, xmlNode *node)
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
				cmd->post_functions = g_array_new(FALSE,TRUE,sizeof(PostFunction *));
				cmd->args = g_array_new(FALSE,TRUE,sizeof(gchar *));
				load_cmd_details(cmd, cur_node);
				g_hash_table_insert(commands_hash,g_strdup(cmd->name),cmd);
			}
		}
		cur_node = cur_node->next;

	}
}


void load_arg_details(PotentialArg *arg, xmlNode *node)
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
				g_free(tmpbuf);
				tmpbuf = NULL;
			}
		}
		cur_node = cur_node->next;
	}
}


void load_cmd_details(Command *cmd, xmlNode *node)
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
			if (g_strcasecmp((gchar *)cur_node->name,"type") == 0)
			{
				generic_xml_gchar_import(cur_node,&tmpbuf);
				cmd->type = translate_string(tmpbuf);
				g_free(tmpbuf);
				tmpbuf = NULL;
			}
			if (g_strcasecmp((gchar *)cur_node->name,"func_call_name") == 0)
				generic_xml_gchar_import(cur_node,&cmd->func_call_name);
			if (g_strcasecmp((gchar *)cur_node->name,"func_call_arg") == 0)
			{
				generic_xml_gchar_import(cur_node,&tmpbuf);
				cmd->func_call_arg = translate_string(tmpbuf);
				g_free(tmpbuf);
				tmpbuf = NULL;
			}
			if (g_strcasecmp((gchar *)cur_node->name,"base") == 0)
				generic_xml_gchar_import(cur_node,&cmd->base);

			if (g_strcasecmp((gchar *)cur_node->name,"return_data_func") == 0)
				generic_xml_gchar_import(cur_node,&cmd->return_data_func);
			if (g_strcasecmp((gchar *)cur_node->name,"return_data_arg") == 0)
			{
				generic_xml_gchar_import(cur_node,&tmpbuf);
				cmd->return_data_arg = translate_string(tmpbuf);
				g_free(tmpbuf);
				tmpbuf = NULL;
			}
			if (g_strcasecmp((gchar *)cur_node->name,"post_functions") == 0)
				load_cmd_post_functions(cmd,cur_node);
			if (g_strcasecmp((gchar *)cur_node->name,"args") == 0)
				load_cmd_args(cmd,cur_node);
		}
		cur_node = cur_node->next;
	}
}


void load_cmd_args(Command *cmd, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	gchar * tmpbuf = NULL;

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
				generic_xml_gchar_import(cur_node,&tmpbuf);
				cmd->args = g_array_append_val(cmd->args,tmpbuf);
				tmpbuf = NULL;
			}
		}
		cur_node = cur_node->next;
	}
}

void load_cmd_post_functions(Command *cmd, xmlNode *node)
{
	xmlNode *cur_node = NULL;
	GModule *module = NULL;
	PostFunction *pf = NULL;

	if (!node->children)
	{
		printf("ERROR, load_cmd_post_functions, xml node is empty!!\n");
		return;
	}

	module = g_module_open(NULL,G_MODULE_BIND_LAZY);
	if (!module)
		if (dbg_lvl & (CRITICAL))
			dbg_func(g_strdup_printf(__FILE__": load_cmd_post_functions()\n\tUnable to call g_module_open, error: %s\n",g_module_error()));

	cur_node = node->children;
	while (cur_node->next)
	{
		if (cur_node->type == XML_ELEMENT_NODE)
		{
			if (g_strcasecmp((gchar *)cur_node->name,"function") == 0)
			{
				pf = g_new0(PostFunction, 1);
				generic_xml_gchar_import(cur_node,&pf->name);
				if (!g_module_symbol(module,pf->name,(void *)&pf->function))
				{
					if (dbg_lvl & (CRITICAL))
						dbg_func(g_strdup_printf(__FILE__": load_cmd_post_functions()\n\tError finding symbol \"%s\", error:\n\t%s\n",pf->name,g_module_error()));
					if (!g_module_close(module))
					{
						if (dbg_lvl & (CRITICAL))
							dbg_func(g_strdup_printf(__FILE__": load_cmd_post_functions()\n\t Failure calling \"g_module_close()\", error %s\n",g_module_error()));
					}
				}
				g_array_append_val(cmd->post_functions,pf);
			}
		}
		cur_node = cur_node->next;
	}
	if (!g_module_close(module))
	{
		if (dbg_lvl & (CRITICAL))
			dbg_func(g_strdup_printf(__FILE__": load_cmd_post_functions()\n\t Failure calling \"g_module_close()\", error %s\n",g_module_error()));
	}

}


void xmlcomm_dump_commands(gpointer key, gpointer value, gpointer data)
{
	Command *cmd = NULL;
	PostFunction *pf = NULL;
	gint i = 0;

	cmd = (Command *)value;
	printf("Command key %s\n",(gchar *)key);
	printf("Command name %s\n",cmd->name);
	printf("Command desc %s\n",cmd->desc);
	if (cmd->base)
		printf("Command base %s\n",cmd->base);
	if (cmd->return_data_func)
		printf("Return data function \"%s\()\"\n",cmd->return_data_func);
	if (cmd->args->len > 0 )
	{
		printf("Command args (%i): \n",cmd->args->len);
		for (i=0;i<cmd->args->len;i++)
			printf("  %s\n",g_array_index(cmd->args,gchar *,i));
	}
	if (cmd->post_functions->len > 0 )
	{
		printf("Post Functions (%i): \n",cmd->post_functions->len);
		for (i=0;i<cmd->post_functions->len;i++)
		{
			pf = g_array_index(cmd->post_functions,PostFunction *,i);
			printf("  %s: %p\n",pf->name,pf->function);
		}
	}
	if (cmd->type == FUNC_CALL)
		printf("Function call %s\n",cmd->func_call_name);
	printf("\n\n");
}
