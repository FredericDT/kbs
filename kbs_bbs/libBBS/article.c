#include "bbs.h"

void cancelpost(char    *board,char *userid,struct fileheader *fh,int     owned,int     autoappend);
int get_postfilename(char* filename,char* direct)
{
    static const char post_sufix[]="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int fp;
    time_t now;
    int i;
    char fname[255];
    int pid=getpid();
    /* 自动生成 POST 文件名 */
    now = time(NULL);
    for (i=0;i<10;i++) {
        sprintf(filename,"M.%d.%c%c",now,post_sufix[(pid+i)%62],post_sufix[(pid*i)%62]);
	sprintf(fname,"%s/%s",direct,filename);
    	if ((fp = open(fname,O_CREAT|O_EXCL|O_WRONLY,0644)) != -1) {
		break;
	};
    }
    if (fp==-1) return -1;
    close(fp) ;
    return 0;
}

int isowner(struct userec* user,struct fileheader* fileinfo)
{
    char buf[25];
    time_t posttime;
    if (strcmp(fileinfo->owner,user->userid))
        return 0;
    posttime = atoi(fileinfo->filename+2);
    if (posttime<user->firstlogin)
        return 0;
    return 1;
}

int cmpname(fhdr,name)  /* Haohmaru.99.3.30.比较 某文件名是否和 当前文件 相同 */
struct fileheader *fhdr ;
char  name[STRLEN];
{
    if(!strncmp(fhdr->filename,name,STRLEN))
        return 1 ;
    return 0 ;
}

int do_del_post(struct userec* user,int ent ,struct fileheader *fileinfo ,char *direct ,char* board,int digestmode,int decpost)
{
    FILE        *fn;
    char        buf[512];
    char        usrid[STRLEN];
    char        *t ;
    int         owned, fail;

    strcpy(buf,direct) ;
    if( (t = strrchr(buf,'/')) != NULL )
        *t = '\0' ;
/* .post.X not use???! KCN
postreport(fileinfo->title, -1, currboard); added by alex, 96.9.12 */
/*    if( keep <= 0 ) {*/
        fail = delete_record(direct,sizeof(struct fileheader),ent,cmpname,fileinfo->filename);
/*
    } else {
        fail = update_file(direct,sizeof(struct fileheader),ent,cmpfilename,
                           cpyfilename);
    }
    */
    owned = isowner(user,fileinfo);
    if( !fail ) {
        cancelpost( board, user->userid, fileinfo, owned ,1);
	 updatelastpost(board);
/*
        sprintf(buf,"%s/%s",buf,fileinfo->filename) ;
        if(keep >0)  if ( (fn = fopen( buf, "w" )) != NULL ) {
            fprintf( fn, "\n\n\t\t本文章已被 %s 删除.\n",
                     currentuser->userid );
            fclose( fn );
        }
*/
        if ((YEA != digestmode) /* 不可以用 “NA ==” 判断：digestmode 三值 */
                &&!((fileinfo->accessed[0]&FILE_MARKED)
                    &&(fileinfo->accessed[1]& FILE_READ)
                    &&(fileinfo->accessed[0]& FILE_FORWARDED)))
        { /* Leeward 98.06.17 在文摘区删文不减文章数目 */
            if (owned)
            {
                if ((int)user->numposts > 0 && !junkboard(board))
                {
                    user->numposts--;/*自己删除的文章，减少post数*/
                }
            } else if ( !strstr(usrid,".")&&BMDEL_DECREASE&&decpost/*版主删除,减少POST数*/){
                struct userec* lookupuser;
                int id = getuser(usrid,&lookupuser);
                if(id && (int)lookupuser->numposts > 0 && !junkboard(board) && strcmp(board, "sysmail") ) /* SYSOP MAIL版删文不减文章 Bigman: 2000.8.12*/
                { /* Leeward 98.06.21 adds above later 2 conditions */
                    lookupuser->numposts--;
                }
            }
        }
    	 bbslog("1bbs","Del '%s' on '%s'",fileinfo->title,board) ; /* bbslog*/
        return 0;
    }
    return -1;
}

/* by ylsdd 
   unlink action is taked within cancelpost if in mail mode,
   otherwise this item is added to the file '.DELETED' under
   the board's directory, the filename is not changed. 
   Unlike the fb code which moves the file to the deleted
   board.
*/
void cancelpost( board, userid, fh, owned ,autoappend)
char    *board, *userid;
struct fileheader *fh;
int     owned;
int     autoappend;
{
    struct fileheader   postfile;
    char oldpath[sizeof(genbuf)];
    struct fileheader* ph;
    time_t now;
#ifdef BBSMAIN
    if(uinfo.mode==RMAIL)  {
       sprintf(oldpath,"mail/%c/%s/%s",toupper(currentuser->userid[0]),
                currentuser->userid,fh->filename);
       unlink(oldpath);
       return;
    }
#endif
    if (autoappend) ph=&postfile;
    else ph=fh;

    sprintf(oldpath, "/board/%s/%s.html", board, fh->filename);
    ca_expire_file(oldpath);

    if (autoappend) {
      bzero(&postfile,sizeof(postfile));
      strcpy( postfile.filename, fh->filename );
      strncpy( postfile.owner, fh->owner, IDLEN+2 );
      postfile.owner[IDLEN+1]=0;
    };
    now=time(NULL);
    sprintf( oldpath, "%-32.32s - %s", fh->title, userid );
    strncpy( ph->title,oldpath, STRLEN );
    ph->title[STRLEN-1]=0;
    ph->accessed[11]=now/(3600*24)%100; /*localtime(&now)->tm_mday;*/
    if (autoappend) {
        setbdir( (owned)?5:4,oldpath, board );
        append_record( oldpath, &postfile, sizeof(postfile) );
    }
}


void add_loginfo(char* filepath,struct userec* user,char* currboard,int Anony)    /* POST 最后一行 添加 */
{
    FILE *fp;
    int color,noidboard;
    char fname[STRLEN];

    noidboard=(seek_in_file("etc/anonymous",currboard)&&Anony); /* etc/anonymous文件中 是匿名版版名 */
    color=(user->numlogins%7)+31; /* 颜色随机变化 */
    sethomefile( fname, user->userid,"signatures" );
    fp=fopen(filepath,"a");
    if (!dashf(fname)||          /* 判断是否已经 存在 签名档 */
            user->signature==0||noidboard)
    {       fputs("\n--\n", fp);
    }else{ /*Bigman 2000.8.10修改,减少代码 */
        fprintf(fp,"\n");}
    /* 由Bigman增加:2000.8.10 Announce版匿名发文问题 */
    if (!strcmp(currboard,"Announce"))
        fprintf(fp, "[m[%2dm※ 来源:·%s %s·[FROM: %s][m\n"
                ,color,BBS_FULL_NAME,email_domain(),
                NAME_BBS_CHINESE" BBS站");
    else
        fprintf(fp, "\n[m[%2dm※ 来源:·%s %s·[FROM: %s][m\n"
                ,color,BBS_FULL_NAME,email_domain(),(noidboard)?NAME_ANONYMOUS_FROM:fromhost);

    fclose(fp);
    return;
}

void addsignature(FILE *fp,struct userec* user,int sig)
{
    FILE *sigfile;
    int  i,valid_ln=0;
    char tmpsig[MAXSIGLINES][256];
    char inbuf[256];
    char fname[STRLEN];
    char tmp[STRLEN];

   if (sig==0) return;
    sethomefile( fname, user->userid,"signatures" );
    if ((sigfile = fopen(fname, "r"))== NULL)
    {return;}
    fputs("\n--\n", fp);
    for (i=1; i<=(sig-1)*MAXSIGLINES&sig!=1; i++)
    {
        if (!fgets(inbuf, sizeof(inbuf), sigfile)){
            fclose(sigfile);
            return;}
    }
    for (i=1; i<=MAXSIGLINES; i++) {
        if (fgets(inbuf, sizeof(inbuf), sigfile))
        {
            if(inbuf[0]!='\n')
                valid_ln=i;
            strcpy(tmpsig[i-1],inbuf);
        }
        else break;
    }
    fclose(sigfile);
    for(i=1;i<=valid_ln;i++)
        fputs(tmpsig[i-1], fp);
}

int write_posts(char *id, char *board, char *title)
{
    char *ptr;
    time_t now;
    struct posttop postlog, pl;

    if(junkboard(board)||normal_board(board)!=1)
        return ;
    now = time(0) ;
    strcpy(postlog.author, id);
    strcpy(postlog.board, board);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    strncpy(postlog.title, ptr, 65);
    postlog.date = now;
    postlog.number = 1;

    { /* added by Leeward 98.04.25 
    TODO: 这个地方有点不妥,每次发文要遍历一次,保存到.Xpost中,
    用来完成十大发文统计针对ID而不是文章.不好
    	KCN*/
        char buf[STRLEN];
        int  log = 1;
        FILE *fp = fopen(".Xpost", "r");

        if (fp)
        {
            while (!feof(fp))
            {
                fread(&pl, sizeof(pl), 1, fp);
                if (feof(fp)) break;

                if (!strcmp(pl.title, postlog.title)
                        && !strcmp(pl.author, postlog.author)
                        && !strcmp(pl.board, postlog.board))
                {
                    log = 0;
                    break;
                }
            }
            fclose(fp);
        }

        if (log)
        {
            append_record(".Xpost", &postlog, sizeof(postlog));
            append_record(".post", &postlog, sizeof(postlog));
        }
    }

    append_record(".post.X", &postlog, sizeof(postlog));
}

void write_header(FILE *fp,struct userec* user,int in_mail,char* board,char* title,int Anony,int mode)
{
    int  noname;
    char uid[20] ;
    char uname[40] ;
    time_t now;

    now = time(0) ;
    strncpy(uid,user->userid,20) ;
    uid[19] = '\0' ;
    if (in_mail)
#if defined(MAIL_REALNAMES)
    	strncpy(uname,user->realname,NAMELEN) ;
#else
		strncpy(uname,user->username,NAMELEN) ;
#endif
    else
#if defined(POSTS_REALNAMES)
        strncpy(uname,user->realname,NAMELEN) ;
#else
        strncpy(uname,user->username,NAMELEN) ;
#endif
    /* uid[39] = '\0' ; SO FUNNY:-) 定义的 20 这里却用 39 !
                        Leeward: 1997.12.11 */
    uname[39] = 0; /* 其实是写错变量名了! 嘿嘿 */
    title[STRLEN-10] = '\0' ;
    noname=seek_in_file("etc/anonymous",board);
    if(in_mail)
        fprintf(fp,"寄信人: %s (%s)\n",uid,uname) ;
    else
    {
        if(mode==0&&!(noname&&Anony))
        {
            write_posts(user->userid,board,title);
        }

        if (!strcmp(board,"Announce"))
            /* added By Bigman */
            fprintf(fp,"发信人: %s (%s), 信区: %s       \n","SYSOP",
                    NAME_SYSOP,board) ;
        else
            fprintf(fp,"发信人: %s (%s), 信区: %s       \n",(noname&&Anony)?board:uid,
                    (noname&&Anony)?NAME_ANONYMOUS:uname,board) ;
    } 

    fprintf(fp,"标  题: %s\n",title) ;
    fprintf(fp,"发信站: %s (%24.24s)\n",BBS_FULL_NAME,ctime(&now)) ;
    if(in_mail)
        fprintf(fp,"来  源: %s \n",fromhost) ;
    fprintf(fp,"\n");

}

void getcross(char* filepath,char* quote_file,struct userec* user,int in_mail,char* board,char* title,int Anony,int mode)      /* 把quote_file复制到filepath (转贴或自动发信)*/
{
    FILE        *inf, *of;
    char        buf[256];
    char        owner[256];
    int                     count;
    time_t      now;

    now=time(0);
    inf=fopen(quote_file,"r");
    of = fopen( filepath, "w" );
    if(inf==NULL || of ==NULL)
    {
        /*---	---*/
        if(NULL != inf) fclose(inf);
        if(NULL != of ) fclose(of) ;
        /*---	---*/

        report("Cross Post error");
        return ;
    }
    if(mode==0/*转贴*/)
    {
        int normal_file;         
        int header_count;        
        normal_file=1;           

        write_header(of,user,in_mail,board,title,Anony,1/*不写入 .posts*/);
        if(fgets( buf, 256, inf ) != NULL)
            { for(count=8;buf[count]!=' ';count++)
                owner[count-8]=buf[count];}
        owner[count-8]='\0';
        if(in_mail==YEA)
            fprintf( of, "[1;37m【 以下文字转载自 [32m%s [37m的信箱 】\n",user->userid);
        else
            fprintf( of, "【 以下文字转载自 %s 讨论区 】\n",board);
        if (id_invalid(owner)) normal_file=0;
       if (normal_file) {                                                   
         for (header_count=0;header_count<3;header_count++) {               
           if ( fgets( buf, 256, inf ) == NULL) break;/*Clear Post header*/ 
          }                                                        
          if ((header_count!=2)||(buf[0]!='\n')) normal_file=0;    
        }                                                          
        if (normal_file)                                           
            fprintf( of, "【 原文由 %s 所发表 】\n",owner);        
        else                                                       
            fseek(inf,0,SEEK_SET);                                 

    }else if(mode==1/*自动发信*/)
    {
        fprintf( of,"发信人: deliver (自动发信系统), 信区: %s\n",board);
        fprintf( of,"标  题: %s\n",title);
        fprintf( of,"发信站: %s自动发信系统 (%24.24s)\n\n",BBS_FULL_NAME,ctime(&now));
        fprintf( of,"【此篇文章是由自动发信系统所张贴】\n\n");
    }else if(mode==2)
    {
        write_header(of,user,in_mail,board,title,Anony,0/*写入 .posts*/);
    }
    while( fgets( buf, 256, inf ) != NULL)
    {
        if((strstr(buf,"【 以下文字转载自 ")&&strstr(buf,"讨论区 】"))||(strstr(buf,"【 原文由")&&strstr(buf,"所发表 】")))
            continue; /* 避免引用重复 */
        else
            fprintf( of, "%s", buf );
    }
    fclose( inf );
    fclose( of);
    *quote_file = '\0';
}


