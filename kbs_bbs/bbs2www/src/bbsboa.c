/*
 * $Id$
 */
#include "bbslib.h"

extern int     brdnum;
extern int yank_flag;
extern char    *boardprefix;

/*static int cmp_board(struct boardheader *b1, struct boardheader *b2)
{
	return strcasecmp(b1->filename, b2->filename);
}*/

/*
 * from boards_t.c
*/
int
cmpboard( brd, tmp ) /*������*/
struct newpostdata      *brd, *tmp;
{
	register int        type = 0;

	if ( !(currentuser->flags[0] & BRDSORT_FLAG) )
	{
		type = brd->title[0] - tmp->title[0];
		if (type==0)
			type=strncasecmp( brd->title+1, tmp->title+1,6);

	}
	if ( type == 0 )
		type = strcasecmp( brd->name, tmp->name );
	return type;
}

static int check_newpost( struct newpostdata *ptr)
{
	struct BoardStatus* bptr;
	ptr->total = ptr->unread = 0;

	bptr = getbstatus(ptr->pos);
	if (bptr == NULL)
		return 0;
	ptr->total = bptr->total;

	if (!brc_initial(currentuser->userid,ptr->name))
	{
		ptr->unread = 1;
	}
	else
	{
		if (brc_unread(bptr->lastpost))
		{
			ptr->unread = 1;
		}
	}
	return 1;
}

/*
 * Return value:
 *     board number loaded for success
 *     -1                  for error
*/
int brd_show_boards(int sec, char *cgi)
{
	char        buf[ 256 ];
	struct newpostdata newpost_buffer[ MAXBOARD ];
	struct newpostdata  *ptr;
	char *ptr2;
	int i;

	sprintf( buf, "EGROUP%c", seccode[sec][0]);
	boardprefix = sysconf_str( buf );
	yank_flag = 0;
	if ( !strcmp( currentuser->userid, "guest" ) )
		yank_flag = 1;
	nbrd = newpost_buffer;
	brdnum = 0;
	if (load_boards() == -1)
		return -1;
	qsort( nbrd, brdnum, sizeof( nbrd[0] ), 
		   (int (*)(const void *, const void *))cmpboard );
	printf("<style type=\"text/css\">A {color: #0000f0}</style>");
	printf("<center>\n");
	printf("%s -- ���������� [%s]<hr color=\"green\">", BBSNAME, secname[sec]);
	printf("<table width=\"610\">\n");
	printf("<tr><td>���</td><td>δ</td><td>����������</td><td>���</td><td>��������</td><td>����</td><td>������</td></tr>\n");
	for (i=0; i<brdnum; i++)
	{
		ptr = &nbrd[i];
		check_newpost( ptr );
		printf("<tr><td>%d</td><td>%s</td>", i+1, ptr->unread ? "��" : "��");
		printf("<td><a href=\"%s?board=%s\">%s</a></td>", cgi, ptr->name, ptr->name);
		printf("<td>%6.6s</td>", ptr->title+1);
		printf("<td><a href=\"%s?board=%s\">%s</a></td>", cgi, ptr->name, ptr->title+7);
		strncpy(buf, ptr->BM, sizeof(buf)-1);
		buf[sizeof(buf)-1] = '\0';
		ptr2 = strchr(buf, ' ');
		if (ptr2 == NULL)
		{
			ptr2 = "����������";
			printf("<td>%s</td>", ptr2);
		}
		else
		{
			*ptr2 = '\0';
			printf("<td><a href=\"bbsqry?userid=%s\">%s</a></td>", ptr2, ptr2);
		}
		printf("<td>%d</td></tr>\n", ptr->total);
	}
	printf("</table><hr></center>\n");
	return brdnum;
}

int main()
{
	int sec1;
	char *cgi="bbstdoc";

	init_all();
	sec1 = atoi(getsenv("QUERY_STRING"));
	if (sec1<0 || sec1>=SECNUM)
		http_fatal("����Ĳ���");
	if (atoi(getparm("my_def_mode"))!=0)
		cgi="bbsdoc";
	brd_show_boards(sec1, cgi);
	http_quit();
}

