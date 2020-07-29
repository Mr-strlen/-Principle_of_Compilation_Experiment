                         /*PL/0 ����ϵͳC�汾ͷ�ļ� pl0.h*/

# define norw 17                 /*�ؼ��ָ���*/
# define txmax 100               /*���ֱ�����*/
# define nmax  14                /*number�����λ��*/
# define al 10                   /*���ŵ���󳤶�*/
# define amax 2047               /*��ַ�Ͻ�*/
# define levmax 3                /*����������Ƕ����������[0��lexmax]*/
# define cxmax 200               /*���������������*/
/*����*/
enum symbol{
	 nul,    ident,     number,     plus,     minus,
	 times,   slash,   oddsym,   eql,     neq,//slash?? times?? 
lss,     leq,      gtr,     geq,     lparen,
rparen,  comma,   semicolon,period,  becomes,//peroid��� 
beginsym, endsym, ifsym,    thensym,  whilesym,//����if then����������� 
writesym, readsym,  dosym,  callsym,  constsym,
varsym,  procsym,lquotation,colon,arraysym,
leftbracket,rightbracket,forsym,stepsym,untilsym,
//���������ţ������� ð�ţ������������� 
//����������  forѭ������ 
//��������Ĵ��� leftbracket ������ rightbracket ������ 
};
#define symnum 40
/*-------------*/
enum object{  //���ֱ�ʶ��������
   constant,
   variable,
   procedur,
   array, 
};
/*--------------*/
enum fct{//fct���ͷֱ��ʶ��PCODE�ĸ���ָ��
lit, opr,  lod,  sto,  cal,  inte,  jmp,  jpc,outs,sto1,lod1//outs��ʾ����ַ���ָ�� sto1�洢���� 
};
//sto1�����洢����Ԫ��
//lod1������������Ԫ�� 
#define fctnum 12
/*--------------*/
struct instruction//ָ��
{
  enum fct f;//������
  int l;//��β�
  float a;//a�ڲ�ͬ��ָ���б�ʾ��ͬ�ĺ���
};

FILE * fas;//������ֱ�
FILE * fa;//������������
FILE * fa1;//���Դ�ļ�������ж�Ӧ���׵�ַ
FILE * fa2;//������

bool tableswitch;//��ʾ���ֱ����
bool listswitch;//��ʾ������������
char ch;//��ȡ�ַ��Ļ�������getchʹ��
enum symbol sym;//��ǰ����
char id[al+1];//��ǰ��ident����һ���ֽ����ڴ��0
char st[255]="";//���ڱ���write����������ַ���  
int start=0;//������¼���ű����͵���ʼλ�� 
int endnum=0;//������¼���ű����͵���ֹλ�� 
int  num;//��ǰnumber integer�� 
float realnum;//��ǰreal�� 
int cc,ll;//getchʹ�õļ�������cc��ʾ��ǰ�ַ�(ch)��λ��
int cx;//���������ָ��
char line[81];//��ȡ�л�����
char a[al+1];//��ʱ����
struct instruction code[cxmax];//�����������������
char word[norw][al];//������
enum symbol wsym[norw];//�����ֶ�Ӧ�ķ���ֵ
enum symbol ssym[256];//���ַ��ķ���ֵ
char mnemonic[fctnum][5];//���������ָ������
bool declbegsys[symnum];//��ʾ��ʼ�ķ��ż�,declaring begin symbol set
bool statbegsys[symnum];//��ʾ��俪ʼ�ķ��ż��� statment
bool facbegsys[symnum];//��ʾ���ӿ�ʼ�ķ��ż��� factor
int low=0;//������½� 
int high=0; //������Ͻ� 
//*************************
float arraystore[amax];//�漰��������������ͳһ��
int arraybound[amax];//�����洢������Ͻ磬�洢��λ��Ϊ����Ļ���ַ 
int con=0;//������α� 
//int arrayadr[amax];//��ȡ�����ַ������ջ ---���� 
//int adrcon=0;//��ַ�α� 
int readflag=0;//���ö�ȡ������� 

//*************************
int tailx=0;//���ֱ��βָ�� 
enum numtype{
	real,
	integer,	
};
enum numtype intorreal;//��ʾ��ǰ��ȡ����������--���ڸ�ֵʱʹ�� 
struct tablestruct
{
   char name[al];                            /*����*/
   enum object kind;                         /*���ͣ�const��var��array or procedure*/
   int val;                                  /*��ֵ����constʹ��*/
   int level;                                /*�����㣬��const��ʹ��*/
   int adr;                                  /*��ַ����const��ʹ��*/
   int size;                                 /*��Ҫ������������ռ䣬��procedureʹ��*/
   float realnumber;
   enum numtype tp;//���ݵ����� 
   int base;//base���ڴ������Ļ���ַ 
   int low;
   int high; 
};
char ss[amax];  /*�ַ���ջ*/ 
int stail=0;//�ַ���ջ��βָ�� 
struct tablestruct table[txmax];             /*���ֱ�*/
FILE * fin;
FILE* fout;
char fname[al];
int err;                                       /*���������*/
/*�������лᷢ��fatal errorʱ�����أ�1��֪�������ĺ����������˳�����*/
#define getsymdo                              if(-1==getsym())return -1
#define getchdo                               if(-1==getch())return -1
#define testdo(a,b,c)                         if(-1==test(a,b,c))return -1
#define gendo(a,b,c)                          if(-1==gen(a,b,c))return -1
#define expressiondo(a,b,c)                   if(-1==expression(a,b,c))return -1
#define factordo(a,b,c)                       if(-1==factor(a,b,c))return -1
#define termdo(a,b,c)                         if(-1==term(a,b,c))return -1
#define conditiondo(a,b,c)                    if(-1==condition(a,b,c))return -1
#define statementdo(a,b,c)                    if(-1==statement(a,b,c))return -1
#define constdeclarationdo(a,b,c)             if(-1==constdeclaration(a,b,c))return -1
#define vardeclarationdo(a,b,c)               if(-1==vardeclaration(a,b,c))return -1
void error(int n);
int getsym();
int getch();
void init();//��ʼ������
int gen(enum fct x,int y,int z);
int test(bool*s1,bool*s2,int n);
int inset(int e,bool*s);
int addset(bool*sr,bool*s1,bool*s2,int n);
int subset(bool*sr,bool*s1,bool*s2,int n);
int mulset(bool*sr,bool*s1,bool*s2,int n);
int block(int lev,int tx,bool* fsys);
void interpret();
int factor(bool* fsys,int* ptx,int lev);
int term(bool*fsys,int*ptx,int lev);
int condition(bool*fsys,int*ptx,int lev);
int forcondition(bool*fsys,int*ptx,int lev,int p,int cx3);
int expression(bool*fsys,int*ptx,int lev);
int statement(bool*fsys,int*ptx,int lev);
void listcode(int cx0);
int vardeclaration(int* ptx,int lev, int* pdx);
int constdeclaration(int* ptx,int lev, int* pdx);
int position(char* idt,int tx);
void enter(enum object k,int* ptx,int lev,int* pdx);
int base(int l,float* s,int b);
int getarraybound(int &loworhigh,int *ptx);//��ȡ��������½纯�� 
int dealarray(bool*fsys,int*ptx,int lev); 
 

        








