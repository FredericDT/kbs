#include "bbs.h"

int
uleveltochar( char* buf, struct userec *lookupuser ) // ȡ�û�Ȩ������˵�� Bigman 2001.6.24
{
	unsigned lvl;
	char userid[IDLEN+2];
	
	lvl = lookupuser->userlevel;
	strncpy( userid, lookupuser->userid, IDLEN+2 );

	// buf[10], buf ��� 4 ������ + 1 byte ��\0��β��
	//���� level
   	if( lvl < PERM_DEFAULT ) strcpy( buf, "����" );
    	else if( lvl & PERM_SYSOP ) strcpy(buf,"����");
      	else if( lvl & PERM_MM && lvl & PERM_CHATCLOAK ) strcpy(buf,"��������");
      	else if( lvl & PERM_MM ) strcpy(buf,"�������");
      	else  if( lvl & PERM_CHATCLOAK ) strcpy(buf,"�޵�ţʭ");
      	else if  ( lvl & PERM_BOARDS ) strcpy(buf,"�����ҵ�");
      	else if ( lvl & PERM_DENYPOST ) strcpy( buf, "�ư�" ); 
      	else if ( ! (lvl & PERM_BASIC) ) strcpy( buf, "Ϲ��" ); 
      	else strcpy(buf,"��ͷ����");

	//����˵�������� level
    	if( !strcmp(lookupuser->userid,"SYSOP")
		    || !strcmp(lookupuser->userid,"Rama") )
	    strcpy( buf, "ǿ��ͷ" );
    	else if( !strcmp(lookupuser->userid,"zixia") )
	    strcpy( buf, "����" );
    	else if( !strcmp(lookupuser->userid,"halen") )
	    strcpy( buf, "СƤ����" );
    	else if( !strcmp(lookupuser->userid,"cityhunter") 
		    || !strcmp(lookupuser->userid,"soso")
	  	    || !strcmp(lookupuser->userid,"KCN") )
	    strcpy( buf, "ţħ��" );
    	else if( !strcmp(lookupuser->userid,"guest") )
	    strcpy( buf, "����" );

    	return 1;
}

#if 0
int
uleveltochar( char *buf, struct userec *lookupuser ) // ȡ�û�Ȩ������˵�� Bigman 2001.6.24
{
	unsigned lvl;
	char userid[IDLEN+2];
	
	lvl = lookupuser->userlevel;
	strncpy( userid, lookupuser->userid, IDLEN+2 );

    if( !(lvl &  PERM_BASIC) ) {
	strcpy( buf, "����");
        return 0;
    }
/*    if( lvl < PERM_DEFAULT )
    {
        strcpy( buf, "- --" );
        return 1;
    }
*/

    /* Bigman: �������Ĳ�ѯ��ʾ 2000.8.10 */
    /*if( lvl & PERM_ZHANWU ) strcpy(buf,"վ��");*/
    if( (lvl & PERM_ANNOUNCE) && (lvl & PERM_OBOARDS) ) strcpy(buf,"վ��");
    else  if( lvl & PERM_CHATCLOAK ) strcpy(buf,"Ԫ��");
    else if (lvl & PERM_CHATOP) strcpy(buf,"ChatOP");
    else if  ( lvl & PERM_BOARDS ) strcpy(buf,"����");
    else  if( lvl & PERM_HORNOR ) strcpy(buf,"����");
	/* Bigman: �޸���ʾ 2001.6.24 */
	else if (lvl & (PERM_LOGINOK))
	{
		if (!(lvl & (PERM_CHAT)) || !(lvl & (PERM_PAGE)) || !(lvl & (PERM_POST)) ||(lvl & (PERM_DENYMAIL)) ||(lvl &(PERM_DENYPOST))) strcpy(buf,"����");	
		else strcpy(buf,"�û�");
	}
	else if (!(lvl & (PERM_CHAT)) && !(lvl & (PERM_PAGE))  && !(lvl & (PERM_POST))) strcpy(buf,"����");
	else strcpy(buf,"����");

/*    else {
        buf[0] = (lvl & (PERM_SYSOP)) ? 'C' : ' ';
        buf[1] = (lvl & (PERM_XEMPT)) ? 'L' : ' ';
        buf[2] = (lvl & (PERM_BOARDS)) ? 'B' : ' ';
        buf[3] = (lvl & (PERM_DENYPOST)) ? 'p' : ' ';
        if( lvl & PERM_ACCOUNTS ) buf[3] = 'A';
        if( lvl & PERM_SYSOP ) buf[3] = 'S'; 
        buf[4] = '\0';
    }
*/

    return 1;
}
#endif
