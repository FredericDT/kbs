#include "bbs.h"

struct userec deliveruser;

int main(int argc, char **argv)
{
    FILE *fp, *fp2;
    char fn[80],fn2[80];
    int i,j,ip[4],t,k;
    time_t tt;
    chdir(BBSHOME);
    resolve_boards();
    resolve_ucache();
    strcpy(fn, ".IPdenys");
    strcpy(fn2, "tmp/showipdeny.txt");
    fp=fopen(fn, "r");
    if(fp) {
        fp2=fopen(fn2, "w");
        while(!feof(fp)) {
            if(fscanf(fp, "%d %ld %d.%d.%d.%d %d %d", &i, &j, &ip[0], &ip[1], &ip[2], &ip[3], &t, &k)<=0) break;
            tt=(time_t) j;
            if(i==0)
                fprintf(fp2, "%s ����%d.%d.%d.%d��������ʱ��̫��.һСʱ�ڹ�����%d��.\n", ctime(tt), ip[0],ip[1],ip[2],ip[3],t);
            else
                fprintf(fp2, "%s ����%d.%d.%d.%d���ӹ���Ƶ��.һСʱ�ڹ�����%d��.\n", ctime(tt), ip[0],ip[1],ip[2],ip[3],t);
        }
        fclose(fp);
        fclose(fp2);
        unlink(fn);
        bzero(&deliveruser, sizeof(struct userec));
        strcpy(deliveruser.userid, "deliver");
        deliveruser.userlevel = -1;
        strcpy(deliveruser.username, "�Զ�����ϵͳ");
        currentuser = &deliveruser;
        strcpy(fromhost, "����");
        sprintf(fn, "%s�ķ����¼", ctime(time(0)));
        post_file(&deliveruser, NULL, fn2, "system_dev", fn, 0, 1);
    }
    return 0;
}
