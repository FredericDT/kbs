#! /bin/sh

BBS_HOME=/home/bbs
INSTALL="//bin/install -c"
TARGET=/home/bbs/bin

echo "This script will install the whole BBS to ${BBS_HOME}..."
echo -n "Press <Enter> to continue ..."
read ans

if [ -d ${BBS_HOME} ] ; then
        echo -n "Warning: ${BBS_HOME} already exists, overwrite whole bbs [N]?"
        read ans
        ans=${ans:-N}
        case $ans in
            [Yy]) echo "Installing new bbs to ${BBS_HOME}" ;;
            *) echo "Abort ..." ; exit ;;
        esac
else
        echo "Making dir ${BBS_HOME}"
        mkdir ${BBS_HOME}
        chown -R bbs ${BBS_HOME}
        chgrp -R bbs ${BBS_HOME}
fi

echo "Setup bbs directory tree ....."
( cd bbshome ; tar cf - * ) | ( cd ${BBS_HOME} ; tar xf - )

chown -R bbs ${BBS_HOME}
chgrp -R bbs ${BBS_HOME}

${INSTALL} -m 770  -s -g bbs -o bbs    bbs        ${TARGET}
${INSTALL} -m 770  -s -g bbs -o bbs    bbs.chatd  ${TARGET}
${INSTALL} -m 4755 -s -g bin -o root   bbsrf      ${TARGET}

cat > ${BBS_HOME}/etc/sysconf.ini << EOF
# comment

BBSHOME         = "/home/bbs"
BBSID           = "NoName"
BBSNAME         = "Never Land BBS"
BBSDOMAIN       = "some.where.on.earth"

#SHOW_IDLE_TIME         = 1
KEEP_DELETED_HEADER     = 0

#BBSNTALKD      = 1
#NTALK          = "/bin/ctalk.sh"
#REJECTCALL     = "/bin/rejectcall.sh"
#GETPAGER       = "getpager"

BCACHE_SHMKEY   = 7813
UCACHE_SHMKEY   = 7912
UTMP_SHMKEY     = 3785
ACBOARD_SHMKEY  = 9013
ISSUE_SHMKEY    = 5002
GOODBYE_SHMKEY  = 5003

IDENTFILE       = "etc/preach"
EMAILFILE       = "etc/mailcheck"
#NEWREGFILE     = "etc/newregister"

PERM_BASIC      = 0x00001
PERM_CHAT       = 0x00002
PERM_PAGE       = 0x00004
PERM_POST       = 0x00008
PERM_LOGINOK    = 0x00010
PERM_DENYPOST   = 0x00020
PERM_CLOAK      = 0x00040
PERM_SEECLOAK   = 0x00080
PERM_XEMPT      = 0x00100
PERM_WELCOME    = 0x00200
PERM_BOARDS     = 0x00400
PERM_ACCOUNTS   = 0x00800
PERM_CHATCLOAK  = 0x01000
PERM_OVOTE      = 0x02000
PERM_SYSOP      = 0x04000
PERM_POSTMASK   = 0x08000
PERM_ANNOUNCE   = 0x10000
PERM_OBOARDS    = 0x20000
PERM_ACBOARD    = 0x40000
UNUSE1          = 0x80000
UNUSE2          = 0x100000
UNUSE3          = 0x200000
UNUSE4          = 0x400000
UNUSE5          = 0x800000
UNUSE6          = 0x1000000
UNUSE7          = 0x2000000
UNUSE8          = 0x4000000
UNUSE9          = 0x8000000
UNUSE10         = 0x10000000
UNUSE11         = 0x20000000

PERM_ADMENU    = PERM_ACCOUNTS , PERM_OVOTE , PERM_SYSOP,PERM_OBOARDS,PERM_WELCOME,PERM_ANNOUNCE
AUTOSET_PERM    = PERM_CHAT, PERM_PAGE, PERM_POST, PERM_LOGINOK

#include "etc/menu.ini"
EOF

cat > ${BBS_HOME}/etc/menu.ini << EOF
#---------------------------------------------------------------------
%S_MAIN






[34m"" -[1;34m _ ..[0m[36m-- -"[1;36m ''- - -.[36m _. ._. ._.[0m[1;34m. _ _. _[34m.- --" [36m'- --._ _ _.[1;36m '. ._ [34m _ .-.-[0m[34m.."[m
                                                                        
                                                        O                      
                 ,                                    o         ,/            
              .//                                   o         ///         
          ,.///;,   ,;/                                   ,,////.   ,/     [1;32m  )[m
         o;;;;;:::;///                              '.  o:::::::;;///    [1;32m ( ([m
        >;;...::::;\\\                                <::::::::;;\\\     [1;32m(  )[m
          ''\\\\\'" ''\                                 ''::::::' \\     [1;32m )([0;32m\[1m([m  
             '\\                                            '\\           [1;32m()[0;32m/[1;32m))[m
                                                               '\         [1;32m\([0;32m|[1m/[m
                                                                          [1;32m()[m[32m|[1m([m
                                                                           [1;32m([m[32m|[1m)[m
                                                                           [1;32m\[m[32m|[1m/[m
%

#---------------------------------------------------------------------
%menu TOPMENU
title        0, 0, "�D���"
screen       3, 0, S_MAIN
!M_EGROUP    10, 28, 0,         "EGroup",    "E) �����Q�װ�"
!M_MAIL      0, 0, 0,           "Mail",      "M) �B�z�H��
!M_TALK      0, 0, 0,           "Talk",      "T) �W�N��    Talk  "
!M_INFO      0, 0, PERM_BASIC,  "Info",      "I) �u��c   Xyz-1  "
!M_SYSINFO   0, 0, 0,           "Config",    "C) �t�θ�T Xyz-2  "
@LeaveBBS    0, 0, 0,           "GoodBye",   "G) ������          "
@Announce    0, 0, 0,           "0Announce", "0) ��ؤ��G��      "
!M_TIN       0, 0, 0,           "1TIN"    ,  "1) �峹�J��   TIN  "
#@ExecGopher  0, 0, 0,           "2Gopher",  "2) ��Ƭd�� Gopher "
!M_ExceMJ    0, 0, PERM_POST,   "Services",  "S) ���������A�ȻP Game"
#@SetHelp    0, 0, 0,           "Help",      "H) ���U�e���]�w    "
!M_ADMIN     0, 0, PERM_ADMENU,"Admin",      "A) �t�κ޲z�\���  "
%
#---------------------------------------------------------------------
%S_TIN



                                                                        
                                                                        
                                                                       
                                                                            
                            [1;33m���A�ɨ�����״I���@��[m
                          �z�w�w�w�w�w�w�w�w�w�w�w�{
                          �x��                  ���x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x��                  ���x
                          �|�w�w�w�w�w�w�w�w�w�w�w�}
                                                                        
                                                                
                                                                
%

#---------------------------------------------------------------------
%menu M_TIN
title        0, 0, "Tin ���"
screen       3, 0, S_TIN
@ExecTin     14, 28, PERM_POST, "1TIN",       "1) �峹�J��   TIN  "
@EGroups     0, 0, 0,           "TINinBBS",   "T) �� Tin �h�� BBS"
!..          0, 0, 0,           "Exit",       "E) �^��D���"
%


#------------------------------------------------------------------
%S_EGROUP






                     �z�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�{
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
%
#------------------------------------------------------------------
%menu M_EGROUP
title        0, 0, "�����Q�װϿ��"
screen       3, 0, S_EGROUP
@EGroups     10, 25, 0, "0BBS",      "0) BBS �t�� -- [����]"
@EGroups     0, 0, 0,   "1CCU",      "1) �����j�� -- [����]"
@EGroups     0, 0, 0,   "2Campus",   "2) �ն��T -- [�ն�] [��T]"
@EGroups     0, 0, 0,   "3Computer", "3) �q���޳N -- [�q��] [�t��]"
@EGroups     0, 0, 0,   "4Rec",      "4) �𶢮T�� -- [��] [����]"
@EGroups     0, 0, 0,   "5Art",      "5) �����ǳN -- [����] [����]"
#@EGroups    0, 0, 0,   "6Science",  "6) �ǳN��� -- [�Ǭ�] [�y��]"
@EGroups     0, 0, 0,   "6Sports",   "6) ��|���� -- [�B��] [¾��]"
@EGroups     0, 0, 0,   "7Talk",     "7) �ͤѲ�a -- [�ͤ�] [�s�D]"
@EGroups     0, 0, 0,   "TINinBBS",  "T) �� Tin �h�� BBS"
@PostArticle 0, 0, PERM_POST,   "Post",      "P) �廨����        "
@BoardsAll   0, 0, 0,   "Boards",    "B) �Ҧ��Q�װ�"
@BoardsNew   0, 0, 0,   "New",       "N) �\Ū�s�峹  New"
!..          0, 0, 0,   "Exit",      "E) �^��D���"
%
EGROUP0 = "0"
EGROUP1 = "C"
EGROUP2 = "IE"
EGROUP3 = "NR"
EGROUP4 = "am"
EGROUP5 = "enij"
EGROUP6 = "rs"
EGROUP7 = "xf"
EGROUPT = "*"
#------------------------------------------------------------------
%S_MAIL






                          �z�w�w�w�w�w�w�w�w�w�w�w�{
                          �x��                  ���x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x                      �x
                          �x��                  ���x
                          �|�w�w�w�w�w�w�w�w�w�w�w�}
%

#------------------------------------------------------------------
%menu M_MAIL
title            0, 0, "�B�z�H����"
screen           3, 0, S_MAIL
@ReadNewMail     11, 29, 0,             "NewMail", "N) ���\�s�H��"
@ReadMail        0, 0, 0,               "ReadMail","R) ���\�����H��"
@SendMail        0, 0, PERM_POST,       "SendMail","S) ��p�ȱ�"
@GroupSend       0, 0, PERM_POST,       "Gsend",   "G) �H�H���@�s�H"
@SetFriends      0, 0, PERM_BASIC,      "Override","O)�z�]�w�n�ͦW��"
@OverrideSend    0, 0, PERM_POST,       "Osend",   "O)�|�H�H���n�ͦW��"
@SetMailList     0, 0, PERM_POST,       "Makelist","M)�z�]�w�H�H�W��   "
@ListSend        0, 0, PERM_POST,       "Lsend",   "L)�|�H���]�w���W�� "
@SendNetMail     0, 0, PERM_POST,       "Internet","I) ���F�Ǯ�"
!..              0, 0, 0,               "Exit",    "E) �^��D���"
%
#------------------------------------------------------------------
%S_TALK






  ,-*~,-*~''~*-,._.,-*~''~*-,._.,-*~''~*-,._.,-*~''~*-,._.,-*~''~*-,'~*-,._.,
                                                
                                                
                                                
                                                
                                                
                                        
                                                
                                                
                                                
                                                
                                                
                                                        
  ,-*~,-*~''~*-,._.,-*~''~*-,._.,-*~''~*-,._.,-*~''~*-,._.,-*~''~*-,'~*-,._.,
%

#------------------------------------------------------------------
%menu M_TALK
title        0, 0, "�W�N�����"
screen       3, 0,   S_TALK
@ShowFriends 10, 27,    PERM_BASIC, "Friends",  "F) �]���n��   Friend"
@ShowLogins  0, 0,      0,          "Users",    "U) ���U�|��    Users"
@QueryUser   0, 0,      PERM_BASIC, "Query",    "Q) �d�ߺ���    Query"
@Talk        0, 0,      PERM_POST,  "Talk",     "T) �N���ӻy     Talk"
@SetPager    0, 0,      PERM_BASIC, "Pager",    "P) �Q�@�ӤH�R�R Page"
@SendMsg     0, 0,      PERM_POST,  "SendMsg",  "S) �e�T�����O�H"
@SetFriends  0, 0,      PERM_BASIC, "Override", "O) �]�w�n�ͦW��"
@EnterChat   0, 0,      PERM_BASIC, "1Chat",    "1) �]�ߤl�ȴ�"
#@EnterChat   0, 0,      PERM_BASIC, "2Chat",    "2) �]�ߤl�ȴ�"
#@EnterChat   0, 0,      PERM_BASIC, "3Relay",   "3) �֫v�F    "
#@EnterChat   0, 0,      PERM_BASIC, "4Chat",    "4) �Ѥj��ѫ�"
@ExecIrc     0, 0,      PERM_POST,  "IRC",      "I) �P�ڷ|��      IRC"
#@ListLogins 0, 0,      PERM_CHAT,  "List",     "L) �W���ϥΪ�²��"
@Monitor     0, 0,      PERM_CHAT,  "Monitor",  "M) ��������"
@RealLogins  0, 0,      PERM_SYSOP, "Nameofreal","N) �W���ϥΪ̯u��m�W"
!..          0, 0, 0,               "Exit",     "E) �^��D���"
%

#------------------------------------------------------------------
%S_INFO






                          ����������������������������
                          ��                        ��
                          ��                        ��
                          ��                        ��
                          ��                        ��
                          ��                        ��
                          ��                        ��
                          ��                        ��
                          ��                        ��                    ,--,/
                          ��                        ��w��    �~�w��_ ___/ /\|
                          ��                        ��  ���w�w�� ,:( )__,_)  ~
                          ��                        ��         //  //   L==;
                          ����������������������������         '   \     | ^
 ,_.-*'"'*-._,_.-*'"'*-._,_.-*'"'*-._,_.-*'"'*-._,_.-*'"'*-._,_.-*'"'*-._,_.-*
%

#------------------------------------------------------------------
%menu M_INFO
title        0, 0, "�u��c���"
screen       3, 0, S_INFO
@FillForm    11, 29, 0,             "FillForm",  "F) ���U�ԲӭӤH���"
@SetInfo     0, 0,      PERM_BASIC, "Info",      "I) �]�w�ӤH���"
@UserDefine  0, 0,      PERM_BASIC, "Userdefine","U) �ӤH�ѼƳ]�w"
#@OffLine     0, 0,      PERM_BASIC, "OffLine",   "O) �۱�~~~~~"
#@EditSig    0, 0,      PERM_POST,  "Signature", "S) ��L(ñ�W��) Sig."
#@EditPlan   0, 0,      PERM_POST,  "QueryEdit", "Q) ���ӤH������ Plan"
@EditUFiles  0, 0,      PERM_POST,  "WriteFiles","W) �s�׭ӤH�ɮ�"
@SetCloak    0, 0,      PERM_SYSOP, "Cloak",     "C) �����N"
@SetHelp     0, 0,      PERM_BASIC, "Help",      "H) ���U�e���]�w"
!..          0, 0, 0,               "Exit",      "E) �^��D���"
%

#------------------------------------------------------------------
%S_SYSINFO






         ������������������������������������������������������������
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ��                                                        ��
         ������������������������������������������������������������
                                                                        
%
#------------------------------------------------------------------
%menu M_SYSINFO
title        0, 0, "�t�θ�T���"
screen       3, 0, S_SYSINFO
@ShowLicense 10, 28, 0,        "License",   "L) �ϥΰ���      "
@ShowVersion 0, 0, 0,          "Copyright", "C) ���z�v��T    "
@Notepad     0,0,0,            "Notepad",   "N) �ݬݯd���O"    
@ShowDate    0, 0, 0,          "Date",      "D) �ثe�ɨ�     Date "
@DoVote      0, 0, PERM_BASIC, "Vote",      "V) �����벼     Vote "
@VoteResult  0, 0, PERM_BASIC, "Results",   "R) �ﱡ����          "
@MailAll     0, 0, PERM_SYSOP, "MailAll",   "M) �H�H���Ҧ��H      "
@ExecBBSNet  0, 0, PERM_POST,  "BBSNet",    "B) ����Ȫe(�Ѱ��ѳ~)"  
@ShowWelcome 0, 0, 0,          "Welcome",   "W) �i���e��      "
@SpecialUser 0, 0, PERM_POST,  "Users",     "U) �X�椽���C��  "
#@ExecViewer  0, 0, 0,         "0Announce", "0) �������G�� (�ª��t��)"
!..          0, 0, 0,          "Exit",      "E) �^��D���"
%

#---------------------------------------------------------------------------
%S_ExceMJ






                     �z�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�{
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
                     �x                                  �x
 ,_.-*'"'*-._,_.-*'"'*-._,_.-*'"'*-._,_.-*'"'*-._,_.-*'"'*-._,_.-*'"'*-._,_.-*
%
#------------------------------------------------------------------
%menu M_ExceMJ
title        0, 0, "����Game�j���L  "
screen       3, 0, S_ExceMJ
@WWW        12,29, 0,           "WWW",         "W) ��r�Ҧ��� WWW"
@ExecGopher  0, 0, 0,           "Gopher",      "G) ��Ƭd�� Gopher "
@ExecMJ      0, 0, PERM_CLOAK, "Mujang",       "M) �����±N"
@ExecBIG2    0, 0,  0,          "Big2",        "B) �ѤG�̤j  "
@ExecCHESS   0, 0,  0,          "Chess",       "C) �M�ԶH��  "
!..          0, 0,  0,          "Exit",        "E) �^��D���"
%

#------------------------------------------------------------------
%S_ADMIN






                                                                        
                                                                             
                                                                             
                                                                        
                                                                        
                                                                        
                                                                        
                                                                        
                                                                        
                                                                        
                                                                        
                                                                        
                                                                        
                                                                        
%

#------------------------------------------------------------------
%menu M_ADMIN
title        0, 0, "�t�κ��@���"
screen       3, 0, S_ADMIN
@CheckForm   9, 28,PERM_SYSOP,  "Register",    "R) �]�w�ϥΪ̵��U���"
@ModifyInfo  0, 0, PERM_SYSOP,  "Info",        "I) �ק�ϥΪ̸��"
@ModifyLevel 0, 0, PERM_SYSOP,  "Level",       "L) ���ϥΪ̪��v��"
@KickUser    0, 0, PERM_SYSOP,  "Kick",        "K) �N�ϥΪ̽�X���t��"
@DelUser     0, 0, PERM_ACCOUNTS,"DeleteUser", "D) �屼�ϥΪ̱b��"
@OpenVote    0, 0, PERM_OVOTE,  "Vote",        "V) �|��t�Χ벼"
@NewBoard    0, 0, PERM_OBOARDS,"NewBoard",    "N) �}�Ҥ@�ӷs���Q�װ�"
@EditBoard   0, 0, PERM_ANNOUNCE,"ChangeBoard","C) �ק�Q�װϻ����P�]�w
@DelBoard    0, 0, PERM_OBOARDS,"BoardDelete","B) �屼�@�ӵL�Ϊ��Q�װ�"
@SetTrace    0, 0, PERM_SYSOP,  "Trace",       "T) �]�w�O�_�O��������T"
@CleanMail   0, 0, PERM_SYSOP,  "MailClean",   "M) �M���Ҧ�Ū�L���p�H�H��"
@Announceall 0, 0, PERM_SYSOP,  "Announceall", "A) ��Ҧ��H�s��"
@EditSFiles  0, 0, PERM_WELCOME,"Files",       "F) �s��t���ɮ�"
!..          0, 0, 0,           "Exit",        "E) �^��D���"
%

#------------------------------------------------------------------


EOF

echo "Install is over...."
