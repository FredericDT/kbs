#include "bbslib.h"

//char mybrd[32][80];
int main()
{
	FILE *fp;
	char file[200], board[200];
	int rv;
	struct boardheader x;

	init_all();
	strsncpy(board, getparm("board"), 32);
	if(!loginok)
		http_fatal("��ʱ��δ��¼��������login");
	//sprintf(file, "home/%c/%s/mybrds", toupper(currentuser->userid[0]), currentuser->userid);
	//fp=fopen(file, "r");
	//if(fp)
	//{
	//	mybrdnum=fread(mybrd, sizeof(mybrd[0]), 30, fp);
	//	fclose(fp);
	//}
	if (getboardnum(board, &x) == 0)
		http_fatal("��������������");
	load_favboard(0);
	//if(get_favboard_count() >= FAVBOARDNUM)
	//	http_fatal("��Ԥ����������Ŀ�Ѵ����ޣ���������Ԥ��");
	//if(IsFavBoard(mybrdnum-1)>0)
	//	http_fatal("���Ѿ�Ԥ�������������");
	if(!has_read_perm(&currentuser, x.filename))
		http_fatal("��������������");
	rv = add_favboard(x.filename);
	switch (rv)
	{
	case -2:
		http_fatal("��Ԥ����������Ŀ�Ѵ����ޣ���������Ԥ��");
		break;
	case -1:
		http_fatal("���Ѿ�Ԥ�������������߸�������������");
		break;
	case -3:
		http_fatal("��������������");
		break;
	default:
		// do nothing
	}
	//strcpy(mybrd[mybrdnum], board);
	//fp=fopen(file, "w");
	//fwrite(mybrd, 80, mybrdnum+1, fp);
	//fclose(fp);
	save_favboard();
	printf("<script>top.f2.location='bbsleft'</script>\n");
	printf("Ԥ�� %s �������ɹ�<br><a href=\"javascript:history.go(-1)\">"
			"���ٷ���</a>", x.filename);
	http_quit();
}
