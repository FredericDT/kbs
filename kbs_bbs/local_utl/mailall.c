#include "bbs.h"
int mailuser(struct userec* user,void* arg)
{
    mail_file("Arbitrator", "tekan/JuryMail01", user->userid, "[公告]BBS 水木清华站仲裁委员会集中招新", BBSPOST_LINK, NULL);
    return 0;
};

int main(int etn, char **atppp)
{
    chdir(BBSHOME);
    resolve_ucache();
    resolve_boards();
    resolve_utmp();
    return apply_users(mailuser,NULL);
}

