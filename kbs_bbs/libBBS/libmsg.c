#include "bbs.h"
#include "screen.h"
#define MAXMESSAGE 5
char MsgDesUid[14];
char msgerr[255];
int getuinfopid(void);
int
sendmsgfunc(uentp,msgstr,mode)
struct user_info *uentp;
char msgstr[256];
int mode;
{
    char uident[STRLEN] ,ret_str[20];
    FILE *fp;
    time_t now;
    struct user_info *uin ;
    char buf[80],msgbuf[256] ,*timestr,msgbak[256];
    int msg_count=0;
    int Gmode = 0;

    *msgbak = 0;	/* period 2000-11-20 may be used without init */
    *msgbuf = 0;
    *msgerr=0; /* clear msg error */
    {
        /*  if(!strcasecmp(uentp->userid,currentuser->userid))	rem by Haohmaru,�����ſ����Լ����Լ���msg
            return 0;    
        */   uin=uentp;
        strcpy(uident,uin->userid);
        /*   strcpy(MsgDesUid, uin->userid); change by KCN,is wrong*/
    }
    if(!HAS_PERM(PERM_SEECLOAK) && uin->invisible && strcmp(uin->userid,currentuser->userid) && mode!=4)
        return -2;


    if((mode!=3)&&(LOCKSCREEN == uin->mode)) /* Leeward 98.02.28 */
    {
    	strcpy(msgerr,"�Է��Ѿ�������Ļ�����Ժ��ٷ������(��)д��...\n");
        return -1 ;
    }

    if ((mode!=3)&&(NA==canIsend2(uin->userid)))/*Haohmaru.06.06.99.����Լ��Ƿ�ignore*/
    {
    	strcpy(msgerr,"�Է��ܾ��������ѶϢ...\n");
        return -1;
    }
    if (mode!=3) {
    sethomefile(buf,uident,"msgcount");
    fp=fopen(buf, "rb");
    if (fp!=NULL)
    {
        fread(&msg_count,sizeof(int),1,fp);
        fclose(fp);

        if(msg_count>MAXMESSAGE)
        {
            strcpy(msgerr,"�Է�����һЩѶϢδ�������Ժ��ٷ������(��)д��...\n");
            return -1 ;
        }
    }
    }
    if (msgstr == NULL) {
    	// per-sending check only
    	return 0;
    }
    now=time(0);
    timestr=ctime(&now)+11;
    *(timestr+8)='\0';
    strcpy(ret_str,"R ��ѶϢ");
    if(mode == 0||mode==2||mode==4)
    {
        sprintf(msgbuf,"[44m[36m%-12.12s[33m(%-5.5s):[37m%-59.59s[m[%dm\033[%dm\n", currentuser->userid,
                timestr, msgstr,getuinfopid+100,uin->pid+100);
        sprintf(msgbak,"[44m[0;1;32m=>[37m%-10.10s[33m(%-5.5s):[36m%-59.59s[m[%dm\033[%dm\n", uident,timestr, msgstr,getuinfopid+100,uin->pid+100);
    }else
    {
        if(mode==3) {
            sprintf(msgbuf,"[44m[33mվ��� %8.8s ʱ�㲥��"
                    "[37m%-55.55s[m\033[%dm\n",
                    /*				"[37m%-59.59s[m\033[%dm\n",*/
                    timestr,msgstr,uin->pid+100);
        }
        else if(mode==1)
        {
            sprintf(msgbuf,"[44m[36m%-12.12s(%-5.5s) ������[37m%-43.43s(%s)[m[%dm\033[%dm\n",
                    currentuser->userid, timestr, msgstr,ret_str,getuinfopid+100,uin->pid+100);
            sprintf(msgbak,"[44m[37m��(%-5.5s) ����%-12.12s[36m%-43.43s(%s)[m[%dm\033[%dm\n", timestr,uident,msgstr,ret_str,getuinfopid+100,uin->pid+100);
        }else if(mode==3)
        {
            sprintf(msgbuf,"[44m[32mBBS ϵͳͨ��(%-5.5s):[37m%-59.59s[31m(%s)[m\033[%dm\n",
                    timestr, msgstr,ret_str,uin->pid+100);
        }
    }
    if (Gmode == 2)
        sprintf(msgbuf,"[44m[33mվ��� %8.8s ʱ�㲥��[37m%-59.59s[m\033[%dm\n",timestr,buf,uin->pid+100);
    /* �������msg��Ŀ��uid�Ƿ��Ѿ��ı�  1998.7.5 by dong*/
    uin=t_search(MsgDesUid,uentp->pid);

    if ((uin == NULL) || (uin->active == 0) || (uin->pid == 0) || (kill(uin->pid, 0) !=0) && (uentp->pid != 1))
    { /*
        uin=t_search(MsgDesUid, NA);
            if ((uin == NULL) || (uin->active == 0) || (uin->pid == 0) || (kill(uin->pid, 0) !=0)){ */
        if (mode == 0)
            return -2;
        strcpy(msgerr, "�Է��Ѿ�����....\n");   
        return -1; /* �Է��Ѿ����� */
        /*} */
    }

    sethomefile(buf,uident,"msgfile");
    if((fp=fopen(buf,"a"))==NULL)
        return -2;
    fputs(msgbuf,fp);
    fclose(fp);

    /*Haohmaru.99.6.03.�ص�msgҲ��¼*/
    if(strcmp(currentuser->userid,uident)){
        sethomefile(buf,currentuser->userid,"msgfile");
        if((fp=fopen(buf,"a"))==NULL)
            return -2;
        fputs(msgbak,fp);
        fclose(fp);
    }
    if(uentp->pid !=1 && kill(uin->pid,SIGUSR2)==-1&&msgstr==NULL)
    {
    	
        strcpy(msgerr,"\n�Է��Ѿ�����.....\n") ; 
        return -1;
    }
    sethomefile(buf,uident,"msgcount");
    fp=fopen(buf, "wb");
    if (fp!=NULL)
    {
        msg_count++;
        fwrite(&msg_count,sizeof(int),1,fp);
        fclose(fp);
    }
    return 1 ;
}

