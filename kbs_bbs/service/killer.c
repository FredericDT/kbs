#define BBSMAIN
#include "bbs.h"

#define MAX_ROOM 100
#define MAX_PEOPLE 100
#define MAX_MSG 2000

#define ROOM_LOCKED 01
#define ROOM_SECRET 02
#define ROOM_DENYSPEC 04

struct room_struct {
    int w;
    int style; /* 0 - chat room 1 - killer room */
    char name[14];
    char title[NAMELEN];
    char creator[IDLEN+2];
    unsigned int level;
    int flag;
    int people;
    int maxpeople;
};

#define PEOPLE_SPECTATOR 01
#define PEOPLE_KILLER 02
#define PEOPLE_ALIVE 04
#define PEOPLE_ROOMOP 010

struct people_struct {
    int style;
    char id[IDLEN+2];
    char nick[NAMELEN];
    int flag;
    int pid;
    int vote;
    int vnum;
};

#define INROOM_STOP 1
#define INROOM_NIGHT 2
#define INROOM_DAY 3

struct inroom_struct {
    int w;
    int status;
    int killernum;
    int policenum;
    struct people_struct peoples[MAX_PEOPLE];
    char msgs[MAX_MSG][60];
    int msgpid[MAX_MSG];
    int msgi;
};

struct room_struct * rooms;
struct inroom_struct * inrooms;

int myroom, mypos;

extern int kicked;

/*void load_msgs()
{
    FILE* fp;
    int i;
    char filename[80], buf[80];
    msgst=0;
    sprintf(filename, "home/%c/%s/.INROOMMSG%d", toupper(currentuser->userid[0]), currentuser->userid, uinfo.pid);
    fp = fopen(filename, "r");
    if(fp) {
        while(!feof(fp)) {
            if(fgets(buf, 79, fp)==NULL) break;
            if(buf[0]) {
                if(!strncmp(buf, "�㱻����", 8)) kicked=1;
                if(msgst==200) {
                    msgst--;
                    for(i=0;i<msgst;i++)
                        strcpy(msgs[i],msgs[i+1]);
                }
                strcpy(msgs[msgst],buf);
                msgst++;
            }
        }
        fclose(fp);
    }
}*/

void start_change_inroom()
{
    while(inrooms[myroom].w) sleep(0);
    inrooms[myroom].w = 1;
}

void end_change_inroom()
{
    inrooms[myroom].w = 0;
}

void send_msg(int u, char* msg)
{
    int i, j;
    j=MAX_MSG;
    for(i=0;i<MAX_MSG;i++)
        if(inrooms[myroom].msgs[(i+inrooms[myroom].msgi)%MAX_MSG][0]==0) {
            j=(i+inrooms[myroom].msgi)%MAX_MSG;
            break;
        }
    if(j==MAX_MSG) {
        strcpy(inrooms[myroom].msgs[inrooms[myroom].msgi], msg);
        inrooms[myroom].msgpid[inrooms[myroom].msgi] = inrooms[myroom].peoples[u].pid;
        inrooms[myroom].msgi = (inrooms[myroom].msgi+1)%MAX_MSG;
    }
    else {
        strcpy(inrooms[myroom].msgs[j], msg);
        if(u==-1)
            inrooms[myroom].msgpid[j] = u;
        else
            inrooms[myroom].msgpid[j] = inrooms[myroom].peoples[u].pid;
    }
}

void kill_msg(int u)
{
    int i,j,k;
    char buf[80];
    for(i=0;i<MAX_PEOPLE;i++)
    if(inrooms[myroom].peoples[i].style!=-1)
    if(u==-1||i==u) {
        j=kill(inrooms[myroom].peoples[i].pid, SIGUSR1);
        if(j==-1) {
            sprintf(buf, "%s������", inrooms[myroom].peoples[i].nick[0]?inrooms[myroom].peoples[i].nick:inrooms[myroom].peoples[i].id);
            send_msg(-1, buf);
            start_change_inroom();
            inrooms[myroom].peoples[i].style=-1;
            rooms[myroom].people--;
            if(inrooms[myroom].peoples[i].flag&PEOPLE_ROOMOP) {
                for(k=0;k<MAX_PEOPLE;k++) 
                if(inrooms[myroom].peoples[k].style!=-1&&!(inrooms[myroom].peoples[k].flag&PEOPLE_SPECTATOR))
                {
                    inrooms[myroom].peoples[k].flag|=PEOPLE_ROOMOP;
                    sprintf(buf, "%s��Ϊ�·���", inrooms[myroom].peoples[k].nick[0]?inrooms[myroom].peoples[k].nick:inrooms[myroom].peoples[k].id);
                    send_msg(-1, buf);
                    break;
                }
            }
            end_change_inroom();
            i=-1;
        }
    }
}

int add_room(struct room_struct * r)
{
    int i,j;
    for(i=0;i<MAX_ROOM;i++) 
    if(rooms[i].style==1) {
        if(!strcmp(rooms[i].name, r->name))
            return -1;
        if(!strcmp(rooms[i].creator, currentuser->userid))
            return -1;
    }
    for(i=0;i<MAX_ROOM;i++)
    if(rooms[i].style==-1) {
        memcpy(rooms+i, r, sizeof(struct room_struct));
        inrooms[i].status = INROOM_STOP;
        inrooms[i].killernum = 0;
        inrooms[i].msgi = 0;
        inrooms[i].policenum = 0;
        inrooms[i].w = 0;
        for(j=0;j<MAX_MSG;j++)
            inrooms[i].msgs[j][0]=0;
        for(j=0;j<MAX_PEOPLE;j++)
            inrooms[i].peoples[j].style = -1;
        return 0;
    }
    return -1;
}

/*
int del_room(struct room_struct * r)
{
    int i, j;
    for(i=0;i<*roomst;i++)
    if(!strcmp(rooms[i].name, r->name)) {
        rooms[i].name[0]=0;
        break;
    }
    return 0;
}
*/

void clear_room()
{
    int i;
    for(i=0;i<MAX_ROOM;i++)
        if((rooms[i].style!=-1) && (!strcmp(rooms[i].creator, currentuser->userid)||rooms[i].people==0))
            rooms[i].style=-1;
}

int can_see(struct room_struct * r)
{
    if(r->style==-1) return 0;
    if(r->level&currentuser->userlevel!=r->level) return 0;
    if(r->style!=1) return 0;
    if(r->flag&ROOM_SECRET&&!HAS_PERM(currentuser, PERM_SYSOP)) return 0;
    return 1;
}

int can_enter(struct room_struct * r)
{
    if(r->style==-1) return 0;
    if(r->level&currentuser->userlevel!=r->level) return 0;
    if(r->style!=1) return 0;
    if(r->flag&ROOM_LOCKED&&!HAS_PERM(currentuser, PERM_SYSOP)) return 0;
    return 1;
}

int room_count()
{
    int i,j=0;
    for(i=0;i<MAX_ROOM;i++)
        if(can_see(rooms+i)) j++;
    return j;
}

int room_get(int w)
{
    int i,j=0;
    for(i=0;i<MAX_ROOM;i++) {
        if(can_see(rooms+i)) {
            if(w==j) return i;
            j++;
        }
    }
    return -1;
}

int find_room(char * s)
{
    int i;
    struct room_struct * r2;
    for(i=0;i<MAX_ROOM;i++) {
        r2 = rooms+i;
        if(!can_enter(r2)) continue;
        if(!strcmp(r2->name, s))
            return i;
    }
    return -1;
}

int selected = 0, ipage=0, jpage=0;

int getpeople(int i)
{
    int j, k=0;
    for(j=0;j<MAX_PEOPLE;j++) {
        if(inrooms[myroom].peoples[j].style==-1) continue;
        if(i==k) return j;
        k++;
    }
    return -1;
}

int get_msgt()
{
    int i,j=0,k;
    for(i=0;i<MAX_MSG;i++) {
        if(inrooms[myroom].msgs[(i+inrooms[myroom].msgi)%MAX_MSG][0]==0) break;
        k=inrooms[myroom].msgpid[(i+inrooms[myroom].msgi)%MAX_MSG];
        if(k==-1||k==uinfo.pid) j++;
    }
    return j;
}

char * get_msgs(int s)
{
    int i,j=0,k;
    for(i=0;i<MAX_MSG;i++) {
        if(inrooms[myroom].msgs[(i+inrooms[myroom].msgi)%MAX_MSG][0]==0) break;
        k=inrooms[myroom].msgpid[(i+inrooms[myroom].msgi)%MAX_MSG];
        if(k==-1||k==uinfo.pid) {
            if(j==s)
                return inrooms[myroom].msgs[(i+inrooms[myroom].msgi)%MAX_MSG];
            j++;
        }
    }
    return NULL;
}

void save_msgs(char * s)
{
    FILE* fp;
    int i;
    fp=fopen(s, "w");
    if(fp==NULL) return;
    for(i=0;i<get_msgt();i++)
        fprintf(fp, get_msgs(i));
    fclose(fp);
}

void refreshit()
{
    int i,j,me,msgst;
    for(i=0;i<t_lines-1;i++) {
        move(i, 0);
        clrtoeol();
    }
    move(0,0);
    prints("[44;33;1m ����:[36m%-12s[33m����:[36m%-40s[33m״̬:[36m%6s",
        rooms[myroom].name, rooms[myroom].title, (inrooms[myroom].status==INROOM_STOP?"δ��ʼ":(inrooms[myroom].status==INROOM_NIGHT?"��ҹ��":"�����")));
    clrtoeol();
    resetcolor();
    setfcolor(YELLOW, 1);
    move(1,0);
    prints("�q\x1b[32m���\x1b[33m�����������r�q\x1b[32mѶϢ\x1b[33m�������������������������������������������������������r");
    move(t_lines-2,0);
    prints("�t���������������s�t�����������������������������������������������������������s");
    for(i=2;i<=t_lines-3;i++) {
        move(i,0); prints("��");
        move(i,16); prints("��");
        move(i,18); prints("��");
        move(i,78); prints("��");
    }
    me=mypos;
    for(i=2;i<=t_lines-3;i++) 
    if(ipage+i-2>=0&&ipage+i-2<rooms[myroom].people) {
        j=getpeople(ipage+i-2);
        if(j==-1) continue;
        if(inrooms[myroom].status!=INROOM_STOP)
        if(inrooms[myroom].peoples[j].flag&PEOPLE_KILLER && (inrooms[myroom].peoples[me].flag&PEOPLE_KILLER ||
            inrooms[myroom].peoples[me].flag&PEOPLE_SPECTATOR ||
            !(inrooms[myroom].peoples[j].flag&PEOPLE_ALIVE))) {
            resetcolor();
            move(i,2);
            setfcolor(RED, 1);
            prints("*");
        }
        if(inrooms[myroom].status!=INROOM_STOP&&!(inrooms[myroom].peoples[j].flag&PEOPLE_ALIVE)&&
            !(inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR)) {
            resetcolor();
            move(i,3);
            setfcolor(BLUE, 1);
            prints("X");
        }
        else if((inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR)) {
            resetcolor();
            move(i,3);
            setfcolor(GREEN, 0);
            prints("O");
        }
        resetcolor();
        move(i,4);
        if(ipage+i-2==selected) {
            setfcolor(RED, 1);
        }
        if(inrooms[myroom].peoples[j].nick[0])
            prints(inrooms[myroom].peoples[j].nick);
        else
            prints(inrooms[myroom].peoples[j].id);
    }
    resetcolor();
    msgst=get_msgt();
    for(i=2;i<=t_lines-3;i++) 
    if(msgst-1-(t_lines-3-i)-jpage>=0)
    {
        char * ss=get_msgs(msgst-1-(t_lines-3-i)-jpage);
        move(i,20);
        if(ss)
            prints(ss);
    }
}

extern int RMSG;

void room_refresh(int signo)
{
    int y,x;
    signal(SIGUSR1, room_refresh);

    if(RMSG) return;
    if(rooms[myroom].style!=1) kicked = 1;
    
    getyx(&y, &x);
    refreshit();
    move(y, x);
    refresh();
}

void start_game()
{
    int i,j,totalk=0,total=0, me;
    char buf[80];
    me=mypos;
    for(i=0;i<MAX_PEOPLE;i++) 
    if(inrooms[myroom].peoples[i].style!=-1)
    {
        inrooms[myroom].peoples[i].flag &= ~PEOPLE_KILLER;
    }
    totalk=inrooms[myroom].killernum;
    for(i=0;i<MAX_PEOPLE;i++)
        if(inrooms[myroom].peoples[i].style!=-1)
        if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR)) 
            total++;
    if(total<6) {
        send_msg(me, "\x1b[31;1m����6�˲μӲ��ܿ�ʼ��Ϸ\x1b[m");
        end_change_inroom();
        refreshit();
        return;
    }
    if(totalk==0) totalk=((double)total/6+0.5);
    if(totalk>total) {
        send_msg(me, "\x1b[31;1m����������Ҫ��Ļ�������,�޷���ʼ��Ϸ\x1b[m");
        end_change_inroom();
        refreshit();
        return;
    }
    sprintf(buf, "\x1b[31;1m��Ϸ��ʼ��! ��Ⱥ�г�����%d������\x1b[m", totalk);
    send_msg(-1, buf);
    for(i=0;i<totalk;i++) {
        do{
            j=rand()%MAX_PEOPLE;
        }while(inrooms[myroom].peoples[j].style==-1 || inrooms[myroom].peoples[j].flag&PEOPLE_KILLER);
        inrooms[myroom].peoples[j].flag |= PEOPLE_KILLER;
        send_msg(j, "������һ���޳ܵĻ���");
        send_msg(j, "����ļ⵶(\x1b[31;1mCtrl+S\x1b[m)ѡ����Ҫ�к����˰�...");
    }
    for(i=0;i<MAX_PEOPLE;i++) 
    if(inrooms[myroom].peoples[i].style!=-1)
    if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR))
    {
        inrooms[myroom].peoples[i].flag |= PEOPLE_ALIVE;
        if(!(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER))
            send_msg(i, "����������...");
    }
    inrooms[myroom].status = INROOM_NIGHT;
    end_change_inroom();
    kill_msg(-1);
}

#define menust 8
int do_com_menu()
{
    char menus[menust][15]=
        {"0-����","1-�˳���Ϸ","2-������", "3-����б�", "4-�Ļ���", "5-���÷���", "6-�����", "7-��ʼ��Ϸ"};
    int menupos[menust],i,j,k,sel=0,ch,max=0,me;
    char buf[80];
    if(inrooms[myroom].status != INROOM_STOP)
        strcpy(menus[7], "7-������Ϸ");
    menupos[0]=0;
    for(i=1;i<menust;i++)
        menupos[i]=menupos[i-1]+strlen(menus[i-1])+1;
    do{
        resetcolor();
        move(t_lines-1,0);
        clrtoeol();
        j=mypos;
        for(i=0;i<menust;i++) 
        if(inrooms[myroom].peoples[j].flag&PEOPLE_ROOMOP||i<=3)
        {
            resetcolor();
            move(t_lines-1, menupos[i]);
            if(i==sel) {
                setfcolor(RED,1);
            }
            if(i>=max-1) max=i+1;
            prints(menus[i]);
        }
        ch=igetkey();
        if(kicked) return 0;
        switch(ch){
        case KEY_LEFT:
        case KEY_UP:
            sel--;
            if(sel<0) sel=max-1;
            break;
        case KEY_RIGHT:
        case KEY_DOWN:
            sel++;
            if(sel>=max) sel=0;
            break;
        case '\n':
        case '\r':
            switch(sel) {
                case 0:
                    return 0;
                case 1:
                    me=mypos;
                    if(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE&&!(inrooms[myroom].peoples[me].flag&PEOPLE_ROOMOP)&&inrooms[myroom].status!=INROOM_STOP) {
                        send_msg(me, "�㻹����Ϸ,�����˳�");
                        refreshit();
                        return 0;
                    }
                    return 1;
                case 2:
                    move(t_lines-1, 0);
                    resetcolor();
                    clrtoeol();
                    getdata(t_lines-1, 0, "����������:", buf, 13, 1, 0, 1);
                    if(kicked) return 0;
                    if(buf[0]) {
                        k=1;
                        for(j=0;j<strlen(buf);j++)
                            k=k&&(isprint2(buf[j]));
                        k=k&&(buf[0]!=' ');
                        k=k&&(buf[strlen(buf)-1]!=' ');
                        if(!k) {
                            move(t_lines-1,0);
                            resetcolor();
                            clrtoeol();
                            prints(" ���ֲ����Ϲ淶");
                            refresh();
                            sleep(1);
                            return 0;
                        }
                        me=mypos;
                        j=0;
                        for(i=0;i<MAX_PEOPLE;i++)
                            if(inrooms[myroom].peoples[i].style!=-1)
                            if(i!=me)
                            if(!strcmp(buf,inrooms[myroom].peoples[i].id) || !strcmp(buf,inrooms[myroom].peoples[i].nick)) j=1;
                        if(j) {
                            move(t_lines-1,0);
                            resetcolor();
                            clrtoeol();
                            prints(" �����������������");
                            refresh();
                            sleep(1);
                            return 0;
                        }
                        start_change_inroom();
                        me=mypos;
                        strcpy(inrooms[myroom].peoples[me].nick, buf);
                        end_change_inroom();
                        kill_msg(-1);
                    }
                    return 0;
                case 3:
                    me=mypos;
                    for(i=0;i<MAX_PEOPLE;i++) 
                    if(inrooms[myroom].peoples[i].style!=-1)
                    {
                        sprintf(buf, "%-12s  %s", inrooms[myroom].peoples[i].id, inrooms[myroom].peoples[i].nick);
                        send_msg(me, buf);
                    }
                    refreshit();
                    return 0;
                case 4:
                    move(t_lines-1, 0);
                    resetcolor();
                    clrtoeol();
                    getdata(t_lines-1, 0, "�����뻰��:", buf, 31, 1, 0, 1);
                    if(kicked) return 0;
                    if(buf[0]) {
                        start_change_inroom();
                        strcpy(rooms[myroom].title, buf);
                        end_change_inroom();
                        kill_msg(-1);
                    }
                    return 0;
                case 5:
                    move(t_lines-1, 0);
                    resetcolor();
                    clrtoeol();
                    getdata(t_lines-1, 0, "�����뷿���������:", buf, 30, 1, 0, 1);
                    if(kicked) return 0;
                    if(buf[0]) {
                        i=atoi(buf);
                        if(i>0&&i<=100)
                            rooms[myroom].maxpeople = i;
                    }
                    move(t_lines-1, 0);
                    clrtoeol();
                    getdata(t_lines-1, 0, "����Ϊ���ط���? [Y/N]", buf, 30, 1, 0, 1);
                    if(kicked) return 0;
                    buf[0]=toupper(buf[0]);
                    if(buf[0]=='Y'||buf[0]=='N') {
                        if(buf[0]=='Y') rooms[myroom].flag|=ROOM_SECRET;
                        else rooms[myroom].flag&=~ROOM_SECRET;
                    }
                    move(t_lines-1, 0);
                    clrtoeol();
                    getdata(t_lines-1, 0, "����Ϊ��������? [Y/N]", buf, 30, 1, 0, 1);
                    if(kicked) return 0;
                    buf[0]=toupper(buf[0]);
                    if(buf[0]=='Y'||buf[0]=='N') {
                        if(buf[0]=='Y') rooms[myroom].flag|=ROOM_LOCKED;
                        else rooms[myroom].flag&=~ROOM_LOCKED;
                    }
                    move(t_lines-1, 0);
                    clrtoeol();
                    getdata(t_lines-1, 0, "����Ϊ�ܾ��Թ��ߵķ���? [Y/N]", buf, 30, 1, 0, 1);
                    if(kicked) return 0;
                    buf[0]=toupper(buf[0]);
                    if(buf[0]=='Y'||buf[0]=='N') {
                        if(buf[0]=='Y') rooms[myroom].flag|=ROOM_DENYSPEC;
                        else rooms[myroom].flag&=~ROOM_DENYSPEC;
                    }
                    kill_msg(-1);
                    return 0;
                case 6:
                    move(t_lines-1, 0);
                    resetcolor();
                    clrtoeol();
                    getdata(t_lines-1, 0, "������Ҫ�ߵ�id:", buf, 30, 1, 0, 1);
                    if(kicked) return 0;
                    if(buf[0]) {
                        for(i=0;i<MAX_PEOPLE;i++)
                            if(inrooms[myroom].peoples[i].style!=-1)
                            if(!strcmp(inrooms[myroom].peoples[i].id, buf)) break;
                        if(!strcmp(inrooms[myroom].peoples[i].id, buf) && inrooms[myroom].peoples[i].pid!=uinfo.pid) {
                            send_msg(i, "�㱻����");
                            kill_msg(i);
                            return 2;
                        }
                    }
                    return 0;
                case 7:
                    start_change_inroom();
                    if(inrooms[myroom].status == INROOM_STOP)
                        start_game();
                    else {
                        inrooms[myroom].status = INROOM_STOP;
                        send_msg(-1, "��Ϸ������ǿ�ƽ���");
                        end_change_inroom();
                        kill_msg(-1);
                    }
                    return 0;
            }
            break;
        default:
            for(i=0;i<max;i++)
                if(ch==menus[i][0]) sel=i;
            break;
        }
    }while(1);
}

void join_room(int w, int spec)
{
    char buf[80],buf2[80],buf3[80],roomname[80];
    int i,j,killer,me;
    clear();
    myroom = w;
    start_change_inroom();
    if(rooms[myroom].style!=1) {
        end_change_inroom();
        return;
    }
    strcpy(roomname, rooms[myroom].name);
    signal(SIGUSR1, room_refresh);
    i=0;
    while(inrooms[myroom].peoples[i].style!=-1) i++;
    mypos = i;
    inrooms[myroom].peoples[i].style = 0;
    inrooms[myroom].peoples[i].flag = 0;
    strcpy(inrooms[myroom].peoples[i].id, currentuser->userid);
    inrooms[myroom].peoples[i].nick[0]=0;
    inrooms[myroom].peoples[i].pid = uinfo.pid;
    if(rooms[myroom].people==0 && !strcmp(rooms[myroom].creator, currentuser->userid))
        inrooms[myroom].peoples[i].flag = PEOPLE_ROOMOP;
    if(spec) inrooms[myroom].peoples[i].flag|=PEOPLE_SPECTATOR;
    rooms[myroom].people++;
    end_change_inroom();

    kill_msg(-1);
/*    sprintf(buf, "%s���뷿��", currentuser->userid);
    for(i=0;i<myroom->people;i++) {
        send_msg(inrooms.peoples+i, buf);
        kill(inrooms.peoples[i].pid, SIGUSR1);
    }*/

    room_refresh(0);
    while(1){
        do{
            int ch;
            ch=-getdata(t_lines-1, 0, "����:", buf, 70, 1, NULL, 1);
            if(rooms[myroom].style!=1) kicked = 1;
            if(kicked) goto quitgame;
            if(ch==KEY_UP) {
                selected--;
                if(selected<0) selected = rooms[myroom].people-1;
                if(ipage>selected) ipage=selected;
                if(selected>ipage+t_lines-5) ipage=selected-(t_lines-5);
                refreshit();
            }
            else if(ch==KEY_DOWN) {
                selected++;
                if(selected>=rooms[myroom].people) selected=0;
                if(ipage>selected) ipage=selected;
                if(selected>ipage+t_lines-5) ipage=selected-(t_lines-5);
                refreshit();
            }
            else if(ch==KEY_PGUP) {
                jpage+=t_lines/2;
                refreshit();
            }
            else if(ch==KEY_PGDN) {
                jpage-=t_lines/2;
                if(jpage<=0) jpage=0;
                refreshit();
            }
            else if(ch==Ctrl('S')) {
                int pid;
                int sel;
                sel = getpeople(selected);
                me=mypos;
                pid=inrooms[myroom].peoples[sel].pid;
                if(inrooms[myroom].peoples[me].vote==0)
                if(inrooms[myroom].peoples[me].flag&PEOPLE_ALIVE&&
                    (inrooms[myroom].peoples[me].flag&PEOPLE_KILLER&&inrooms[myroom].status==INROOM_NIGHT ||
                    inrooms[myroom].status==INROOM_DAY)) {
                    if(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE && 
                        !(inrooms[myroom].peoples[sel].flag&PEOPLE_SPECTATOR) &&
                        sel!=me) {
                        int i,j,t1,t2,t3;
                        sprintf(buf, "\x1b[32;1m%sͶ��%sһƱ\x1b[m", inrooms[myroom].peoples[me].nick[0]?inrooms[myroom].peoples[me].nick:inrooms[myroom].peoples[me].id,
                            inrooms[myroom].peoples[sel].nick[0]?inrooms[myroom].peoples[sel].nick:inrooms[myroom].peoples[sel].id);
                        start_change_inroom();
                        inrooms[myroom].peoples[me].vote = pid;
                        end_change_inroom();
                        if(inrooms[myroom].status==INROOM_NIGHT) {
                            for(i=0;i<MAX_PEOPLE;i++)
                                if(inrooms[myroom].peoples[i].style!=-1)
                                if(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER||
                                    inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR)
                                    send_msg(i, buf);
                        }
                        else {
                            send_msg(-1, buf);
                        }
checkvote:
                        t1=0; t2=0; t3=0;
                        for(i=0;i<MAX_PEOPLE;i++)
                            inrooms[myroom].peoples[i].vnum = 0;
                        for(i=0;i<MAX_PEOPLE;i++)
                        if(inrooms[myroom].peoples[i].style!=-1)
                        if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) &&
                            inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE &&
                            (inrooms[myroom].peoples[i].flag&PEOPLE_KILLER||inrooms[myroom].status==INROOM_DAY)) {
                            for(j=0;j<MAX_PEOPLE;j++)
                                if(inrooms[myroom].peoples[j].style!=-1)
                                if(inrooms[myroom].peoples[j].pid == inrooms[myroom].peoples[i].vote)
                                    inrooms[myroom].peoples[j].vnum++;
                        }
                        for(i=0;i<MAX_PEOPLE;i++)
                        if(inrooms[myroom].peoples[i].style!=-1)
                        if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) &&
                            inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE) {
                            if(inrooms[myroom].peoples[i].vnum>=t1) {
                                t2=t1; t1=inrooms[myroom].peoples[i].vnum;
                            }
                            else if(inrooms[myroom].peoples[i].vnum>=t2) {
                                t2=inrooms[myroom].peoples[i].vnum;
                            }
                        }
                        j=1;
                        for(i=0;i<MAX_PEOPLE;i++)
                        if(inrooms[myroom].peoples[i].style!=-1)
                        if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) &&
                            inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE &&
                            (inrooms[myroom].peoples[i].flag&PEOPLE_KILLER||inrooms[myroom].status==INROOM_DAY))
                            if(inrooms[myroom].peoples[i].vote == 0) {
                                j=0;
                                t3++;
                            }
                        if(j || t1-t2>t3) {
                            int max=0, ok=0, maxi, maxpid;
                            for(i=0;i<MAX_PEOPLE;i++)
                                inrooms[myroom].peoples[i].vnum = 0;
                            for(i=0;i<MAX_PEOPLE;i++)
                            if(inrooms[myroom].peoples[i].style!=-1)
                            if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) &&
                                inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE &&
                                (inrooms[myroom].peoples[i].flag&PEOPLE_KILLER||inrooms[myroom].status==INROOM_DAY)) {
                                for(j=0;j<MAX_PEOPLE;j++)
                                if(inrooms[myroom].peoples[j].style!=-1)
                                    if(inrooms[myroom].peoples[j].pid == inrooms[myroom].peoples[i].vote)
                                        inrooms[myroom].peoples[j].vnum++;
                            }
                            sprintf(buf, "ͶƱ���:");
                            if(inrooms[myroom].status==INROOM_NIGHT) {
                                for(j=0;j<MAX_PEOPLE;j++)
                                    if(inrooms[myroom].peoples[j].style!=-1)
                                    if(inrooms[myroom].peoples[j].flag&PEOPLE_KILLER||
                                        inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR)
                                        send_msg(j, buf);
                            }
                            else {
                                send_msg(-1, buf);
                            }
                            for(i=0;i<MAX_PEOPLE;i++)
                            if(inrooms[myroom].peoples[i].style!=-1)
                            if(!(inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) &&
                                inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE) {
                                sprintf(buf, "%s��ͶƱ��: %d Ʊ", 
                                    inrooms[myroom].peoples[i].nick[0]?inrooms[myroom].peoples[i].nick:inrooms[myroom].peoples[i].id,
                                    inrooms[myroom].peoples[i].vnum);
                                if(inrooms[myroom].peoples[i].vnum==max)
                                    ok=0;
                                if(inrooms[myroom].peoples[i].vnum>max) {
                                    max=inrooms[myroom].peoples[i].vnum;
                                    ok=1;
                                    maxi=i;
                                    maxpid=inrooms[myroom].peoples[i].pid;
                                }
                                if(inrooms[myroom].status==INROOM_NIGHT) {
                                    for(j=0;j<MAX_PEOPLE;j++)
                                        if(inrooms[myroom].peoples[j].style!=-1)
                                        if(inrooms[myroom].peoples[j].flag&PEOPLE_KILLER||
                                            inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR)
                                            send_msg(j, buf);
                                }
                                else {
                                    send_msg(-1, buf);
                                }
                            }
                            if(!ok) {
                                sprintf(buf, "���Ʊ����ͬ,������Э�̽��...");
                                if(inrooms[myroom].status==INROOM_NIGHT) {
                                    for(j=0;j<MAX_PEOPLE;j++)
                                        if(inrooms[myroom].peoples[j].style!=-1)
                                        if(inrooms[myroom].peoples[j].flag&PEOPLE_KILLER||
                                            inrooms[myroom].peoples[j].flag&PEOPLE_SPECTATOR)
                                            send_msg(j, buf);
                                }
                                else
                                    send_msg(-1, buf);
                                start_change_inroom();
                                for(j=0;j<MAX_PEOPLE;j++)
                                    inrooms[myroom].peoples[j].vote=0;
                                end_change_inroom();
                            }
                            else {
                                int a=0,b=0;
                                if(inrooms[myroom].status == INROOM_DAY)
                                    sprintf(buf, "�㱻��Ҵ�����!");
                                else
                                    sprintf(buf, "�㱻����ɱ����!");
                                send_msg(maxi, buf);
                                if(inrooms[myroom].status == INROOM_DAY) {
                                    if(inrooms[myroom].peoples[maxi].flag&PEOPLE_KILLER)
                                        sprintf(buf, "����%s��������!",
                                            inrooms[myroom].peoples[maxi].nick[0]?inrooms[myroom].peoples[maxi].nick:inrooms[myroom].peoples[maxi].id);
                                    else
                                        sprintf(buf, "����%s��������!",
                                            inrooms[myroom].peoples[maxi].nick[0]?inrooms[myroom].peoples[maxi].nick:inrooms[myroom].peoples[maxi].id);
                                }
                                else
                                    sprintf(buf, "%s��ɱ����!",
                                        inrooms[myroom].peoples[maxi].nick[0]?inrooms[myroom].peoples[maxi].nick:inrooms[myroom].peoples[maxi].id);
                                for(j=0;j<MAX_PEOPLE;j++)
                                    if(inrooms[myroom].peoples[j].style!=-1)
                                    if(j!=maxi)
                                        send_msg(j, buf);
                                start_change_inroom();
                                for(i=0;i<MAX_PEOPLE;i++)
                                    if(inrooms[myroom].peoples[i].style!=-1)
                                    if(inrooms[myroom].peoples[i].pid == maxpid)
                                        inrooms[myroom].peoples[i].flag &= ~PEOPLE_ALIVE;
                                for(i=0;i<MAX_PEOPLE;i++)
                                    if(inrooms[myroom].peoples[i].style!=-1)
                                    if(inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE) {
                                        if(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER) a++;
                                        else b++;
                                    }
                                if(a>0&&a>=b-1&&inrooms[myroom].status==INROOM_DAY) {
                                    inrooms[myroom].status = INROOM_STOP;
                                    send_msg(-1, "���˻����ʤ��...");
                                    for(j=0;j<MAX_PEOPLE;j++)
                                    if(inrooms[myroom].peoples[j].style!=-1)
                                    if(inrooms[myroom].peoples[j].flag&PEOPLE_KILLER &&
                                        inrooms[myroom].peoples[j].flag&PEOPLE_ALIVE) {
                                        sprintf(buf, "ԭ��%s�ǻ���!",
                                            inrooms[myroom].peoples[j].nick[0]?inrooms[myroom].peoples[j].nick:inrooms[myroom].peoples[j].id);
                                        send_msg(-1, buf);
                                    }
                                }
                                else if(a==0) {
                                    inrooms[myroom].status = INROOM_STOP;
                                    send_msg(-1, "���л��˶��������ˣ����˻����ʤ��...");
                                }
                                else if(inrooms[myroom].status == INROOM_DAY) {
                                    inrooms[myroom].status = INROOM_NIGHT;
                                    send_msg(-1, "�ֲ���ҹɫ�ֽ�����...");
                                    for(i=0;i<MAX_PEOPLE;i++) 
                                        if(inrooms[myroom].peoples[i].style!=-1)
                                        if(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER&&inrooms[myroom].peoples[i].flag&PEOPLE_ALIVE)
                                            send_msg(i, "��ץ����ı���ʱ����\x1b[31;1mCtrl+S\x1b[mɱ��...");
                                }
                                else {
                                    inrooms[myroom].status = INROOM_DAY;
                                    send_msg(-1, "������...");
                                }
                                for(i=0;i<MAX_PEOPLE;i++)
                                    inrooms[myroom].peoples[i].vote = 0;
                                end_change_inroom();
                            }
                        }
                        kill_msg(-1);
                    }
                    else {
                        if(sel==me)
                            send_msg(me, "\x1b[31;1m�㲻��ѡ����ɱ\x1b[m");
                        else if(!(inrooms[myroom].peoples[sel].flag&PEOPLE_ALIVE))
                            send_msg(me, "\x1b[31;1m��������\x1b[m");
                        else
                            send_msg(me, "\x1b[31;1m�������Թ���\x1b[m");
                        refreshit();
                    }
                }
            }
            else if(ch<=0&&!buf[0]) {
                j=do_com_menu();
                if(kicked) goto quitgame;
                if(j==1) goto quitgame;
                if(j==2) goto checkvote;
            }
            else if(ch<=0){
                break;
            }
        }while(1);
        start_change_inroom();
        me=mypos;
        strcpy(buf2, buf);
        sprintf(buf, "%s: %s", 
            inrooms[myroom].peoples[me].nick[0]?inrooms[myroom].peoples[me].nick:inrooms[myroom].peoples[me].id, 
            buf2);
        if(inrooms[myroom].status==INROOM_NIGHT) {
            if(inrooms[myroom].peoples[me].flag&PEOPLE_KILLER)
            for(i=0;i<MAX_PEOPLE;i++) 
            if(inrooms[myroom].peoples[i].style!=-1)
            {
                if(inrooms[myroom].peoples[i].flag&PEOPLE_KILLER||
                    inrooms[myroom].peoples[i].flag&PEOPLE_SPECTATOR) {
                    send_msg(i, buf);
                }
            }
        }
        else {
            if(!(inrooms[myroom].peoples[me].flag&PEOPLE_SPECTATOR))
            send_msg(-1, buf);
        }
        end_change_inroom();
        kill_msg(-1);
    }

quitgame:
    start_change_inroom();
    me=mypos;
    if(inrooms[myroom].peoples[me].flag&PEOPLE_ROOMOP) {
        for(i=0;i<MAX_PEOPLE;i++)
        if(inrooms[myroom].peoples[i].style!=-1)
        if(i!=me) {
            send_msg(i, "�㱻����");
        }
        rooms[myroom].style = -1;
        end_change_inroom();
        for(i=0;i<MAX_PEOPLE;i++)
            if(inrooms[myroom].peoples[i].style!=-1)
            if(i!=me)
                kill_msg(i);
        goto quitgame2;
    }
    inrooms[myroom].peoples[me].style=-1;
    rooms[myroom].people--;
    end_change_inroom();

/*    if(killer)
        sprintf(buf, "ɱ��%sǱ����", buf2);
    else
        sprintf(buf, "%s�뿪����", buf2);
    for(i=0;i<myroom->people;i++) {
        send_msg(inrooms.peoples+i, buf);
        kill(inrooms.peoples[i].pid, SIGUSR1);
    }*/
quitgame2:
    kicked=0;
    getdata(t_lines-1, 0, "�Ļر���ȫ����Ϣ��?[y/N]", buf3, 3, 1, 0, 1);
    if(toupper(buf3[0])=='Y') {
        sprintf(buf, "tmp/%d.msg", rand());
        save_msgs(buf);
        sprintf(buf2, "\"%s\"��ɱ�˼�¼", roomname);
        mail_file(currentuser->userid, buf, currentuser->userid, buf2, BBSPOST_MOVE, NULL);
    }
    signal(SIGUSR1, talk_request);
}

static int room_list_refresh(struct _select_def *conf)
{
    clear();
    docmdtitle("[��Ϸ��ѡ��]",
              "  �˳�[\x1b[1;32m��\x1b[0;37m,\x1b[1;32me\x1b[0;37m] ����[\x1b[1;32mEnter\x1b[0;37m] ѡ��[\x1b[1;32m��\x1b[0;37m,\x1b[1;32m��\x1b[0;37m] ���[\x1b[1;32ma\x1b[0;37m] ����[\x1b[1;32mJ\x1b[0;37m] \x1b[m");
    move(2, 0);
    prints("[0;1;37;44m    %4s %-14s %-12s %4s %4s %4s %4s %-20s", "���", "��Ϸ������", "������", "����", "����", "���", "����", "����");
    clrtoeol();
    resetcolor();
    update_endline();
    return SHOW_CONTINUE;
}

static int room_list_show(struct _select_def *conf, int i)
{
    struct room_struct * r;
    int j = room_get(i-1);
    if(j!=-1) {
        r=rooms+j;
        prints("  %3d  %-14s %-12s %4s %3d  %3d   %2s  %-20s", i, r->name, r->creator, "ɱ��", r->people, r->maxpeople, (r->flag&ROOM_LOCKED)?"��":"��", r->title);
    }
    return SHOW_CONTINUE;
}

static int room_list_select(struct _select_def *conf)
{
    struct room_struct * r, * r2;
    int i=room_get(conf->pos-1), j;
    char ans[4];
    if(i==-1) return SHOW_CONTINUE;
    r = rooms+i;
    j=find_room(r->name);
    if(j==-1) {
        move(0, 0);
        clrtoeol();
        prints(" �÷����ѱ�����!");
        refresh(); sleep(1);
        return SHOW_REFRESH;
    }
    r2 = rooms+j;
    if(r2->people>=r2->maxpeople&&!HAS_PERM(currentuser, PERM_SYSOP)) {
        move(0, 0);
        clrtoeol();
        prints(" �÷�����������");
        refresh(); sleep(1);
        return SHOW_REFRESH;
    }
    getdata(0, 0, "�Ƿ����Թ�����ݽ���? [y/N]", ans, 3, 1, NULL, 1);
    if(toupper(ans[0])=='Y'&&r2->flag&ROOM_DENYSPEC) {
        move(0, 0);
        clrtoeol();
        prints(" �÷���ܾ��Թ���");
        refresh(); sleep(1);
        return SHOW_REFRESH;
    }
    join_room(find_room(r2->name), toupper(ans[0])=='Y');
    return SHOW_DIRCHANGE;
}

static int room_list_getdata(struct _select_def *conf, int pos, int len)
{
    clear_room();
    conf->item_count = room_count();
    return SHOW_CONTINUE;
}

static int room_list_prekey(struct _select_def *conf, int *key)
{
    switch (*key) {
    case 'e':
    case 'q':
        *key = KEY_LEFT;
        break;
    case 'p':
    case 'k':
        *key = KEY_UP;
        break;
    case ' ':
    case 'N':
        *key = KEY_PGDN;
        break;
    case 'n':
    case 'j':
        *key = KEY_DOWN;
        break;
    }
    return SHOW_CONTINUE;
}

static int room_list_key(struct _select_def *conf, int key)
{
    struct room_struct r, *r2;
    int i,j;
    char name[40], ans[4];
    switch(key) {
    case 'a':
        strcpy(r.creator, currentuser->userid);
        getdata(0, 0, "������:", name, 13, 1, NULL, 1);
        if(!name[0]) return SHOW_REFRESH;
        if(name[0]==' '||name[strlen(name)-1]==' ') {
            move(0, 0);
            clrtoeol();
            prints(" ��������ͷ��β����Ϊ�ո�");
            refresh(); sleep(1);
            return SHOW_CONTINUE;
        }
        strcpy(r.name, name);
        r.style = 1;
        r.flag = 0;
        r.people = 0;
        r.maxpeople = 100;
        strcpy(r.title, "��ɱ��ɱ��ɱɱɱ");
        if(add_room(&r)==-1) {
            move(0, 0);
            clrtoeol();
            prints(" ��һ�����ֵķ�����!");
            refresh(); sleep(1);
            return SHOW_REFRESH;
        }
        join_room(find_room(r.name), 0);
        return SHOW_DIRCHANGE;
    case 'J':
        getdata(0, 0, "������:", name, 12, 1, NULL, 1);
        if(!name[0]) return SHOW_REFRESH;
        if((i=find_room(name))==-1) {
            move(0, 0);
            clrtoeol();
            prints(" û���ҵ��÷���!");
            refresh(); sleep(1);
            return SHOW_REFRESH;
        }
        r2 = rooms+i;
        if(r2->people>=r2->maxpeople&&!HAS_PERM(currentuser, PERM_SYSOP)) {
            move(0, 0);
            clrtoeol();
            prints(" �÷�����������");
            refresh(); sleep(1);
            return SHOW_REFRESH;
        }
        getdata(0, 0, "�Ƿ����Թ�����ݽ���? [y/N]", ans, 3, 1, NULL, 1);
        if(toupper(ans[0])=='Y'&&r2->flag&ROOM_DENYSPEC) {
            move(0, 0);
            clrtoeol();
            prints(" �÷���ܾ��Թ���");
            refresh(); sleep(1);
            return SHOW_REFRESH;
        }
        join_room(find_room(name), toupper(ans[0])=='Y');
        return SHOW_DIRCHANGE;
    case 'K':
        if(!HAS_PERM(currentuser, PERM_SYSOP)) return SHOW_CONTINUE;
        i = room_get(conf->pos-1);
        if(i!=-1) {
            r2 = rooms+i;
            r2->style = -1;
            for(j=0;j<MAX_PEOPLE;j++)
            if(inrooms[i].peoples[j].style!=-1)
                kill(inrooms[i].peoples[j].pid, SIGUSR1);
        }
        return SHOW_DIRCHANGE;
    }
    return SHOW_CONTINUE;
}

int choose_room()
{
    struct _select_def grouplist_conf;
    int i;
    POINT *pts;

    clear_room();
    bzero(&grouplist_conf, sizeof(struct _select_def));
    grouplist_conf.item_count = room_count();
    if (grouplist_conf.item_count == 0) {
        grouplist_conf.item_count = 1;
    }
    pts = (POINT *) malloc(sizeof(POINT) * BBS_PAGESIZE);
    for (i = 0; i < BBS_PAGESIZE; i++) {
        pts[i].x = 2;
        pts[i].y = i + 3;
    }
    grouplist_conf.item_per_page = BBS_PAGESIZE;
    grouplist_conf.flag = LF_VSCROLL | LF_BELL | LF_LOOP | LF_MULTIPAGE;
    grouplist_conf.prompt = "��";
    grouplist_conf.item_pos = pts;
    grouplist_conf.arg = NULL;
    grouplist_conf.title_pos.x = 0;
    grouplist_conf.title_pos.y = 0;
    grouplist_conf.pos = 1;     /* initialize cursor on the first mailgroup */
    grouplist_conf.page_pos = 1;        /* initialize page to the first one */

    grouplist_conf.on_select = room_list_select;
    grouplist_conf.show_data = room_list_show;
    grouplist_conf.pre_key_command = room_list_prekey;
    grouplist_conf.key_command = room_list_key;
    grouplist_conf.show_title = room_list_refresh;
    grouplist_conf.get_data = room_list_getdata;
    list_select_loop(&grouplist_conf);
    free(pts);
}

int killer_main()
{
    int i,oldmode;
    void * shm, * shm2;
    shm=attach_shm("GAMEROOM_SHMKEY", 3451, sizeof(struct room_struct)*MAX_ROOM, &i);
    rooms = shm;
    if(i) {
        for(i=0;i<MAX_ROOM;i++) {
            rooms[i].style=-1;
            rooms[i].w = 0;
        }
    }
    shm2=attach_shm("KILLER_SHMKEY", 9578, sizeof(struct inroom_struct)*MAX_ROOM, &i);
    inrooms = shm2;
    if(i) {
        for(i=0;i<MAX_ROOM;i++)
            inrooms[i].w = 0;
    }
    oldmode = uinfo.mode;
    modify_user_mode(KILLER);
    choose_room();
    modify_user_mode(oldmode);
    shmdt(shm);
    shmdt(shm2);
}

