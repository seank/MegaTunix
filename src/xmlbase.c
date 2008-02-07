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
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <xmlbase.h>

extern gint dbg_lvl;


void load_integer_from_xml(xmlNode *node, gint *dest)
{
	if (!node->children)
	{
		printf("ERROR, load_integer_from_xml, xml node is empty!!\n");
		return;
	}
	if (!(node->children->type == XML_TEXT_NODE))
		return;
	*dest = (gint)g_ascii_strtod((gchar*)node->children->content,NULL);

}

void load_string_from_xml(xmlNode *node, gchar **dest)
{
	if (!node->children) /* EMPTY node, thus, clear the var on the gauge */
	{
		if (*dest)
			g_free(*dest);
		*dest = g_strdup("");
		return;
	}
	if (!(node->children->type == XML_TEXT_NODE))
		return;

	if (*dest)
		g_free(*dest);
	if (node->children->content)
		*dest = g_strdup((gchar*)node->children->content);
	else
		*dest = g_strdup("");

}
