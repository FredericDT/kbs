/*
 * $Id$
 */
#include "bbslib.h"

char *encode_url(char *buf, const char* str, size_t buflen)
{
	int i, j;
	int len;

	bzero(buf, buflen);
	len = strlen(str);
	for (i = 0, j = 0; i < len && j < buflen; i++)
	{
		if (str[i] == '"')
		{
			snprintf(&buf[j], buflen-j, "&quot;");
			j = strlen(buf);
		}
		else
		{
			buf[j] = str[i];
			j++;
		}
	}
	buf[buflen - 1] = '\0';

	return buf;
}

int is_BM(struct boardheader *board, struct userec *user)
{
	char BM[STRLEN];

	strncpy(BM, board->BM, sizeof(BM)-1);
	BM[sizeof(BM)-1] = '\0';
	return chk_currBM(BM, user);
}

int is_owner(struct fileheader *fh, struct userec *user)
{
	if (!strcmp(fh->owner, user->userid))
		return 1;
	else
		return 0;
}

int can_delete_post(struct boardheader *board, struct fileheader *fh, 
		struct userec *user)
{
	if (is_BM(board, user) || is_owner(fh, user))
		return 1;
	else
		return 0;
}

int can_edit_post(struct boardheader *board, struct fileheader *fh, 
		struct userec *user)
{
	if (is_BM(board, user) || is_owner(fh, user))
		return 1;
	else
		return 0;
}

int main()
{
	FILE *fp;
	char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
	struct fileheader x;
	int num, tmp, total;
	struct userec *user = NULL;
	struct boardheader *bp;

	init_all();
	strsncpy(board, getparm("board"), 32);
	strsncpy(file, getparm("file"), 32);
	num=atoi(getparm("num"));
	printf("<center>\n");
	if(!has_read_perm(currentuser, board))
		http_fatal("�����������");
	bp = getbcache(board);
	strcpy(board, bp->filename);
	if (valid_filename(file) < 0)
		http_fatal("����Ĳ���");
	if (get_file_ent(board, file, &x) <= 0)
		http_fatal("������ļ���");
	printf("%s -- �����Ķ� [������: %s]<hr color=\"green\">", BBSNAME, board);
	sprintf(dir, "boards/%s/.DIR", board);
	total=file_size(dir)/sizeof(x);
	if(total<=0)
		http_fatal("�������������ڻ���Ϊ��");
	printf("<table width=\"610\" border=\"1\">\n");
	printf("<tr><td>\n<pre>");
	sprintf(filename, "boards/%s/%s", board, file);
	fp=fopen(filename, "r");
	if(fp==0)
		http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
	while(1)
	{
		char *id, *s;
		if(fgets(buf, 512, fp)==0)
			break;
		if(!strncmp(buf, "������: ", 8))
		{
			ptr=strdup(buf);
			id=strtok(ptr+8, " ");
			s=strtok(0, "");
			if(id==0) id=" ";
			if(s==0) s="\n";
			if(strlen(id)<13 && getuser(id, &user))
			{
				printf("������: %s%s", userid_str(id), s);
				free(ptr);
				continue;
			}
			free(ptr);
		}
		if(!strncmp(buf, ": ", 2))
			printf("<font color=\"808080\">");
		hhprintf("%s", void1(buf));
		if(!strncmp(buf, ": ", 2))
			printf("</font>");
	}
	fclose(fp);

	brc_initial(currentuser->userid, board);
	brc_add_read(x.filename);
	brc_update(currentuser->userid);
	printf("</pre></td></tr>\n</table><hr>\n");
	printf("[<a href=\"bbsfwd?board=%s&file=%s\">ת��/�Ƽ�</a>]", board, file);
	printf("[<a href=\"bbsccc?board=%s&file=%s\">ת��</a>]", board, file);
	if (can_delete_post(bp, &x, getcurrusr()))
		printf("[<a onclick=\"return confirm('�����Ҫɾ��������?')\" href=\"bbsdel?board=%s&file=%s\">ɾ������</a>]", board, file);
	if (can_edit_post(bp, &x, getcurrusr()))
	{
		printf("[<a href=\"bbsedit?board=%s&file=%s\">�޸�����</a>]",
				board, file);
	}
	/*
	 * TODO: ������Ҫ���ζ�������Ӧ�ü�Ϊһ����
	 */
	fp = fopen(dir, "r+");
	if(fp==0)
		http_fatal("dir error2");
	if(num>0)
	{
		fseek(fp, sizeof(x)*(num-1), SEEK_SET);
		fread(&x, sizeof(x), 1, fp);
		printf("[<a href=\"bbscon?board=%s&file=%s&num=%d\">��һƪ</a>]",
				board, x.filename, num-1);
	}
	printf("[<a href=\"bbsdoc?board=%s\">��������</a>]", board);
	if(num<total-1)
	{
		fseek(fp, sizeof(x)*(num+1), SEEK_SET);
		fread(&x, sizeof(x), 1, fp);
		printf("[<a href=\"bbscon?board=%s&file=%s&num=%d\">��һƪ</a>]",
				board, x.filename, num+1);
	}
	fclose(fp);
     	ptr=x.title;
     	if(!strncmp(ptr, "Re: ", 4)) ptr+=4;
	ptr[60]=0;
	if ((x.accessed[1] & FILE_READ) == 0)
        printf("[<a href=\"bbspst?board=%s&file=%s&userid=%s&title=Re: %s\">������</a>]",
			board, file, x.owner, encode_url(buf, void1(ptr), sizeof(buf)));
     	printf("[<a href=\"bbstfind?board=%s&title=%s\">ͬ�����Ķ�</a>]\n",
			board, encode_url(buf, void1(ptr), sizeof(buf)));
   	printf("</center>\n"); 
	http_quit();
}
