#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#define BBSUID 			80 //9999
#define BBSGID 			80 //99

#define DEFAULTBOARD    	"zixia.net"//test
#define MAXUSERS  		20000 //150,000
#define MAXBOARD  		256 //400
#define MAXACTIVE 		666  //3000

#define POP3PORT		3110	//110
#define	LENGTH_SCREEN_LINE	256	//220
#define	LENGTH_FILE_BUFFER 	260	//160
#define	LENGTH_ACBOARD_BUFFER	200	//150
#define	LENGTH_ACBOARD_LINE 	300	//80

#define LIFE_DAY_USER		366	//120
#define LIFE_DAY_LONG		666	//666
#define LIFE_DAY_SYSOP		500	//120
#define LIFE_DAY_NODIE		999	//120
#define LIFE_DAY_NEW		30	//15
#define LIFE_DAY_SUICIDE	15	//15

#define DAY_DELETED_CLEAN	7	//20

#define	REGISTER_WAIT_TIME	(30*60) // (72*60*60)
#define	REGISTER_WAIT_TIME_NAME	"30����" //72 Сʱ

#define NAME_BBS_ENGLISH	"bbs.zixia.net" //smth.org
#define	NAME_BBS_CHINESE	"������" //ˮľ�廪
#define NAME_BBS_NICK		"��ͷ��" // BBS վ

#define FOOTER_MOVIE		"  ��  ͷ  ��  " // ��  ӭ  Ͷ  ��
#define ISSUE_LOGIN		"��վʹ��һ̨X86������" //��վʹ����⹫˾������ݷ�����
#define ISSUE_LOGOUT		"�������_�ܣ�" //����������

#define NAME_USER_SHORT		"����" //�û�
#define NAME_USER_LONG		"��ͷ����" //"ˮľ�û�"
#define NAME_SYSOP		"ǿ��ͷ" //"System Operator"
#define NAME_BM			"�����ҵ�" //����
#define NAME_POLICE		"����" //"����"
#define	NAME_SYSOP_GROUP	"��ͷ��߲�" //"վ����"
#define NAME_ANONYMOUS		"Do you zixia!?" //"ˮĸ���� Today!"
#define NAME_ANONYMOUS_FROM	"ˮ����" //������ʹ�ļ�

#define NAME_MATTER		"����" //վ��
#define NAME_SYS_MANAGE		"���ڹ���" //"ϵͳά��"
#define NAME_SEND_MSG		"����ͷ" // "��ѶϢ"
#define NAME_VIEW_MSG		"����ͷ" // "��ѶϢ"

#define CHAT_MAIN_ROOM		"zixia" //main
#define	CHAT_TOPIC		"��˿����������ˮ�������" //"�����������İ�"
#define CHAT_MSG_NOT_OP		"*** ��˿����Ҫ�Ҵ��� ***" //"*** �����Ǳ������ҵ�op ***"
#define	CHAT_ROOM_NAME		"����"//"������"
#define	CHAT_SERVER		"��˿��" //"����㳡"
#define CHAT_MSG_QUIT		"�ص������ǰ" //"����ϵͳ"
#define CHAT_OP			"����" //"������ op"
#define CHAT_SYSTEM		"����" //"ϵͳ"
#define	CHAT_PARTY		"����" // "���"



static char    *explain[] = {
	"�� ͷ ��",
	"������",
	"��С�ֶ�",
        "�Ժ�����",
	"��������",
	"����Ū��",
        "����ȡ��",
        "������ʩ",
	"�¹ⱦ��",
        "�� �� ��",
         NULL
};
 
static char    *groups[] = {
	"AxFaction",
	"zixia",
	"Factions",
	"Entertain",
	"Watering",
	"Poem",
	"GoWest",
	"DouFuGirl",
  	"PandoraBox",
  	"Reserve",
         NULL
};
#if 0
    static char    *explain[] = {
        "��վϵͳ",
        "��������",
        "���Լ���",
        "ѧ����ѧ",
        "��������",
        "̸��˵��",
        "У԰��Ϣ",
        "�����Ļ�",
        "�������",
        "������Ϣ",
        "�廪��ѧ",
        "�ֵ�ԺУ",
        "��  ��",
        NULL
    };

    static char    *groups[] = {
        "system.faq",
        "rec.faq",
        "comp.faq",
        "sci.faq",
        "sport.faq",
        "talk.faq",
        "campus.faq",
        "literal.faq",
        "soc.faq",
        "network.faq",
        "thu.faq",
        "univ.faq",
        "other.faq",
        NULL
    };

#endif

#endif
