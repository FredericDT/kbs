/*
 * $Id$
 */
#include "bbslib.h"

char genbuf[1024];
char currfile[STRLEN];

int main()
{
    int i, total = 0, mode;
    char board[80], *ptr;
    char buf[STRLEN];
    bcache_t *brd;

    init_all();
    if (!loginok)
        http_fatal("���ȵ�¼");
    strsncpy(board, getparm("board"), 60);
    mode = atoi(getparm("mode"));
    brd = getbcache(board);
    if (brd == 0)
        http_fatal("�����������");
    strcpy(board, brd->filename);
    if (!has_BM_perm(currentuser, board))
        http_fatal("����Ȩ���ʱ�ҳ");
    if (mode <= 0 || mode > 5)
        http_fatal("����Ĳ���");
    printf("<table>");
    for (i = 0; i < parm_num && i < 40; i++) {
        if (!strncmp(parm_name[i], "box", 3)) {
            total++;
            if (mode == 1)
                do_del(board, atoi(parm_name[i] + 3));
            if (mode == 2)
                do_set(board, atoi(parm_name[i] + 3), FILE_MARKED);
            if (mode == 3)
                do_set(board, atoi(parm_name[i] + 3), FILE_DIGEST);
            if (mode==4)
                do_set(board, atoi(parm_name[i] + 3), FILE_READ);
            if (mode == 5)
                do_set(board, atoi(parm_name[i] + 3), 0);
        }
    }
    printf("</table>");
    if (total <= 0)
        printf("����ѡ������<br>\n");
    printf("<br><a href=\"/bbsmdoc.php?board=%s\">���ع���ģʽ</a>", encode_url(buf, board, sizeof(buf)));
    http_quit();
}

/* modified by stiger,use id */
int do_del(char *board, int id)
{
    FILE *fp;
    int num = 0;
    //char path[256], buf[256], dir[256] ;
    char dir[256];
    struct fileheader f;
    //struct userec *u = NULL;
    bcache_t *brd = getbcache(board);

    sprintf(dir, "boards/%s/.DIR", board);
    //sprintf(path, "boards/%s/%s", board, file);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("����Ĳ���");
    while (1) {
        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
            break;
        if (f.id==id) {
            switch (del_post(num, &f, dir, board)) {
            case DONOTHING:
                http_fatal("����Ȩɾ������");
                break;
            default:
                printf("<tr><td>%s  </td><td>����:%s </td><td>ɾ���ɹ�.</td></tr>\n", f.owner, nohtml(f.title));
            }
            return;
        }
        num++;
    }
    fclose(fp);
    printf("<tr><td></td><td></td><td>�ļ�������.</td></tr>\n");
}

/* �� G ʱ��û�� post ����ժ�� */
/* modified by stiger,use id */
int do_set(char *board, int id, int flag)
{
    FILE *fp;
    char path[256], dir[256];
    struct fileheader f;

    sprintf(dir, "boards/%s/.DIR", board);
    //sprintf(path, "boards/%s/%s", board, file);
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("����Ĳ���");
    while (1) {
        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
            break;
        if (f.id==id) {
            if (flag==FILE_READ)
                f.accessed[1] |= flag;
            else
                f.accessed[0] |= flag;
            if (flag == 0) {
                f.accessed[0] &= ~(FILE_MARKED|FILE_DIGEST|FILE_SIGN);
                f.accessed[1] &= ~(FILE_READ);
            }
            fseek(fp, -1 * sizeof(struct fileheader), SEEK_CUR);
            fwrite(&f, sizeof(struct fileheader), 1, fp);
            fclose(fp);
            printf("<tr><td>%s</td><td>����:%s</td><td>��ǳɹ�.</td></tr>\n", f.owner, nohtml(f.title));
            return;
        }
    }
    fclose(fp);
    printf("<tr><td></td><td></td><td></td><td>�ļ�������.</td></tr>\n");
}

//change_post_flag(NULL, currentuser, 0, board, ent, fileinfo, direct, flag, 0)
