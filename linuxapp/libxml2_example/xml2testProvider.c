//=====================================================================
/**
 * section: XPath
 * synopsis: Evaluate XPath expression and prints result node set.
 * purpose: Shows how to evaluate XPath expression and register
 *          known namespaces in XPath context.
 * usage: xpath1 <xml-file> <xpath-expr> [<known-ns-list>]
 * test: ./xpath1 test3.xml '//child2' > xpath1.tmp ; diff xpath1.tmp xpath1.res ; rm xpath1.tmp
 * author: Aleksey Sanin
 * copy: see Copyright for the status of this software.
 */
/*
 <?xml version="1.0" encoding="UTF-8"?>
 <document xmlns:xi="http://www.w3.org/2003/XInclude">
 <p>
 <q>fasdf</q>
 </p>
 </document>
 保存为test.xml
./xml2test version.xml /VersionInfo/File[@type=\'so\']

 运行命令行xpath1 test.xml //p//q
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#define MANIFEST_FILE "serviceproviders.xml"

bool find_provider_apn(char *apnvalue,int opcode)
{

	xmlDocPtr doc;
	xmlXPathContextPtr xpathCtx;
	xmlXPathObjectPtr xpathObj;
	int findNode = 0;
	xmlChar * xpathExprRootfs[PATH_MAX];// = (xmlChar *) "//serviceproviders//country//gsm//network-id[@mcc=\'460\' and @mnc=\'01\']";

	sprintf((char *)xpathExprRootfs,"//serviceproviders//country//gsm//network-id[@mcc=\'%03d\' and @mnc=\'%02d\']",(opcode/100)%1000,opcode%100);
	printf("%s\n",(char *)xpathExprRootfs);
	/* Load XML document */
	doc = xmlParseFile(MANIFEST_FILE);
	if (doc == NULL)
	{
		fprintf(stderr, "Error: unable to parse file \"%s\"\n", MANIFEST_FILE);
		return (-1);
	}

	/* Create xpath evaluation context */
	xpathCtx = xmlXPathNewContext(doc);
	if (xpathCtx == NULL)
	{
		fprintf(stderr, "Error: unable to create new XPath context\n");
		xmlFreeDoc(doc);
		return (-1);
	}

//	print_xpath_nodes(xpathObj->nodesetval, stdout);
	int size, i;
	xmlNodePtr cur;

	/* Evaluate xpath expression */
		xpathObj = xmlXPathEvalExpression(xpathExprRootfs, xpathCtx);
		if (xpathObj == NULL)
		{
			fprintf(stderr, "Error: unable to evaluate xpath expression \"%s\"\n",
					xpathExprRootfs);
			xmlXPathFreeContext(xpathCtx);
			xmlFreeDoc(doc);
			return (-1);
		}

		/* Print results */
		size = (xpathObj->nodesetval) ? xpathObj->nodesetval->nodeNr : 0;

		for (i = 0; i < size; ++i)
		{
			if (xpathObj->nodesetval->nodeTab[i]->type == XML_ELEMENT_NODE)
			{
				cur = xpathObj->nodesetval->nodeTab[i];
				if (!cur->ns && !findNode)
				{
//					printf("= element node \"%s\"\n", cur->name);
					xmlNodePtr gsm = cur->parent;
					printf("%s\n",gsm->name);
					if (strcmp((const char*) gsm->name, "gsm") == 0 )
					{
						xmlNodePtr gsmNode = gsm->xmlChildrenNode;
						while(gsmNode != NULL  && !findNode)
						{
							if (strcmp((const char*) gsmNode->name, "apn") == 0 )
							{
								xmlChar * apnname = NULL;//,*other;//,*other1,*other2;
								apnname = xmlGetProp(gsmNode, BAD_CAST "value");
								printf("apnname:%s\n",apnname);
								xmlNodePtr apn = gsmNode->xmlChildrenNode;
								while(apn != NULL  && !findNode)
								{
									if (strcmp((const char*) apn->name, "usage") == 0 )
									{
										xmlChar * usagetype = NULL;//,*other;//,*other1,*other2;
										usagetype = xmlGetProp(apn, BAD_CAST "type");

										if (strcmp((const char*)usagetype, "internet") == 0 )
										{
											findNode = 1;
										}
										printf("usagetype:%s,%d\n",usagetype,findNode);

									}
									apn = apn->next;
								}

							}
							gsmNode = gsmNode->next;
						}

					}
				}
			}
		}


	/* Cleanup */
	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx);
	xmlFreeDoc(doc);

	return findNode;

}
int main(void)
{
	find_provider_apn(NULL,46001);
//	fprintf(stderr, "XPath support not compiled in\n");
	exit(1);
}
