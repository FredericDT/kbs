#ifndef __SYSNAME_H_
#define __SYSNAME_H_

#define BBSUID 			9999
#define BBSGID 			99

#define DEFAULTBOARD    	"test"
#define MAXUSERS  		150000
#define MAXBOARD  		400
#define MAXACTIVE 		3000

#define POP3PORT		110
#define	LENGTH_SCREEN_LINE	220
#define	LENGTH_FILE_BUFFER 	160
#define	LENGTH_ACBOARD_BUFFER	150
#define	LENGTH_ACBOARD_LINE 	80

#define LIFE_DAY_USER		120
#define LIFE_DAY_LONG		666
#define LIFE_DAY_SYSOP		120
#define LIFE_DAY_NODIE		120
#define LIFE_DAY_NEW		15
#define LIFE_DAY_SUICIDE	15

#define DAY_DELETED_CLEAN	20

#define	REGISTER_WAIT_TIME	(72*60*60)
#define	REGISTER_WAIT_TIME_NAME	"72 Сʱ"

#define NAME_BBS_ENGLISH	"smth.org"
#define	NAME_BBS_CHINESE	"ˮľ�廪"
#define NAME_BBS_NICK		"BBS վ"

#define FOOTER_MOVIE		"��  ӭ  Ͷ  ��"
#define ISSUE_LOGIN		"��վʹ����⹫˾������ݷ�����"
#define ISSUE_LOGOUT		"����������"

#define NAME_USER_SHORT		"�û�"
#define NAME_USER_LONG		"ˮľ�û�"
#define NAME_SYSOP		"System Operator"
#define NAME_BM			"����"
#define NAME_POLICE		"����"
#define	NAME_SYSOP_GROUP	"վ����"
#define NAME_ANONYMOUS		"ˮĸ���� Today!"
#define NAME_ANONYMOUS_FROM	"������ʹ�ļ�"

#define NAME_MATTER		"վ��"
#define NAME_SYS_MANAGE		"ϵͳά��"
#define NAME_SEND_MSG		"��ѶϢ"
#define NAME_VIEW_MSG		"��ѶϢ"

#define CHAT_MAIN_ROOM		"main"
#define	CHAT_TOPIC		"�����������İ�"
#define CHAT_MSG_NOT_OP		"*** �����Ǳ������ҵ�op ***"
#define	CHAT_ROOM_NAME		"������"
#define	CHAT_SERVER		"����㳡"
#define CHAT_MSG_QUIT		"����ϵͳ"
#define CHAT_OP			"������ op"
#define CHAT_SYSTEM		"ϵͳ"
#define	CHAT_PARTY		"���"



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
