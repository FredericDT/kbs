/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


/* Maximum Screen width in chars */
/*#define LINELEN (220) */
#define LINELEN LENGTH_SCREEN_LINE

/* Line buffer modes             */
#define MODIFIED (1)   /* if line has been modifed, output to screen   */
#define STANDOUT (2)   /* if this line has a standout region */

namespace BBSWindow {

//##ModelId=3CFDD0B8039B
//##Documentation
//## �ͻ��ն˵���Ļ������
class Screen
{
  public:
    //##ModelId=3CFE32F70347
    //##Documentation
    //## ���캯���������ʼ������Ļ����������
    Screen(int slns = 25, int scols = 80);

    //##ModelId=3CFE32F70383
    //##Documentation
    //## ȫ���ػ���Ļ
    void redoscr();

    //##ModelId=3CFE32F70397
    //##Documentation
    //## ˢ���޸Ĺ�����Ļ
    void refresh();

    //##ModelId=3CFE32F703AB
    //##Documentation
    //## ����ƶ���(x,y)��
    void move(int y, int x);

    //##ModelId=3CFE32F703E7
    //##Documentation
    //## ��õ�ǰ�Ĺ��λ��
    void getyx(int *y, int *x);

    //##ModelId=3CFE32F80031
    //##Documentation
    //## ����
    void clear();

    //##ModelId=3CFE32F80045
    //##Documentation
    //## �����i��
    void clear_whole_line(int i);

    //##ModelId=3CFE32F8006D
    //##Documentation
    //## �嵽��ĩ
    void clrtoeol();

    //##ModelId=3CFE32F80081
    //##Documentation
    //## �嵽ҳĩ
    void clrtobot();

    //##ModelId=3CFE32F8009F
    //##Documentation
    //## ���һ���ַ�
    void outc(unsigned char c);

    //##ModelId=3CFE32F800C7
    //##Documentation
    //## ���n���ַ����ַ���
    void outns(const char *str, int n);

    //##ModelId=3CFE32F80103
    //##Documentation
    //## ����ַ���
    void outs(const char *str);

    //##ModelId=3CFE32F8012B
    //##Documentation
    //## ����printf����Ļ���
    void prints(char *format, ...);

    //##ModelId=3CFE32F80167
    //##Documentation
    //## ����
    void scroll();

    //##ModelId=3CFE32F8017B
    //##Documentation
    //## �Ϲ�һ��
    void rscroll();

    //##ModelId=3CFE32F8018F
    //##Documentation
    //## ��ʼ���������ֱ�����õ�ǰ�������Լ��Ϸ���
    void standout();

    //##ModelId=3CFE32F801A3
    //##Documentation
    //## �������������ȥ����ǰ���ʷ�������
    void standend();

    //##ModelId=3CFE32F801B8
    //##Documentation
    //## �����i��
    void saveline(int line, int mode, char *buffer);

    //##ModelId=3D001A7E025A
    WORD get_cur_attr() const;

    //##ModelId=3D001A7E02D2
    void set_cur_attr(WORD left);

    //##ModelId=3D001D2D0031
    void set_fg_color(int color);

    //##ModelId=3D001DF7026C
    void set_bg_color(int color);

    //##ModelId=3D001E8500C2
    void set_highlight(bool hi);

    //##ModelId=3D001EBA001E
    void set_underline(bool underline);

    //##ModelId=3D001F410180
    void set_twinkling(bool twink);

    //##ModelId=3D00192E015C
    //##Documentation
    //## ��ɫ
    static const int BLACK;

    //##ModelId=3D00192E01FD
    //##Documentation
    //## ��ɫ
    static const int RED;

    //##ModelId=3D00192E0257
    //##Documentation
    //## ��ɫ
    static const int GREEN;

    //##ModelId=3D00192E02B1
    //##Documentation
    //## ��ɫ
    static const int YELLOW;

    //##ModelId=3D00192E031F
    //##Documentation
    //## ����ɫ
    static const int BLUE;

    //##ModelId=3D00192E0379
    //##Documentation
    //## �ۺ�ɫ
    static const int LT_RED;

    //##ModelId=3D00192E03DD
    //##Documentation
    //## ����ɫ
    static const int LT_BLUE;

    //##ModelId=3D00192F005A
    //##Documentation
    //## ��ɫ
    static const int WHITE;

  private:
    //##ModelId=3CFDE54B00F6
    //##Documentation
    //## ��Ļ��������������Ӧ�� >=25
    unsigned char scr_lns;

    //##ModelId=3CFDE54B010A
    //##Documentation
    //## ��Ļ����������Ӧ�� =80
    unsigned char scr_cols;

    //##ModelId=3CFDE54B0114
    //##Documentation
    //## ��ǰ���������
    unsigned char cur_ln;

    //##ModelId=3CFDE54B011E
    //##Documentation
    //## ��ǰ���������
    unsigned char cur_col;

    //##ModelId=3CFDE54B0128
    //##Documentation
    //## ��ǰ��Ļ��big_picture�����е���ʼ�±�
    unsigned char roll;

    //##ModelId=3CFDE54B0132
    //##Documentation
    //## ��ǰ��Ļ�Ѿ������˶�����
    unsigned char scrollcnt;

    //##ModelId=3CFDE54B013C
    //##Documentation
    //## �Ѿ�����Ļ���´���Ҫ����redoscr���ػ�
    unsigned char docls;

    //##ModelId=3CFDE54B015A
    //##Documentation
    //## ��ǰ���ڷ������
    unsigned char standing;

    //##ModelId=3CFDE54B015B
    //##Documentation
    //## �Ƿ����ڹ���esc������ַ�
    int inansi;

    //##ModelId=3CFE2F4F02BF
    //##Documentation
    //## �Ƿ����ansi��������
    int fitler_color;

    //##ModelId=3CFE48B600AD
    //##Documentation
    //## ��ǰ��������
    WORD cur_attr;

    //##ModelId=3D001F830009
    static const int ATTR_STAND;

    //##ModelId=3D001FCC01EF
    static const int ATTR_UNDERLINE;

    //##ModelId=3D0020050164
    static const int ATTR_HIGHLIGHT;

    //##ModelId=3D00202E0091
    static const int ATTR_TWINKLING;

    //##ModelId=3D00205C02FA
    static const int ATTR_DEFAULT;

    //##ModelId=3D00207D038E
    const int ATTR_MODIFIED;

    //##ModelId=3CFE303F03DD
    //##Documentation
    //## ��Ļ��������顣��һ��scr_lns�е�union screenline
    screenline *big_picture;

};

} // namespace BBSWindow

namespace BBSWindow {

//##ModelId=3CFDE56501F8
union screenline
{
    //##ModelId=3CFDE58C0077
    char rawlinebuf[LINELEN];

    //##ModelId=3CFE4D10000F
    struct _line linebuf;

};

} // namespace BBSWindow

namespace BBSWindow {

//##ModelId=3CFE4CED019F
struct _line
{
    //##ModelId=3CFE4D07030E
    //##Documentation
    //## ÿ�е���ʾ����
    char linebuf[80];

    //##ModelId=3CFE4D07030F
    //##Documentation
    //## ÿһ����Ӧ������
    DWORD attr[80];

    //##ModelId=3CFE4D070318
    //##Documentation
    //## ���޸Ĺ�����ʼλ��
    unsigned char mod_start;

    //##ModelId=3CFE4D070319
    //##Documentation
    //## �е����ݳ���
    unsigned char line_len;

};

} // namespace BBSWindow

