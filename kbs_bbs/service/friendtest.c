#include "bbs.h"

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PINK 5
#define CYAN 6
#define WHITE 7

#define TOTALNUM 20
#define FRIENDTOP ".FRIENDTOP"

struct usertype {
    char userid[IDLEN+2];
    unsigned int tried;
    unsigned int got;
    time_t create;
} * users;
int userst=0;

struct questype {
    char topic[80];
    char quest[5][40];
    int value[5];
} * quests;
int questst=0;

struct userec* lookuser;

void sethome(char* s, char* id)
{
    sprintf(s, "home/%c/%s/.FRIENDTEST", toupper(id[0]), id);
}

void sethometop(char* s, char* id)
{
    sprintf(s, "home/%c/%s/" FRIENDTOP, toupper(id[0]), id);
}

void sethomestat(char* s, char* id)
{
    sprintf(s, "home/%c/%s/.FRIENDSTAT", toupper(id[0]), id);
}

void setfcolor(int i,int j)
{
    prints("\033[%d;%dm",i+30,j);
}

void setbcolor(int i)
{
    prints("\033[%d;%dm",i+40);
}

void resetcolor()
{
    prints("\033[27;40;0m");
}

void load_users(char* filename)
{
    int fd;
    struct flock ldata;
    if(userst) free(users);
    userst=0;
    if((fd = open(filename, O_RDONLY, 0644))!=-1) {
        ldata.l_type=F_RDLCK;
        ldata.l_whence=0;
        ldata.l_len=0;
        ldata.l_start=0;
        if(fcntl(fd, F_SETLKW, &ldata)!=-1){
            read(fd, &userst, sizeof(userst));
            users=(struct usertype*)malloc(sizeof(struct usertype)*(userst+1));
            read(fd, users, sizeof(struct usertype)*userst);
            	
            ldata.l_type = F_UNLCK;
            fcntl(fd, F_SETLKW, &ldata);
        }
        close(fd);
    }
}

void save_users(char* filename)
{
    int fd;
    struct flock ldata;
    if((fd = open(filename, O_WRONLY|O_CREAT, 0644))!=-1) {
        ldata.l_type=F_WRLCK;
        ldata.l_whence=0;
        ldata.l_len=0;
        ldata.l_start=0;
        if(fcntl(fd, F_SETLKW, &ldata)!=-1){
            write(fd, &userst, sizeof(userst));
            write(fd, users, sizeof(struct usertype)*userst);
            	
            ldata.l_type = F_UNLCK;
            fcntl(fd, F_SETLKW, &ldata);
        }
        close(fd);
    }
}

void load_quests(char* filename)
{
    int fd;
    struct flock ldata;
    if(questst) free(quests);
    questst=0;
    if((fd = open(filename, O_RDONLY, 0644))!=-1) {
        ldata.l_type=F_RDLCK;
        ldata.l_whence=0;
        ldata.l_len=0;
        ldata.l_start=0;
        if(fcntl(fd, F_SETLKW, &ldata)!=-1){
            read(fd, &questst, sizeof(questst));
            quests=(struct questype*)malloc(sizeof(struct questype)*questst);
            read(fd, quests, sizeof(struct questype)*questst);
            	
            ldata.l_type = F_UNLCK;
            fcntl(fd, F_SETLKW, &ldata);
        }
        close(fd);
    }
}

void save_quests(char* filename)
{
    int fd;
    struct flock ldata;
    if((fd = open(filename, O_WRONLY|O_CREAT, 0644))!=-1) {
        ldata.l_type=F_WRLCK;
        ldata.l_whence=0;
        ldata.l_len=0;
        ldata.l_start=0;
        if(fcntl(fd, F_SETLKW, &ldata)!=-1){
            write(fd, &questst, sizeof(questst));
            write(fd, quests, sizeof(struct questype)*questst);
            	
            ldata.l_type = F_UNLCK;
            fcntl(fd, F_SETLKW, &ldata);
        }
        close(fd);
    }
}

void save_stats(char* filename, char a[20])
{
    int fd;
    struct flock ldata;
    if((fd = open(filename, O_WRONLY|O_CREAT, 0644))!=-1) {
        ldata.l_type=F_WRLCK;
        ldata.l_whence=0;
        ldata.l_len=0;
        ldata.l_start=0;
        if(fcntl(fd, F_SETLKW, &ldata)!=-1){
            lseek(fd, 0, SEEK_END);
            write(fd, a, 20);
            	
            ldata.l_type = F_UNLCK;
            fcntl(fd, F_SETLKW, &ldata);
        }
        close(fd);
    }
}

void sort_users()
{
    int i,j;
    struct usertype tmp;
    for(i=0;i<userst;i++)
        for(j=i+1;j<userst;j++)
        if(users[i].tried<users[j].tried) {
            memcpy(&tmp,&users[i],sizeof(tmp));
            memcpy(&users[i],&users[j],sizeof(tmp));
            memcpy(&users[j],&tmp,sizeof(tmp));
        }
}

void init()
{
}

void done()
{
    if(userst) free(users);
    if(questst) free(quests);
}

int usermenu()
{
    int i,j,tuid;
    char buf[IDLEN+2], direct[PATHLEN];
    struct stat st;
    clear();
    load_users(FRIENDTOP);
    sort_users();
    move(2,20);
    setfcolor(WHITE,0);
    prints("��ӭ���������������(");
    setfcolor(RED,1);
    prints("FRIENDTEST");
    setfcolor(WHITE,0);
    prints(")���԰�0.1");
    move(3,40);
    prints("����: ");
    setfcolor(RED,0);
    prints("bad@smth.org");
    move(5,28);
    setfcolor(WHITE,1);
    prints("===�û����Դ�������===");
    move(6,30);
    setfcolor(RED,1);
    prints("�q���������������r");
    for(i=0;i<10;i++){
        move(7+i, 30);
        setfcolor(RED,1);
        prints("��");
        if (i>=userst) {
            move(7+i, 35+7);
            setfcolor(GREEN,1);
            prints("--��--");
        }
        else {
            move(7+i, 38-strlen(users[i].userid)/2+7);
            setfcolor(GREEN,1);
            prints(users[i].userid);
        }
        move(7+i, 46+14);
        setfcolor(RED,1);
        prints("��");
    }
    move(17,30);
    setfcolor(RED,1);
    prints("�t���������������s");
    setfcolor(WHITE,0);
    move(19,22);
    prints("Ҫ����������Լ�����,������a<�س�>");
    move(20,22);
    prints("Ҫ�����˵���,����������id");
//    setfcolor(WHITE,1);
    while(1){
        move(22,20); clrtoeol();
        getdata(22,26,"��ѡ��:",buf,IDLEN+1,1,NULL,true);
        if(toupper(buf[0])=='A'&&!buf[1])
              return 1;
        else if(buf[0]){
            tuid = getuser(buf, &lookuser);
            if(tuid) {
                sethome(direct, lookuser->userid);
                if(stat(direct, &st) != -1) 
                    return 2;
                else {
                    move(23,20);
                    clrtoeol();
                    prints("ʹ����%s��δ������Ŀ!", lookuser->userid);
                }
            }
            else {
                move(23,20);
                clrtoeol();
                prints("�����ʹ����id!");
            }
        }
        else return 0;
    }
}

int possible_high()
{
    int i,j,k=0,l;
    for(i=0;i<questst;i++){
        l=-10;
        for(j=0;j<5;j++){
            if(!quests[i].quest[j][0]) break;
            if(quests[i].value[j]>l)
                l=quests[i].value[j];
        }
        k+=l;
    }
    return k;
}

void new_friendtest_reset()
{
    resetcolor();
    clear();
    move(0,0);
    setfcolor(YELLOW,1);
    setbcolor(BLUE);
    prints("   FRIENDTEST      �½��û� %s                                                  ", currentuser->userid);
    resetcolor();
    prints("\n");
}

void new_friendtest()
{
    char direct[PATHLEN];
    int i, j, k;
    struct stat st;
    char ans[3], buf[122], buf2[10];
    int empty=1;
    new_friendtest_reset();
    
    sethome(direct, currentuser->userid);
    if(stat(direct, &st) != -1) {
        empty=0;
        getdata(1,0,"�Ѿ���������˺ţ��Ƿ����? [y/N] ", ans, 2, 1, NULL, true);
        if(toupper(ans[0])!='Y') return;
    }
    getdata(1,0,"����������[1--20]", buf, 3, true, NULL, true);
    questst=atoi(buf);
    if(questst<1||questst>20) {
        questst=0;
        return;
    }
    quests=(struct questype*)malloc(sizeof(struct questype)*questst);
    for(i=0;i<questst;i++){
again:
        new_friendtest_reset();
        prints("��%d��(��%d��)\n", i+1, questst);
        getdata(2,0,"��������Ŀ(40��������): ", quests[i].topic, 80, true, NULL, true);
        if (!quests[i].topic[0]) {
            new_friendtest_reset();
            getdata(1,0,"�˳������˺�? [Y-�˳�/N-����/E-������������]: ", ans, 2, true, NULL, true);
            if(toupper(ans[0])=='Y') return;
            if(toupper(ans[0])=='E'&&i!=0) {
                questst=i;
                break;
            }
            goto again;
        }
        for(j=0;j<5;j++) quests[i].quest[j][0]=0;
        do{
            move(3,0); clrtoeol();
            getdata(3,0,"������ѡ�����[1--5]: ", buf, 2, true, NULL, true);
            j=atoi(buf);
        }while(j<1||j>5);
        for(k=0;k<j;k++) {
            do{
                move(4+k*2,0); clrtoeol();
                sprintf(buf, "�������%d��ѡ������:", k+1);
                getdata(4+k*2,0,buf,quests[i].quest[k],40,true,NULL,true);
            } while(!quests[i].quest[k][0]);
            do{
                move(5+k*2,0); clrtoeol();
                sprintf(buf, "�������%d��ѡ���ֵ[-10--10]:", k+1);
                getdata(5+k*2,0,buf,buf2,4,true,NULL,true);
                quests[i].value[k]=atoi(buf2);
            } while(quests[i].value[k]<-10||quests[i].value[k]>10);
        }
    }
    new_friendtest_reset();
    move(1,0);
    prints("��Ĳ��Թ���%d���Ĳ��Կ��ܵõ�����߷���%d", questst, possible_high());
    getdata(2,0,"���������Ƿ���ȷ��ѡ���ȡ����ѡ���ǽ���[Y/n]", ans, 2, true, NULL, true);
    if(toupper(ans[0])=='N') return;
    save_quests(direct);
    load_users(FRIENDTOP);
    j=1;
    for(i=0;i<userst;i++)
    if(!strcmp(users[i].userid, currentuser->userid)) {
        users[i].tried = 0;
        users[i].got = 0;
        users[i].create = time(0);
        j=0;
        break;
    }
    if(j) {
        if(!userst)
            users=(struct usertype*)malloc(sizeof(struct usertype)*(userst+1));
        strcpy(users[userst].userid, currentuser->userid);
        users[userst].tried = 0;
        users[userst].got = 0;
        users[userst].create = time(0);
        userst++;
    }
    save_users(FRIENDTOP);
    sethometop(direct, currentuser->userid);
    unlink(direct);
    sethomestat(direct, lookuser->userid);
    unlink(direct);
    move(3,0);
    prints("��ϲ�㣬���Ѿ��������˺ţ��Ͽ�������ĺ������μӰ�");
    pressreturn();
}

void do_test_reset()
{
    resetcolor();
    clear();
    move(0,0);
    setfcolor(YELLOW,1);
    setbcolor(BLUE);
    prints("   FRIENDTEST      ���꾺�� %s <--> %s                                           ", currentuser->userid, lookuser->userid);
    resetcolor();
    prints("\n");
}

void do_test()
{
    char direct[PATHLEN];
    struct stat st;
    int i, j, k, l;
    char ans[3], buf[122], buf2[10], answer[20];
    time_t span;

    do_test_reset();
    
    sethome(direct, lookuser->userid);
    if(stat(direct, &st) == -1) {
        move(1,0);
        prints("���˺Ų�����!");
        pressreturn();
        return;
    }
    load_quests(direct);
    sethometop(direct, lookuser->userid);
    load_users(direct);
    sort_users();
    for(i=0;i<userst;i++)
    if(!strcmp(users[i].userid, currentuser->userid)){
        span = time(0)-users[i].create;
        if (span<3600) {
            move(2,0);
            prints("����һСʱǰ������ %s ���������\n", lookuser->userid);
            prints("����΢��Ϣһ����ٽ�����\n");
            pressreturn();
            return;
        }
    }
    move(2,0);
    prints("��ӭ���� %s ���������\n", lookuser->userid);
    prints("��������߿��ܵ÷�Ϊ%d\n", possible_high());
    if(userst) {
        prints("�Ѿ��� %d �˲μӹ�����\n", userst);
        prints("������߷� %d ,  ��ͷ� %d\n", users[0].tried, users[userst-1].tried);
        prints("\n=======TOP 10========\n");
        for(i=0;i<10;i++)
        if(i<userst)
            prints(" %-12s %-3d\n", users[i].userid, users[i].tried);
    }
    pressreturn();
    
    k=0;
    for(i=0;i<questst;i++){
        do_test_reset();
        move(1,0);
        prints("��%d��(��%d��)\n\n", i+1, questst);
        prints("%s\n", quests[i].topic);
        for(j=0;j<5;j++){
            if(!quests[i].quest[j][0]) break;
            prints("%d) %s\n", j+1, quests[i].quest[j]);
        }
        sprintf(buf, "ѡ��[1--%d]:", j);
        do{
            move(5+j,0);
            clrtoeol();
            getdata(5+j, 0, buf, ans, 2, true, NULL, true);
            l=atoi(ans);
        }while(l<1||l>j);
        answer[i]=l;
        k+=quests[i].value[l-1];
    }
    load_users(direct);
    j=1;
    for(i=0;i<userst;i++)
    if(!strcmp(users[i].userid, currentuser->userid)) {
        if(k>users[i].tried)
            users[i].tried = k;
        users[i].got++;
        users[i].create = time(0);
        j=0;
        break;
    }
    if(j) {
        if(!userst)
            users=(struct usertype*)malloc(sizeof(struct usertype)*(userst+1));
        strcpy(users[userst].userid, currentuser->userid);
        users[userst].tried = k;
        users[userst].got = 1;
        users[userst].create = time(0);
        userst++;
    }
    save_users(direct);

    if(j){
        load_users(FRIENDTOP);
        for(i=0;i<userst;i++)
        if(!strcmp(users[i].userid, lookuser->userid)) {
            users[i].tried++;
            break;
        }
        save_users(FRIENDTOP);
    }
    sethomestat(direct, lookuser->userid);
    save_stats(direct, answer);
    
    do_test_reset();
    move(2,0);
    prints("���Ѿ������� %s ���������\n", lookuser->userid);
    prints("��������߿��ܵ÷�Ϊ%d\n", possible_high());
    prints("��ĵ÷�Ϊ%d\n", k);
    pressreturn();
}

int friend_main()
{
    int i;
    init();

    while(i=usermenu()) {
        switch(i){
            case 1:
                new_friendtest();
                break;
            case 2:
                do_test();
                break;
        }
    }

    done();
}
