/*
 * $Id$
 */
#include "bbslib.h"

int main()
{
    char board[80], start[80], buf[256];
    struct boardheader bh;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(start, getparm("start"), 32);
    if (!loginok)
        http_fatal("�Ҵҹ����޷�ִ�д������, ���ȵ�¼");
    if (getboardnum(board,&bh) == 0 || !check_read_perm(currentuser, &bh))
        http_fatal("�����������");
    if (strcmp(currentuser->userid,"guest")) {
        brc_initial(currentuser->userid, board);
        brc_clear();
        brc_update(currentuser->userid);
    }
    strcpy(buf, board);
    encode_url(board, buf, sizeof(board));
    sprintf(buf, "/bbsdoc.php?board=%s&start=%s", board, start);
    refreshto(buf, 0);
    http_quit();
}
