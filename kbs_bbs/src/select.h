#define SHOW_QUIT 0 /*�����˳�*/
#define SHOW_DIRCHANGE 1 /*��Ҫ���»�ȡ����ˢ���б�*/
#define SHOW_REFRESH 2	/* ֻ�ػ��б�*/
#define SHOW_REFRESHSELECT 3	/* ֻ�ػ�ѡ�����һ��*/
#define SHOW_SELCHANGE 4 /* ֻ�ı���ѡ��*/
#define SHOW_SELECT 5 /* ѡ�����б�֮�󷵻�*/
#define SHOW_CONTINUE 6 /*����ѭ��*/

#define	LF_MULTIPAGE 	0x1	/*��ҳ*/
#define	LF_HILIGHTSEL	0x2	/*ѡ����б�ɫ*/
#define	LF_VSCROLL	0x4	/*ÿ��itemռһ��*/
#define	LF_NEEDFORCECLEAN 0x8 
	/* ���ÿ��item���ȳ�����Ҫǿ���ÿո����*/
#define	LF_BELL			0x10	/*�����ʱ������*/
#define	LF_LOOP		0x20	/*ѭ���ģ��������һ��������ǰһ������ǰһ����ǰ�����һ��*/

#define  LF_ACTIVE	0x10000	/*�б������־*/
#define  LF_INITED	0x20000	/*�б��Ѿ���ʼ�����*/

/*��չ�Ĺ��ܼ�����*/
#define KEY_REFRESH		0x1000 
#define KEY_ACTIVE 		0x1001
#define KEY_DEACTIVE 	0x1002
#define KEY_SELECT		0x1003
#define KEY_INIT		0x1004

typedef struct tagPOINT {
	int x,y;
} POINT;

struct key_translate {
	int ch;
	int command;
};

struct _select_def {
	int flag; /*���*/

	int item_count; /*��item����*/
	int item_per_page; /*һҳ�ܹ��м���item��һ��Ӧ�õ���list_linenum */
	POINT * item_pos; /*һҳ����ÿ��item��λ�ã����Ϊ�գ���һ��һ������*/
	POINT title_pos; /*����λ��*/
	POINT endline_pos;  /*ҳĩλ��*/
	void* arg; /*�������ݵĲ���*/

/*	int page_num;  ��ǰҳ�� */
	int pos; /* ��ǰѡ���itemλ�� */
	int page_pos; /*��ǰҳ�ĵ�һ��item���*/
	char* prompt; /*ѡ���itemǰ�����ʾ�ַ�*/
	POINT cursor_pos; /*���λ��*/

	int new_pos; /*������SHOW_SELECTCHANGE ��ʱ�򣬰��µ�λ�÷�������*/

	struct key_translate* key_table; /*�������*/
	/*�ڲ�ʹ�õı���*/

	int (*init)(struct _select_def *conf); /*��ʼ��*/
	int (*page_init)(struct _select_def *conf); /*��ҳ��ʼ������ʱposλ���Ѿ����ı���*/
	int (*get_data)(struct _select_def *conf,int pos,int len); /*���posλ��,����Ϊlen������*/
	int (*show_data)(struct _select_def *conf,int pos); /*��ʾposλ�õ����ݡ�*/
	int (*show_title)(struct _select_def *conf); /*��ʾ���⡣*/
	int (*show_endline)(struct _select_def *conf); /*��ʾ��ĩ��*/
	int (*key_command)(struct _select_def *conf, int command); /*�����������*/
	int (*quit)(struct _select_def *conf); /*����*/
	int (*on_selchange)(struct _select_def *conf,int new_pos); /*�ı�ѡ���ʱ��Ļص�����*/
	int (*on_select)(struct _select_def *conf); /*ѡ����ĳһ��*/

	int (*active)(struct _select_def *conf); /*�����б�*/
	int (*deactive)(struct _select_def *conf); /*�б�ʧȥ����*/
};

int list_select(struct _select_def* conf,int key);
void list_select_loop(struct _select_def* conf);
