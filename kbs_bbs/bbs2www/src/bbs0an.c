/*
 * $Id$
 */
#include "bbslib.h"

void ann_show_item(MENU * pm, ITEM * it)
{
    char title[STRLEN];
    char *id;
    char buf[256];
    char pathbuf[256];
    char *ptr;

    strncpy(title, it->title, sizeof(title) - 1);
    title[sizeof(title) - 1] = '\0';
    if (strlen(title) <= 39) {
        id = "";
    } else {
        if ((ptr = strchr(title + 38, '(')) != NULL) {
            *ptr = '\0';
            id = ptr + 1;
            if (strncmp(id, "BM: ", 4) == 0)
                id += 4;
            if ((ptr = strchr(id, ')')) != NULL)
                *ptr = '\0';
        } else if ((ptr = strchr(title + 38, ' ')) != NULL) {
            *ptr = '\0';
            id = ptr + 1;
            trim(id);
        } else
            id = "";
        rtrim(title);
    }
    printf("<tr><td>%d</td>", pm->now + 1);
    sprintf(buf, "%s/%s", pm->path, it->fname);
    ptr = strchr(pm->path, '/');
    if (!file_exist(buf)) {
        printf("<td>[����] </td><td>%s</td>", nohtml(title));
    } else if (file_isdir(buf)) {
        snprintf(pathbuf, sizeof(pathbuf), "%s/%s", ptr == NULL ? "" : ptr, it->fname);
        printf("<td>[Ŀ¼] </td><td><a href=\"bbs0an?path=%s\">%s</a></td>", http_encode_string(pathbuf, sizeof(pathbuf)), nohtml(title));
    } else {
        snprintf(pathbuf, sizeof(pathbuf), "%s/%s", ptr == NULL ? "" : ptr, it->fname);
        printf("<td>[�ļ�] </td><td><a href=\"bbsanc?path=%s\">%s</a></td>", http_encode_string(pathbuf, sizeof(pathbuf)), nohtml(title));
    }
    if (id[0]) {
        printf("<td>%s</td>", userid_str(id));
    } else {
        printf("<td>&nbsp;</td>");
    }
    printf("<td>%6.6s %s</td></tr>\n", wwwCTime(file_time(buf)) + 4, wwwCTime(file_time(buf)) + 20);
}

void ann_show_directory(char *path)
{
    MENU me;
    ITEM *its;
    int len;
    char board[STRLEN];
    char pathbuf[256];
    char buf[STRLEN];
    int i;

    if (strstr(path, "..") || strstr(path, "SYSHome"))  /* SYSHome? */
        http_fatal("��Ŀ¼������");
    if (path[0] != '\0') {
        len = strlen(path);
        if (path[len - 1] == '/')
            path[len - 1] = '\0';
        if (path[0] == '/')
            snprintf(pathbuf, sizeof(pathbuf), "0Announce%s", path);
        else
            snprintf(pathbuf, sizeof(pathbuf), "0Announce/%s", path);
        if (ann_traverse_check(pathbuf, currentuser) < 0)
            http_fatal("��Ŀ¼������");
    } else
        strcpy(pathbuf, "0Announce");
    if ((its = ann_alloc_items(MAXITEMS)) == NULL)
        http_fatal("�����ڴ�ʧ��");
    ann_set_items(&me, its, MAXITEMS);
    me.path = pathbuf;
    if (ann_load_directory(&me) == 0) {
        board[0] = '\0';
        ann_get_board(path, board, sizeof(board));
        buf[0] = '\0';
        if (board[0] != '\0')
            sprintf(buf, "%s��", board);
        printf("<center>\n");
        printf("%s -- %s������<hr class=\"default\">\n", BBSNAME, buf);
        if (me.num <= 0) {
            printf("<br>&lt;&lt; Ŀǰû������ &gt;&gt;\n");
            http_quit();
        }
        printf("<table class=\"default\" border=\"1\" width=\"610\">\n");
        printf("<tr><td>���</td><td>���</td><td>����</td><td>����</td><td>����</td></tr>\n");
        me.now = 0;
        for (i = 0; i < me.num; i++) {
            trim(me.item[i]->title);
            ann_show_item(&me, me.item[i]);
            me.now++;
        }
        printf("</table><br>[<a href=\"javascript:history.go(-1)\">������һҳ</a>] ");
        if (board[0])
            printf("[<a href=\"/bbsdoc.php?board=%s\">��������</a>]\n", encode_url(buf, board, sizeof(buf)));
        printf("</center>\n");
    }
    ann_free_items(its, MAXITEMS);
}

int main()
{
    char path[512];

    init_all();
    strsncpy(path, getparm("path"), 511);
    ann_show_directory(path);
    http_quit();
}
