#include "bbs.h"
int f_rm(char *fpath);
int f_mv(char *src, char *dst);
int f_cp(char *src, char *dst, int mode);
void f_cat(char *fpath, char *msg);
int f_touch(char *filename);
int f_ln(char *src, char *dst);

/* �ַ���ƥ�亯��*/
char* bm_strstr(const char* string,const char* pattern);
/* �ַ������ƥ�亯��*/
char* bm_strstr_rp(const char* string,const char* pattern,
	size_t* shift,bool* init);
/* �ַ�����Сд�����е�ƥ�亯��*/
char* bm_strcasestr(const char* string,const char* pattern);
/* �ַ�����δ�Сд������ƥ�亯��*/
char* bm_strcasestr_rp(const char* string,const char* pattern,
	size_t* shift,bool* init);
void *memfind(const void *in_block,     /* ���ݿ� */
              const size_t block_size,  /* ���ݿ鳤�� */
              const void *in_pattern,   /* ��Ҫ���ҵ����� */
              const size_t pattern_size,        /* �������ݵĳ��� */
              size_t * shift,   /* ��λ��Ӧ����256*size_t������ */
              bool * init); /* �Ƿ���Ҫ��ʼ����λ�� */

void *txtfind(const void *in_block,     /* ���ݿ� */
              const size_t block_size,  /* ���ݿ鳤�� */
              const void *in_pattern,   /* ��Ҫ���ҵ����� */
              const size_t pattern_size,        /* �������ݵĳ��� */
              size_t * shift,   /* ��λ��Ӧ����256*size_t������ */
              bool * init); /* �Ƿ���Ҫ��ʼ����λ�� */

