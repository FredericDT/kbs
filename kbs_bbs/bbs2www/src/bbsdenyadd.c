#include "bbslib.h"
struct deny {
	char id[80];
	char exp[80];
	int free_time;
} denyuser[256];
int denynum=0;

int loaddenyuser(char *board) {
	FILE *fp;
	char path[80], buf[256];
	sprintf(path, "boards/%s/deny_users", board);
	fp=fopen(path, "r");
	if(fp==0) return;
	while(denynum<100) {
		if(fgets(buf, 80, fp)==0) break;
		sscanf(buf, "%s %s %d", denyuser[denynum].id, denyuser[denynum].exp, &denyuser[denynum].free_time);
		denynum++;
	}
	fclose(fp);
}

int savedenyuser(char *board) {
	FILE *fp;
	int i;
	char path[80], buf[256], *exp;
	sprintf(path, "boards/%s/deny_users", board);
	fp=fopen(path, "w");
	if(fp==0) return;
	for(i=0; i<denynum; i++) {
		int m;
		exp=denyuser[i].exp;
		if(denyuser[i].id[0]==0) continue;
		for(m=0; exp[m]; m++) {
			if(exp[m]<=32 && exp[m]>0) exp[m]='.';
		}
		fprintf(fp, "%-12s %s %d\n", denyuser[i].id, denyuser[i].exp, denyuser[i].free_time);
	}
	fclose(fp);
}

int main() {
	int i; 
	char exp[80], board[80], *userid;
	int dt;
	struct userec *u = NULL;

	init_all();
   	if(!loginok) http_fatal("����δ��¼, ���ȵ�¼");
	strsncpy(board, getparm("board"), 30);
	strsncpy(exp, getparm("exp"), 30);
	dt=atoi(getparm("dt"));
	if(!has_read_perm(&currentuser, board)) http_fatal("�����������");
	if(!has_BM_perm(&currentuser, board)) http_fatal("����Ȩ���б�����");
	loaddenyuser(board);
	userid=getparm("userid");
	if(userid[0]==0) return show_form(board);
	if(getuser(userid, &u)==0) http_fatal("�����ʹ�����ʺ�");
	strcpy(userid, u->userid);
	if(dt<1 || dt>99) http_fatal("�����뱻������(1-99)");
	if(exp[0]==0) http_fatal("���������ԭ��");
   	for(i=0; i<denynum; i++)
		if(!strcasecmp(denyuser[i].id, userid)) http_fatal("���û��Ѿ�����");
	if(denynum>40) http_fatal("̫���˱�����");
	strsncpy(denyuser[denynum].id, userid, 13);
	strsncpy(denyuser[denynum].exp, exp, 30);
	denyuser[denynum].free_time=time(0)+dt*86400;
	denynum++;
	savedenyuser(board);
	printf("��� %s �ɹ�<br>\n", userid);
	inform(board, userid, exp, dt);
	printf("[<a href=bbsdenyall?board=%s>���ر����ʺ�����</a>]", board);
	http_quit();
}

int show_form(char *board) {
	printf("<center>%s -- ������� [������: %s]<hr color=green>\n", BBSNAME, board);
	printf("<form action=bbsdenyadd><input type=hidden name=board value='%s'>", board);
	printf("���ʹ����<input name=userid size=12> ����POSTȨ <input name=dt size=2> ��, ԭ��<input name=exp size=20>\n");
	printf("<input type=submit value=ȷ��></form>");
}

int inform(char *board, char *user, char *exp, int dt) {
	FILE *fp;
	char path[80], title[80];
	sprintf(title, "[����] ���%s����POSTȨ%d��", user, dt);
	sprintf(path, "tmp/%d.tmp", getpid());
	fp=fopen(path, "w");
	fprintf(fp, "�����������Զ�����ϵͳ�Զ�������\n\n");
	fprintf(fp, "%s��������Ա[%s]����˱���POSTȨ[%d]��.\n", user, currentuser->userid, dt);
	fprintf(fp, "ԭ����: %s\n", exp);
	fclose(fp);
	// temporary
	//post_article(board, title, path, "deliver", "�Զ�����ϵͳ", "�Զ�����ϵͳ", -1);
	post_mail(user, title, path, currentuser->userid, currentuser->username, fromhost, -1);
	unlink(path);
	printf("ϵͳ�Ѿ�����֪ͨ��%s.<br>\n", user);
}
