/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
   	int i;

   	init_all();
	printf("<style type=\"text/css\">A {color: #0000f0}</style>");
	printf("<center>\n");
   	printf("%s -- ���������� <hr color=\"green\">\n", BBSNAME);
   	printf("<table>\n");
   	printf("<tr><td>����</td><td>���</td><td>����</td></tr>\n");
   	for(i=0; i<SECNUM; i++)
	{
		printf("<tr><td>%d</td><td><a href=\"bbsboa?group=%d\">%s</a></td>",
				i, i, secname[i][0]);
		printf("<td><a href=\"bbsboa?group=%d\">%s</a></td></tr>\n",
				i, secname[i][1]);
   	}
   	printf("</table><hr>\n");
   	printf("[<a href=\"bbsall\">ȫ��������</a>]");
   	printf("[<a href=\"bbs0an\">����������</a>]");
   	printf("</center>\n");
	http_quit();
}
