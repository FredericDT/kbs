/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    FILE *fp;
    int num = 0, total = 0, type, dt, mg = 0, og = 0;
    char dir[80], title[80], title2[80], title3[80], board[80], userid[80];
    char brdencode[STRLEN];
    bcache_t bh;
    struct fileheader x;

    init_all();
    printf("<center>%s -- ������������<hr class=\"default\"><br>\n", BBSNAME);
    type = atoi(getparm("type"));
    strsncpy(board, getparm("board"), 30);
    if (type == 0)
        return show_form(board);
    strsncpy(title, getparm("title"), 60);
    strsncpy(title2, getparm("title2"), 60);
    strsncpy(title3, getparm("title3"), 60);
    strsncpy(userid, getparm("userid"), 60);
    dt = atoi(getparm("dt"));
    if (!strcasecmp(getparm("mg"), "on"))
        mg = 1;
    if (!strcasecmp(getparm("og"), "on"))
        og = 1;
    if (dt < 0)
        dt = 0;
    if (dt > 9999)
        dt = 9999;
    if (getboardnum(board, &bh) == 0)
        http_fatal("�����������");
    strcpy(board, bh.filename);
    if (!check_read_perm(currentuser, &bh))
        http_fatal("�����������");
    sprintf(dir, "boards/%s/.DIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("�����������û��Ŀǰ����");
    encode_url(brdencode, board, sizeof(brdencode));
    printf("����������'%s'��, ���⺬: '%s' ", board, nohtml(title));
    if (title2[0])
        printf("�� '%s' ", nohtml(title2));
    if (title3[0])
        printf("���� '%s' ", nohtml(title3));
    printf("����Ϊ: '%s', '%d'�����ڵ�%s����.<br>\n", userid[0] ? userid_str(userid) : "��������", dt, mg ? "����" : "����");
    printf("<table width=\"610\">\n");
    printf("<tr><td>���</td><td>���</td><td>����</td><td>����</td><td>����</td></tr>\n");
    while (1) {
        if (fread(&x, sizeof(x), 1, fp) == 0)
            break;
        num++;
        if (title[0] && !strcasestr(x.title, title))
            continue;
        if (title2[0] && !strcasestr(x.title, title2))
            continue;
        if (userid[0] && strcasecmp(x.owner, userid))
            continue;
        if (title3[0] && strcasestr(x.title, title3))
            continue;
        if (abs(time(0) - get_posttime(&x)) > dt * 86400)
            continue;
        if (mg && !(x.accessed[0] & FILE_MARKED) && !(x.accessed[0] & FILE_DIGEST))
            continue;
        if (og && !strncmp(x.title, "Re: ", 4))
            continue;
        total++;
        printf("<tr><td>%d</td>", num);
        printf("<td>%s</td>", flag_str(x.accessed[0]));
        printf("<td>%s</td>", userid_str(x.owner));
        printf("<td>%12.12s</td>", 4 + wwwCTime(get_posttime(&x)));
        printf("<td><a href=\"bbscon?board=%s&id=%d&num=%d\">%40.40s </a></td></tr>\n", brdencode, x.id, num, x.title);
        if (total >= 999)
            break;
    }
    fclose(fp);
    printf("</table>\n");
    printf("<br>%d: ���ҵ� %d ƪ���·�������", num, total);
    if (total > 999)
        printf("(ƥ��������, ʡ�Ե�1000�Ժ�Ĳ�ѯ���)");
    printf("<br>\n");
    printf("[<a href=\"/bbsdoc.php?board=%s\">���ر�������</a>] [<a href=\"javascript:history.go(-1)\">������һҳ</a>]", brdencode);
    http_quit();
    return 0;
}

int show_form(char *board)
{
    char brdencode[STRLEN];

    encode_url(brdencode, board, sizeof(brdencode));
    printf("<table><form action=\"bbsbfind?type=1\" method=\"post\">\n");
    printf("<tr><td>��������: <input type=\"text\" maxlength=\"24\" size=\"24\" name=\"board\" value=\"%s\"><br>\n", board);
    printf("<tr><td>���⺬��: <input type=\"text\" maxlength=\"50\" size=\"20\" name=\"title\"> AND ");
    printf("<input type=\"text\" maxlength=\"50\" size=\"20\" name=\"title2\">\n");
    printf("<tr><td>���ⲻ��: <input type=\"text\" maxlength=\"50\" size=\"20\" name=\"title3\">\n");
    printf("<tr><td>�����ʺ�: <input type=\"text\" maxlength=\"12\" size=\"12\" name=\"userid\"><br>\n");
    printf("<tr><td>ʱ�䷶Χ: <input type=\"text\" maxlength=\"4\"  size=\"4\"  name=\"dt\" value=\"7\"> ������<br>\n");
    printf("<tr><td>��������: <input type=\"checkbox\" name=\"mg\"> ");
    printf("��������: <input type=\"checkbox\" name=\"og\"><br><br>\n");
    printf("<tr><td><input type=\"submit\" value=\"�ݽ���ѯ���\">\n");
    printf("</form></table>");
    printf("[<a href=\"/bbsdoc.php?board=%s\">��������</a>]", brdencode);
		   // [<a href=\"bbsfind\">ȫվ���²�ѯ</a>]", brdencode);
    http_quit();
}
