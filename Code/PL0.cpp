//A.2     C      版   本

/*编译和运行环境:
*1Visual C++6.0,VisualC++.NET and Visual C++.NET 2003
*WinNT, Win 200, WinXP and  Win2003 
*2 gcc version 3.3.2  20031022(Red Hat Linux 3.3.2-1)
*Redhat Fedora core 1
*Intel 32 platform
*使用方法:
*运行后输入PL/0 源程序文件名
*回答是否输出虚拟机代码
*回答是否输出名字表
*fa.tmp 输出虚拟机代码
*fa1.tmp  输出源文件及其各行对应的首地址
*fa2.tmp  输出结果 
*fas.tmp  输出名字表
*/
#include<stdio.h>
#include<stdlib.h>
#include"pl0.h"
#include"string.h"
/*解释执行时使用的栈*/
#define stacksize 500
int main()
{
	bool nxtlev[symnum];
	printf("Input pl/0 file ?");
	scanf("%s",fname);                                     /*输入文件名*/
	fin=fopen(fname,"r");
	if(fin)
	{
		//初始化数组栈
		memset(arraystore,0,sizeof(arraystore)); 
		printf("List object code ?(Y/N)");                /*是否输出虚拟机代码*/
		scanf("%s",fname);
		listswitch=(fname[0]=='y'||fname[0]=='Y');
		printf("List symbol table ? (Y/N)");             /*是否输出名字表*/
		scanf("%s",fname);
		tableswitch=(fname[0]=='y'||fname[0]=='Y');
		fa1=fopen("fa1.tmp","w");//输出源文件 
		fprintf(fa1,"Iput pl/0 file ?");
		fprintf(fa1,"%s\n", fname);
		init();                                          /*初始化*/
		err=0;
		cc=cx=ll=0;
		ch=' ';
		if(-1!=getsym())
		{
			fa=fopen("fa.tmp","w");
			fas=fopen("fas.tmp","w");
			addset(nxtlev,declbegsys,statbegsys,symnum);
			nxtlev[period]=true;    //句号 
			if(-1==block(0,0,nxtlev))	//参数分别为 level	/*lev:当前分程序所在层// *tx:名字表当前尾指针//*fsys:当前模块后跟符号集合/*调用编译程序*/
			{
				fclose(fa);
				fclose(fa1);
				fclose(fas);
				fclose(fin);
				printf("\n");
				return 0;
			}
			fclose(fa);
			fclose(fa1);
			fclose(fas);
			if(sym!=period)
			{
				error(9);
			}
			if(err==0)
			{
				fa2=fopen("fa2.tmp", "w");
				interpret();
				fclose(fa2);
			}
			else
			{
				printf("Errors in pl/0 program");
			}
		}
		fclose(fin);
	}
	else
	{
		printf("Can't open file! \n");
	}
	printf("\n");
	return 0;
}
/*
*初始化
*/
void init()
{
	int i;
	for(i=0;i<=255;i++)
	{
		ssym[i]=nul;
	}
	//单字符的说明 
	ssym['+']=plus;
	ssym['-']=minus;
	ssym['*']=times;
	ssym['/']=slash;
	ssym['(']=lparen;
	ssym[')']=rparen;
	ssym['=']=eql;
	ssym[',']=comma;
	ssym['.']=period;
	ssym['#']=neq;
	ssym[';']=semicolon;
	ssym['\'']=lquotation;//加入左右引号
	 
	
	/*设置保留字名字,按照字母顺序,便于折半查找*/
	strcpy(&(word[0][0]),"array");
	strcpy(&(word[1][0]),"begin");
	strcpy(&(word[2][0]),"call");
	strcpy(&(word[3][0]),"const");
	strcpy(&(word[4][0]),"do");
	strcpy(&(word[5][0]),"end");
	strcpy(&(word[6][0]),"for");
	strcpy(&(word[7][0]),"if");
	strcpy(&(word[8][0]),"odd");
	strcpy(&(word[9][0]),"procedure");
	strcpy(&(word[10][0]),"read");
	strcpy(&(word[11][0]),"step");
	strcpy(&(word[12][0]),"then");
	strcpy(&(word[13][0]),"until");
	strcpy(&(word[14][0]),"var");
	strcpy(&(word[15][0]),"while");
	strcpy(&(word[16][0]),"write");	
	
	
	/*设置保留字符号*/
	wsym[0]=arraysym;
	wsym[1]=beginsym;
	wsym[2]=callsym;
	wsym[3]=constsym;
	wsym[4]=dosym;
	wsym[5]=endsym;
	wsym[6]=forsym;
	wsym[7]=ifsym;
	wsym[8]=oddsym;
	wsym[9]=procsym;
	wsym[10]=readsym;
	wsym[11]=stepsym;
	wsym[12]=thensym;
	wsym[13]=untilsym;
	wsym[14]=varsym;
	wsym[15]=whilesym;
	wsym[16]=writesym;	

	/*设置指令名称*/
	strcpy(&(mnemonic[lit][0]),"lit");
	strcpy(&(mnemonic[opr][0]),"opr");
	strcpy(&(mnemonic[lod][0]),"lod");
	strcpy(&(mnemonic[sto][0]),"sto");
	strcpy(&(mnemonic[cal][0]),"cal");
	strcpy(&(mnemonic[inte][0]),"int");
	strcpy(&(mnemonic[jmp][0]),"jmp");
	strcpy(&(mnemonic[jpc][0]),"jpc");
	strcpy(&(mnemonic[outs][0]),"outs");//输出字符串指令 
	strcpy(&(mnemonic[sto1][0]),"sto1");//输出字符串指令 
	strcpy(&(mnemonic[lod1][0]),"lod1");//输出字符串指令 
	/*设置符号集*/
	for(i=0;i<symnum;i++)
	{
		declbegsys[i]=false;
		statbegsys[i]=false;
		facbegsys[i]=false;
	}

	/*设置声明开始符号集*/
	declbegsys[constsym]=true;
	declbegsys[varsym]=true;
	declbegsys[procsym]=true;
	/*设置语句开始符号集*/
	statbegsys[beginsym]=true;
	statbegsys[callsym]=true;
	statbegsys[ifsym]=true;
	statbegsys[whilesym]=true;
	/*设置因子开始符号集*/
	facbegsys[ident]=true;
	facbegsys[number]=true;
	facbegsys[lparen]=true;
	facbegsys[leftbracket]=true;
}
 /*
  *用数组实现集合的集合运算
  */
int inset(int e,bool* s)
{
    return s[e];
}
int addset(bool* sr,bool* s1,bool* s2,int n)//逻辑或运算 
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]||s2[i];  
    }
    return 0;
}
int subset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]&&(!s2[i]);
    }
    return 0; 
}
int mulset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]&&s2[i];  
    } 
    return 0;
}
/*
 *出错处理，打印出错位置和错误编码
 */
void error(int n)
 
{
	char space[81];
	memset(space,32,81); printf("-------%c\n",ch);
	space[cc-1]=0;//出错时当前符号已经读完，所以cc-1
	printf("****%s!%d\n",space,n);
	err++;
}
/*
 *  漏掉空格，读取一个字符
 *
 *  每次读一行，存入line缓冲区，line被getsym取空后再读一行
 *
 *  被函数getsym调用
 */
int getch()
{
	if(cc==ll)//刚初始化完后cc,cx,ll均为0 读取完一行之后ll变为一行的末端，cc为一行的首端 
    {
		if(feof(fin))//判断文件是否结束 
		{
			printf("program incomplete");
			return -1;
		}
		ll=0;
		cc=0;
		printf("%d ",cx );
		fprintf(fa1,"%d ",cx);
		ch=' ';
		while(ch!=10)//10表示换行符 
		{
			//fscanf(fin,"%c",&ch)
            if(EOF==fscanf(fin,"%c",&ch))
			{
				line[ll]=0;
				break;
			}
			printf("%c",ch);
			fprintf(fa1,"%c",ch);
			line[ll]=ch;
			ll++;
		}
		printf("\n");
		fprintf(fa1,"\n");
	}
	ch=line[cc];
	cc++;
	return 0;
}
 /*词法分析，获取一个符号
 */

int getsym()
{
	int i,j,k;
	while( ch==' '||ch==10||ch==9)//9是水平制表符 
	{
		getchdo;//说明此时未读取行，需要调用getch()函数 
	}
	if(ch>='a'&&ch<='z')//fibonacci的第一行读完为ch=c 
	{ 
		k=0;
		do{
			if(k<al)//判断是否超出最大单词长度10 
			{ 
				a[k]=ch;
				k++;
			}
			getchdo; 
		}while(ch>='a'&&ch<='z'||ch>='0'&&ch<='9');
		a[k]=0;//字符串的结束标志 
		strcpy(id,a);//拷贝单词 
		i=0;
		j=norw-1;
		do{//判断是否是关键字程序 
			k=(i+j)/2;
			if(strcmp(id,word[k])<=0)
			{
				j=k-1;
			}
			if(strcmp(id,word[k])>=0)
			{
				i=k+1;
			}
		}while(i<=j);//二分法关键字对比 
		if(i-1>j)
		{
			sym=wsym[k];
		}
		else
		{
			sym=ident;
		}
	}
	else
	{
		if(ch>='0'&&ch<='9')
		{
			if(intorreal==integer){//如果是变量的赋值，则先有intorreal，如是常量，则intorreal为默认 
				k=0;
				num=0;
				sym=number;
				do{
					num=10*num+ch-'0';
					k++;
					getchdo;
				}while(ch>='0'&&ch<='9'); /*获取数字的值*/
				k--;
				if(k>nmax)
				{
					error(30);
				}
			}else if(intorreal==real){
				char floatnum[20];//用于存放浮点数
				k=0;
				sym=number;
				realnum=0;
				do{
					floatnum[k]=ch; 
					k++;
					getchdo;
				}while((ch>='0'&&ch<='9')||(ch=='.')); /*获取数字的值  :用于常量的赋值 )用于检测输出的表达式*/
				floatnum[k]='\0';//ch!=';'&& ch!=':'&&ch!=','&&ch!=')'
				realnum=atof(floatnum);
			} 			
		}
		else
		{
			if(ch==':')             /*检测赋值符号*/
			{
				getchdo;
				if(ch=='=')
				{
					sym=becomes;
					getchdo;
				}
				else
				{
					sym=colon;        //定义类型的符号--改    /*不能识别的符号*/
				}
			}
			else if(ch=='['){//直接读取数组的上下界 
				sym=leftbracket;				
			}	
			else if(ch==']'){
				sym=rightbracket;
				getchdo;
			}		
			else{
				if(ch=='<')         /*检测小于或小于等于符号*/
				{
					getchdo;
					if(ch=='=')
					{
						sym=leq;
						getchdo;
					}
					else
					{ 
						sym=lss;
					}
				}
				else
				{
					if(ch=='>')          /*检测大于或大于等于符号*/
					{
						getchdo;
						if(ch=='=')
						{
							sym=geq;
							getchdo;
						}
						else
						{
						    sym=gtr;
						}
					}
					else
					{
						sym=ssym[ch];/* 当符号不满足上述条件时，全部按照单字符号处理*/
						//getchdo;
						//richard
						if(sym!=period)
						{
							getchdo;
						}
						//end richard
					}
				}
			}
		}
	}
	return 0;
}
int getsym2(char as[]){//专门用于读取字符串 
	int p=0;
	for(p=0;;p++){		
		if(ch!='\''){
			as[p]=ch;	
		}else{
			break;
		}
		getchdo;
	} 
	as[p]='\0';
}
int getarraybound(int &loworhigh,int *ptx){
	int k=0;
	getchdo;// 读取上界 
	if(ch>='a'&&ch<='z'){//标识符 
	    sym=ident;	 
		k=0;
		do{
			if(k<al)//判断是否超出最大单词长度10 
			{ 
				a[k]=ch;
				k++;
			}
			getchdo; 
		}while(ch>='a'&&ch<='z'||ch>='0'&&ch<='9');
		a[k]=0;//字符串的结束标志 
		strcpy(id,a);//拷贝单词 
		int p=position(id,*ptx);
		if(p==0){
			error(11);//标识符未说明 
		}else{
			loworhigh=table[p].val;	
		} 
			
	}else{
		num=0;
		sym=number;
		do{
			num=10*num+ch-'0';
			k++;
			getchdo;
		}while(ch>='0'&&ch<='9'); /*获取数字的值*/
		k--;
		if(k>nmax)
		{
			error(30);
		}else{
			loworhigh=num;	
		}
		
	}	
	if(ch==']'){
		sym=rightbracket;
	} 
}
/*
*生成虚拟机代码
*
*x:instruction.f;
*y:instruction.l;
*z:instruction.a;
*/
int gen(enum fct x,int y,int z)//gendo(jmp,0,a); jmp无条件跳转 地址a 此时为0 
{
	if(cx>=cxmax)//cxmax最大虚拟机代码数为200 cx初始为0 
	{
		printf("Program too long"); /*程序过长*/
		return -1;
	}
	code[cx].f=x;//存放虚拟机的指令功能码 
	code[cx].l=y;//层数 
	code[cx].a=z;// 赋值 
	cx++;//加一条指令 
	return 0;
}
//生成存取立即数的指令 
int gen2(enum fct x,int y,float z)
{
	if(cx>=cxmax)//cxmax最大虚拟机代码数为200 cx初始为0 
	{
		printf("Program too long"); /*程序过长*/
		return -1;
	}
	code[cx].f=x;//存放虚拟机的指令功能码 
	code[cx].l=y;//层数 
	code[cx].a=z;// 赋值 
	cx++;//加一条指令 
	return 0;
}
/*
*测试当前符号是否合法
*
*在某一部分（如一条语句，一个表达式）将要结束时时我们希望下一个符号属于某集合
*（该部分的后跟符号） test 负责这项检测，并且负责当检测不通过时的补救措施
*程序在需要检测时指定当前需要的符号集合和补救用的集合（如之前未完成部分的后跟
*符号），以及不通过时的错误号
*
*S1：我们需要的符号  beginsym
*s2:如果不是我们需要的，则需要一个补救用的集合 endsym
*n:错误号
*/


int test(bool* s1,bool* s2,int n)//testdo(nxtlev,declbegsys,7) 
{
    if(! inset(sym,s1))
    {
		error(n);
		/*当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合*/
		while((! inset(sym,s1))&&(! inset(sym,s2)))
		{
			getsymdo; 
		}
    }
    return 0;
}
/*
 *编译程序主体
 *
 *lev:当前分程序所在层
 *tx:名字表当前尾指针
 *fsys:当前模块后跟符号集合
 */
int block(int lev,int tx,bool* fsys)
{
    int i;
    int dx;                         /*名字分配到的相对地址*/
    int tx0;                        /*保留初始tx 名字表 */
    int cx0;                        /*保留初始cx cx为虚拟机代码指针  此时还未用 即fa.tmp*/
    bool nxtlev[symnum];            /*在下级函数的参数中，符号集合均为值参，但由于使用数组
                                      实现，传递进来的是指针，为防止下级函数改变上级函数的
                                      集合，开辟新的空间传递给下级函数*/
    dx=3;							//编译书上说的起始为3
    tx0=tx;                         /*记录本层名字的初始位置   开始为0*/
    table[tx].adr=cx;
    gendo(jmp,0,0);
    if(lev > levmax)//最大嵌套层数为3 
    {
		error(32);                                                                                                                                         
    }
    do{
        if(sym==constsym)         /*收到常量声明符号，开始处理常量声明*/
        {
			getsymdo;
			do{
                constdeclarationdo(&tx,lev,&dx);   /*dx的值会被constdeclaration改变，使用指针*/
                start=tx;                                   
                while(sym==comma)
                {
                   getsymdo;
                   constdeclarationdo(&tx,lev,&dx);
                }
                endnum=tx;
                if(sym==colon){//常量类型处理 	(如整型，浮点型等)	如果无，则按照浮点类型处理			
					getsymdo;
					if(!strcmp(id,"real")){
						enum numtype ob=real;
						for(int s=start;s<=endnum;s++){
							table[s].tp=ob;//real类型的数据 
						}
					}else if(!strcmp(id,"integer")){//整型数据 
						enum numtype ob=integer;
						for(int s=start;s<=endnum;s++){
							table[s].tp=ob;
							table[s].val=int(table[s].realnumber);
						}
					}
					getsymdo;				
				}else if(sym==semicolon){
					for(int s=start;s<=endnum;s++){
						enum numtype ob=integer;
						table[s].tp=ob;		
						table[s].val=int(table[s].realnumber);				
					}
				} 
                if(sym==semicolon)
                {
                  getsymdo;
				}				
				else
				{
					error(5); /*漏掉了逗号或者分号*/
				}
			}while(sym==ident);
		}
		if(sym==varsym)/*收到变量声名符号，开始处理变量声名*/
		{
			getsymdo;
			do{
				vardeclarationdo(&tx,lev,&dx);
				start=tx;
				while(sym==comma)
				{
					getsymdo;
					vardeclarationdo(&tx,lev,&dx);
				}
				endnum=tx;
				if(sym==colon){
					getsymdo;
					if(!strcmp(id,"real")){//浮点数类型的处理 
						enum numtype ob=real;
						for(int s=start;s<=endnum;s++){
							table[s].tp=ob;
						}
					}else if(!strcmp(id,"integer")){//整数类型的处理 
						enum numtype ob=integer;
						for(int s=start;s<=endnum;s++){
							table[s].tp=ob;
						}
					}else if(sym==arraysym){//数组类型的处理 !strcmp(id,"array") 进行数组系统的处理 
						getsymdo; 
						if(sym==leftbracket){//在getsymdo里已经完成了对数组上下界的分析 
							//对数组上下界的处理 
							getarraybound(low,&tx);					
							getchdo;//读取. 
							//注意三点的错误处理 
							getarraybound(high,&tx);
							if(high>=low){
								getsymdo; //判断完right 现在ch=" " //考虑加错误处理 
								getsymdo; //读取of							 
								if(!strcmp(id,"of")){
									getsymdo;//读取数组的类型 
									if(!strcmp(id,"real")){//浮点数类型的处理 
										enum numtype ob=real;
										for(int s=start;s<=endnum;s++){
											table[s].tp=ob;
											table[s].kind=array;//种类为数组 
											table[s].low=low;//设置数组的上下界 
											table[s].high=high;
											table[s].base=con;
											table[s].size=(high-low+1);
											arraybound[con]=high; 											
											con+=(high-low+1); 
										}
									}else if(!strcmp(id,"integer")){//整数类型的处理 
										enum numtype ob=integer;
										for(int s=start;s<=endnum;s++){
											table[s].tp=ob;
											table[s].kind=array;//种类为数组 
											table[s].low=low;
											table[s].high=high;
											table[s].base=con;
											table[s].size=(high-low+1);
											arraybound[con]=high; 										
											con+=(high-low+1); 
										}
									}
								}else{
									error(36);//缺少of 
								}					
							}else{
								//上下界的检查,如果检查不通过，报错同时tx-1
								tx-=1; 
								error(38);
							}									 
						}else{
							//错误处理
							error(35);//缺少左括号 
						}						
					}
					getsymdo;
				}
				else if(sym==semicolon){
					for(int s=start;s<=endnum;s++){
						enum numtype ob=integer;
						table[s].tp=ob;						
					}
				} 
				if(sym==semicolon||sym==comma)
				{
					getsymdo;
				}
				else
				{
					error(5);
				}
			}while(sym==ident);
		}
		while(sym==procsym)/*收到过程声名符号，开始处理过程声名*/
		{
			getsymdo;
			if(sym==ident)
			{
				enter(procedur,&tx,lev,&dx);/*记录过程名字*/
				getsymdo;
			}
			else
			{
				error(4);/*procedure后应为标识符*/
			}
			if(sym==semicolon)
			{
				getsymdo;
			}
			else
			{
				error(5);/*漏掉了分号*/
			}
			memcpy(nxtlev,fsys,sizeof(bool)*symnum);
			nxtlev[semicolon]=true;
			if(-1==block(lev+1,tx,nxtlev))
			{
				return -1;/*递归调用*/
			}
            if(sym==semicolon)
            {
                getsymdo;
                memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
                nxtlev[ident]=true;
                nxtlev[procsym]=true;
                testdo(nxtlev,fsys,6);
             }
             else
             {
                 error(5);                       /*漏掉了分号*/
             }
        }
        memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
        nxtlev[ident]=true;
        nxtlev[period]=true;
        nxtlev[varsym]=true; 
        nxtlev[constsym]=true;
        testdo(nxtlev,declbegsys,7);//退出声明语法单位 
    }while(inset(sym,declbegsys));                /*直到没有声明符号*/
    code[table[tx0].adr].a=cx;      //存放虚拟机代码  ?????            /*开始生成当前过程代码*/
    table[tx0].adr=cx;                            /*当前过程代码地址*/
    table[tx0].size=dx;                           /*声明部分中每增加一条声明都会给dx增加1,声明部分已经结束,dx就是当前过程数据的size*/
        
    cx0=cx; 
    gendo(inte,0,dx);                             /*生成分配内存代码*/
    if(tableswitch)                               /*输出名字表*/
    {
        printf("TABLE:\n");
        if(tx0+1>tx)
        {
			printf("NULL\n");
        }
        for(i=tx0+1;i<=tx;i++)
        {
            switch(table[i].kind)
            {
                case constant:
					printf("%d const %s ",i,table[i].name);
					printf("val=%d\n",table[i].val);
					fprintf(fas,"%d const %s ",i,table[i].name);
					fprintf(fas,"val=%d\n",table[i].val);
                    break;
                case variable://可以增加类型的输出 
                    printf("%d var %s ",i,table[i].name);
                    printf("lev=%d addr=%d\n",table[i].level,table[i].adr);
                    fprintf(fas,"%d var %s",i,table[i].name);
                    fprintf(fas,"lev=%d addr=%d\n",table[i].level,table[i].adr);
                    break;
                case procedur:
                    printf("%d proc %s ",i,table[i].name);
                    printf("lev=%d addr=%d size=%d\n",table[i].level,table[i].adr,table[i].size);
					fprintf(fas,"%d proc %s",i,table[i].name);
					fprintf(fas,"lev=%d adr=%d size=%d\n",table[i].level,table[i].adr,table[i].size);
					break;
				case array:
					printf("%d array %s ",i,table[i].name);
                    printf("lev=%d addr=%d size=%d low=%d  high=%d\n",table[i].level,table[i].adr,table[i].size,table[i].low,table[i].high);
					fprintf(fas,"%d array %s ",i,table[i].name);
					fprintf(fas,"lev=%d adr=%d size=%d \n",table[i].level,table[i].adr,table[i].size);
					break;
			}
		}
		printf("\n");
	}
	/*语句后跟符号为分号或end*/
	memcpy(nxtlev,fsys,sizeof(bool)*symnum);/*每个后跟符号集和都包含上层后跟符号集和，以便补救*/
	nxtlev[semicolon]=true;
	nxtlev[endsym]=true;
	statementdo(nxtlev,&tx,lev);
	gendo(opr,0,0); /*每个过程出口都要使用的释放数据段命令*/
	memset(nxtlev,0,sizeof(bool)*symnum); /*分程序没有补救集合*/
	test(fsys,nxtlev,8);                  /*检测后跟符号正确性*/
	listcode(cx0);                        /*输出代码*/
	return 0;
}
/*
*在名字表中加入一项
*
*k:名字种类const,var or procedure
*ptx:名字表尾指针的指针，为了可以改变名字表尾指针的数值
*lev:名字所在的层次，以后所有的lev都是这样
*pdx:为当前应分配的变量的相对地址，分配后要增加1
*/
void enter (enum object k,int *ptx,int lev, int *pdx)
{
	(*ptx)++;	 
	strcpy(table[(*ptx)].name,id);       /*全局变量id中已存有当前名字的名字*/
	table[(*ptx)].kind=k;
	switch(k)
	{
		case constant:                      /*常量名字*/
			if (num>amax)
			{
				error(31);
				num=0;
			}			 
			table[(*ptx)].realnumber=realnum;
			break;
		case variable:                     /*变量名字*/
			table[(*ptx)].level=lev;
			table[(*ptx)].adr=(*pdx);
			(*pdx)++;
			break;                          /*过程名字*/
		case procedur:
			table[(*ptx)].level=lev;
			break;
	}

}
/*
 *查找名字的位置
 *找到则返回在名字表中的位置，否则返回0
 *
 *idt: 要查找的名字
 *tx:：当前名字表尾指针
 */
int position(char *  idt,int  tx)
{
	int i;
	strcpy(table[0].name,idt);
	i=tx;
	while(strcmp(table[i].name,idt)!=0)
	{
		i--;
	}
	intorreal=table[i].tp;//当前所取数据的类型 
	return i;
}
/*
 *常量声明处理
 */
int constdeclaration(int *  ptx,int lev,int *  pdx)
{
	if(sym==ident)//常量申明必须要是标识符 
	{
		getsymdo;
		if(sym==eql ||sym==becomes)//becomes是赋值符号 
		{
			if(sym==becomes)
			{
				error(1);                     /*把=写出成了：=*/
			}
			getsymdo;
			if(sym==number)
			{
				enter(constant,ptx,lev,pdx);
				getsymdo;
			}
			else
			{
				error(2);                  /*常量说明=后应是数字*/
			}         
		}
		else
		{
			error(3);                       /*常量说明标识后应是=*/
		}
	}
	else
	{
		error(4);                        /*const后应是标识*/
	}
	return 0;
}
/*
 *
 */
int vardeclaration(int * ptx,int lev,int * pdx)
{
	if(sym==ident)
 	{
 		enter(variable,ptx,lev,pdx);//填写名字表
 		getsymdo;
 	}
 	else
 	{
 		error(4);
    }
    return 0;
}
 /*
  *输入目标代码清单
  */
void listcode(int cx0)
{
	int i;
   	if (listswitch)
   	{
   		for(i=cx0;i<cx;i++)
   	 	{
   	 		if(code[i].a==int(code[i].a)){
   	 			printf("%d %s %d %d\n",i,mnemonic[code[i].f],code[i].l,int(code[i].a));
   	 			fprintf(fa,"%d %s %d %d\n",i,mnemonic[code[i].f],code[i].l,int(code[i].a));
			}else{
				printf("%d %s %d %f\n",i,mnemonic[code[i].f],code[i].l,code[i].a);
   	 			fprintf(fa,"%d %s %d %f\n",i,mnemonic[code[i].f],code[i].l,code[i].a);
			}
   	    }
   	 }
}
/*
*语句处理
*/
int statement(bool* fsys,int * ptx,int lev)
{
	int i,cx1,cx2;
   	bool nxtlev[symnum];
   	int arraypos=0;//保存数组的位置,取基地址的时候用 
   	if(sym==ident)
   	{
		i=position(id,*ptx); 
   	 	if(i==0)
   	 	{
   	 		error(11);
   	 	}
   	 	else
   	 	{
   	 		if(table[i].kind!=variable&&table[i].kind!=array)
   	 		{
   	 		 	error(12);
   	 		 	i=0;
   	 		}
   	 		else if(table[i].kind==array){
   	 			arraypos=i;
   	 			//对数组元素的处理 
   	 			getsymdo;//获取左[
				if(sym==leftbracket){					
					getchdo;
					getsymdo;
					memcpy(nxtlev,fsys,sizeof(bool)* symnum);	
					expressiondo(nxtlev,ptx,lev);//存储立即数 
					if(sym==rightbracket){
						getsymdo;
		   	 			if(sym==becomes){		   	 			
		   	 				getsymdo;
		   	 			}else{
		   	 			 	error(13);
		   	 			}		
						i=position(id,*ptx);					
						memcpy(nxtlev,fsys,sizeof(bool)* symnum);
						dealarray(nxtlev,ptx,lev); //处理右边的表达式
//						if(table[i].kind==array){//调用递归处理array 
//		   	 				//处理右边的表达式 
//		   	 				dealarray(nxtlev,ptx,lev); 		   	 				
//						}else{							
//		   	 				expressiondo(nxtlev,ptx,lev);//存储立即数 
//						}	   	 					   	 			
						if(i!=0){
							gendo(sto1,table[i].low,table[arraypos].base);
							//存储赋值数组  arraypos为左边数组的位置 
						}
					}else{
						error(35);
					}
				} else{
					error(35);
				}
			}
   	 		else
   	 		{
   	 			getsymdo;
   	 			if(sym==becomes)
   	 			{
   	 				getsymdo;
   	 			}
   	 			else
   	 			{
   	 			 	error(13);
   	 			}
   	 			//处理b:=a[1] 的这种赋值类型    	 			 
   	 			memcpy(nxtlev,fsys,sizeof(bool)* symnum);
   	 			if(sym==ident){
   	 				dealarray(nxtlev,ptx,lev);//处理右边赋值数组的表达式 
				}else{
					expressiondo(nxtlev,ptx,lev);//存储立即数 					
				}   	 			
   	 			if(i!=0)
   	 			{
   	 			 	gendo(sto,lev-table[i].level,table[i].adr);//将立即数赋值给对应的变量 
   	 			}
   	 	    }
   		}
    }
    else
    {
    	if(sym==readsym)
    	{
    	 	getsymdo;
    	 	if(sym!=lparen)
    	 	{
    	 	   error(34);
			}
    	    else
			{
    	   		do{
    	   			getsymdo;
    	   	   		if(sym==ident)
    	   	   		{
    	   	   	  		i=position(id, *ptx);
    	   	   		}
    	   	   		else
    	   	   		{
    	   	   	  		i=0;
    	   	   		}
    	   	   		if(i==0)
    	   	   		{
    	   	   	 		error(35);
    	   	   		}else if(table[i].kind==array){//处理数组的读入 
    	   	   		    //先处理地址
    	   	   		    readflag=1;//表示读取数组 
						dealarray(nxtlev,ptx,lev);  
    	   	   			gendo(opr,0,16);//值     	   	   			
    	   	   			//将读入的数存储到数组里 
    	   	   			gendo(sto1,table[i].low,table[i].base);
				    } 
    	   	   		else
    	   	   		{
    	   	   			gendo(opr,0,16);
						gendo(sto,lev-table[i].level,table[i].adr);	/* 储存到变量*/
						getsymdo;
					}
					
				}while (sym==comma);	/*一条read语句可读多个变量 */
				readflag=0; 
			}
			if(sym!=rparen)
			{
				error(33);			/* 格式错误，应是右括号*/
				while(!inset(sym,fsys))/* 出错补救，直到收到上层函数的后跟符号*/
				{
					getsymdo;
				}
			}
			else
			{
				getsymdo;
			}
		}
		else
		{
			if(sym==writesym)			/* 准备按照write语句处理，与read类似*/
			{
				getsymdo;
				if(sym==lparen)
				{			
					do{
						getsymdo;
						int p=position(id,*ptx);
						if(table[p].kind==array&&sym==ident){
							dealarray(nxtlev,ptx,lev); 
							if(table[p].tp==integer){
								gendo(opr,0,17);/* 生成输出指令，输出栈顶的值*/	
							}else if(table[p].tp==real){
								gendo(opr,0,14);/* 生成输出指令，输出栈顶的值*/	
							}
						}else if(sym==lquotation){//处理字符串的输出 									
							getsym2(st);//取出字符串 
							int s=stail;
							for(int p=stail;p<stail+strlen(st);p++){
								ss[p]=st[p-stail];
							}								
							stail+=strlen(st);		
							gendo(outs,s,stail);//从符号表里输出字符串 
							getsymdo;//取出' 
							getsymdo;//取出）			
						}else{//处理无字符串的输出 
							memcpy(nxtlev,fsys,sizeof(bool)*symnum);
							nxtlev[rparen]=true;
							nxtlev[comma]=true;		/* write的后跟符号为）or，*/
							nxtlev[lquotation]=true;
							expressiondo(nxtlev,ptx,lev);/* 调用表达式处理，此处与read不同，read为给变量赋值*/
							if(intorreal==integer){
								gendo(opr,0,17);/* 生成输出指令，输出栈顶的值*/	
							}else if(intorreal==real){
								gendo(opr,0,14);/* 生成输出指令，输出栈顶的值*/	
							}							
						}						
					}while(sym==comma);							
					if(sym!=rparen)
					{
						error(33);/* write()应为完整表达式*/
					}
					else
					{
						getsymdo;
					}										
				}
				gendo(opr,0,15);		/* 输出换行*/
			}
			else
			{
				if(sym==callsym)		/* 准备按照call语句处理*/
				{
					getsymdo;
					if(sym!=ident)
					{
						error(14);           /*call后应为标识符*/
					}
					else
					{
						i=position(id,*ptx);
						if(i==0)
						{
							error(11);          /*过程未找到*/
						}
						else
						{
							if(table[i].kind==procedur)
							{
								gendo(cal,lev-table[i].level,table[i].adr);  /*生成call指令*/
							}
							else
							{
								error(15);      /*call后标识符应为过程*/
							}
						}
						getsymdo;
					}
				}
				else
				{
					if(sym==ifsym)     /*准备按照if语句处理*/
					{
						getsymdo;
						memcpy(nxtlev,fsys,sizeof(bool)*symnum);
						nxtlev[thensym]=true;
						nxtlev[dosym]=true;    /*后跟符号为then或do*/
						conditiondo(nxtlev,ptx,lev);   /*调用条件处理（逻辑运算）函数*/
						if(sym==thensym)
						
						{
							getsymdo;
						}
						else
						{
							error(16);          /*缺少then*/
						}
						cx1=cx;                /*保存当前指令地址*/
						gendo(jpc,0,0);        /*生成条件跳转指令，跳转地址暂写0*/
						statementdo(fsys,ptx,lev);   /*处理then后的语句*/
						code[cx1].a=cx;      /*经statement处理后，cx为then后语句执行
                            完的位置，它正是前面未定的跳转地址*/
					}
					else
					{
						if(sym==beginsym)   /*准备按照复合语句处理*/
						{
							getsymdo;
							memcpy(nxtlev,fsys,sizeof(bool)*symnum);
							nxtlev[semicolon]=true;
							nxtlev[endsym]=true;/*后跟符号为分号或end*/
							/*循环调用语句处理函数，直到下一个符号不是语句开始符号或收到end*/
							statementdo(nxtlev,ptx,lev);
							while(inset(sym,statbegsys)||sym==semicolon)
							{
								if(sym==semicolon)
								{
									getsymdo;
								}
								else
								{
									error(10);/*缺少分号*/
								}
								statementdo(nxtlev,ptx,lev);
							}
							if(sym==endsym)
							{
								getsymdo;
							}
							else
							{
								error(17); /*缺少end或分号*/
							}
						}
						else
						{
							if(sym==whilesym)/*准备按照while语句处理*/
							{
								cx1=cx;        /*保存判断条件超作的位置*/ //cx为虚拟机指针位置 
								getsymdo;
								memcpy(nxtlev,fsys,sizeof(bool)*symnum);
								nxtlev[dosym]=true;/*后跟符号为do*/
								conditiondo(nxtlev,ptx,lev);  /*调用条件处理*/
								cx2=cx;       /*保存循环体的结束的下一个位置*/
								gendo(jpc,0,0);/*生成条件跳转，但跳出循环的地址未知*/
								if(sym==dosym)
								{
									getsymdo;
								}
								else
								{
									error(18);      /*缺少do*/
								}
								statementdo(fsys,ptx,lev); /*循环体*/
								gendo(jmp,0,cx1);/*回头重新判断条件*/
								code[cx2].a=cx;   /*反填跳出循环的地址，与if类似*/
							}else if(sym==forsym){								
								getsymdo;//得到id
								int p=position(id,*ptx); 
								statementdo(fsys,ptx,lev); /*处理i的赋值*/
								gendo(lod,lev,table[p].adr); 
								int cx3=cx;
								gendo(jmp,0,0);//跳过第一步的step 无条件跳转 
								cx1=cx;
								gendo(lod,lev,table[p].adr);
								//少了一条加上i的取值 
								/*保存判断条件超作的位置*/ //cx为虚拟机指针位置 
								//处理step和until
								if(sym!=stepsym){
									//报错 
									error(37);//缺少step 
								}else{
									getsymdo;									
									forcondition(nxtlev,ptx,lev,p,cx3);  /*调用条件处理*/
									cx2=cx;       /*保存循环体的结束的下一个位置*/
									gendo(jpc,0,0);/*生成条件跳转，但跳出循环的地址未知*/
								} 
								memcpy(nxtlev,fsys,sizeof(bool)*symnum);
								nxtlev[dosym]=true;/*后跟符号为do*/
								if(sym==dosym)
								{
									getsymdo;
								}
								else
								{
									error(18);      /*缺少do*/
								}
								statementdo(fsys,ptx,lev); /*循环体*/
								gendo(jmp,0,cx1);/*回头重新判断条件*/
								code[cx2].a=cx;   								
							}
							else
							{
								memset(nxtlev,0,sizeof(bool)*symnum);/*语句结束无补救集合*/
								testdo(fsys,nxtlev,19);/*检测语句结束的正确性*/
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

int dealarray(bool*fsys,int*ptx,int lev){
	//处理数组右边的表达式 表达式为数组开头 
	//2+a[3]的处理 ?  
	bool nxtlev[symnum];
	memcpy(nxtlev,fsys,sizeof(bool)*symnum);
	int arraypos=0;//保存数组的位置,取基地址的时候用 
	enum symbol addop;
   	if(sym==ident)
   	{
		int i=position(id,*ptx); 
   	 	if(i==0)
   	 	{
   	 		error(11);
   	 	}
   	 	else
   	 	{
   	 		if(table[i].kind==constant){//处理常量 
   	 			if(table[i].tp==real){
					gen2(lit,0,table[i].realnumber);/*浮点数生成指令*/
				}else{
					gendo(lit,0,table[i].val);     /*直接把常量的值入栈*/
				}
   	 			getsymdo;
			}
   	 		else if(table[i].kind!=variable&&table[i].kind!=array)
   	 		{
   	 		 	error(12);
   	 		 	i=0;
   	 		}
   	 		else if(table[i].kind==array){
   	 			arraypos=i;//数组名字位置，便于计算基地址 
   	 			//对数组元素的处理 
   	 			getsymdo;//获取左[
				if(sym==leftbracket){					
					getchdo;
					getsymdo;
					memcpy(nxtlev,fsys,sizeof(bool)* symnum);	
					expressiondo(nxtlev,ptx,lev);//处理[]的表达式 
					if(sym==rightbracket){
						if(i!=0&&readflag!=1){//不设置readflag，则会导致读的时候会多一条指令，进而造成不正确的赋值 
							gendo(lod1,table[i].low,table[arraypos].base);
							//存储赋值数组  arraypos为左边数组的位置 
						}
						getsymdo;
//		   	 			if(sym==becomes){		 //不考虑连等情况  	 			
//		   	 				getsymdo;
//		   	 			}else 
						if(sym==plus||sym==minus||sym==times||sym==slash){
							addop=sym; 
		   	 				getsymdo;	
							dealarray(nxtlev,ptx,lev);	
							if(addop==plus){
								gendo(opr,0,2);                    /*生成加法指令*/
							}else if(addop==minus){
								 gendo(opr,0,3);                /*生成减法指令*/
							}else if(addop==times){
								gendo(opr,0,4); 
							}else if(addop==slash){
								gendo(opr,0,5); 
							}							   	 				
						}else if(sym==semicolon||sym==comma||sym==rparen||sym==lss||sym==eql||sym==neq||sym==leq||sym==gtr||sym==geq||sym==thensym||sym==stepsym){
							//结束，不执行
							//comma是输出指令需要的
							//semicolon是分句需要的 
						}else{
		   	 			 	error(13);
		   	 			}										
						memcpy(nxtlev,fsys,sizeof(bool)* symnum);		   	 							   	 			
						
					}else{
						error(35);
					}
				} else{
					error(35);
				}
			}else if(table[i].kind==variable)
   	 		{
   	 			gendo(lod,lev-table[i].level,table[i].adr);
   	 			getsymdo;
   	 			if(sym==plus||sym==minus||sym==times||sym==slash){
					addop=sym; 
   	 				getsymdo;	
					dealarray(nxtlev,ptx,lev);	
					if(addop==plus){
						gendo(opr,0,2);                    /*生成加法指令*/
					}else if(addop==minus){
						 gendo(opr,0,3);                /*生成减法指令*/
					}else if(addop==times){
						gendo(opr,0,4); 
					}else if(addop==slash){
						gendo(opr,0,5); 
					}							   	 				
				}else if(sym==semicolon||sym==comma||sym==rparen||sym==stepsym||sym==lss||sym==eql||sym==neq||sym==leq||sym==gtr||sym==geq||sym==thensym){
					//结束，不执行
					//comma是输出指令需要的
					//semicolon是分句需要的 
				}else{
   	 			 	error(13);
   	 			}					
   	 	    }
		} 
	}else if(sym==number){//处理1+a[2] 
		if(intorreal==real){
			gen2(lit,0,realnum);/*浮点数生成指令*/					
		}else{
			gendo(lit,0,num);     /*直接把常量的值入栈*/
		} 
		getsymdo; 
		if(sym==plus||sym==minus||sym==times||sym==slash){
			addop=sym; 
			getsymdo;	
			dealarray(nxtlev,ptx,lev);	
			if(addop==plus){
				gendo(opr,0,2);                    /*生成加法指令*/
			}else if(addop==minus){
				 gendo(opr,0,3);                /*生成减法指令*/
			}else if(addop==times){
				gendo(opr,0,4); 
			}else if(addop==slash){
				gendo(opr,0,5); 
			}							   	 				
		}else if(sym==semicolon||sym==comma||sym==rparen){
			//结束，不执行
			//comma是输出指令需要的
			//semicolon是分句需要的 
		}else{
		 	error(13);
		}			
	}
}
 
/*
*表达式处理
*/
int expression(bool*fsys,int*ptx,int lev)
{
	enum symbol addop;                    /*用于保存正负号*/
	bool nxtlev[symnum];
	if(sym==plus||sym==minus)             /*开头的正负号，此时当前表达式被看作一个正的或负的项*/
	{
		addop=sym;                    /*保存开头的正负号*/
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);                /*处理项*/
		if(addop==minus)
		{
			gendo(opr,0,1);                   /*如果开头为负号生成取负指令*/
		}
	}
	else                             /*此时表达式被看作项的加减*/
	{
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;//加】？ 
		termdo(nxtlev,ptx,lev);            /*处理项*/
	}
	while(sym==plus||sym==minus)
	{
		addop=sym;
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);              /*处理项*/
		if(addop==plus)
		{
			gendo(opr,0,2);                    /*生成加法指令*/
		}
		else
		{
			 gendo(opr,0,3);                /*生成减法指令*/
		}
	}
	return 0;
}
/*
*项处理
*/
int term(bool*fsys,int *ptx,int lev)
{
    enum symbol mulop;               /*用于保存乘除法符号*/
    bool nxtlev[symnum];
    memcpy(nxtlev,fsys,sizeof(bool)*symnum) ;
    nxtlev[times]=true;
    nxtlev[slash]=true;
    factordo(nxtlev,ptx,lev);       /*处理因子*/
    while(sym==times||sym==slash)
    {
		mulop=sym;
        getsymdo;
        factordo(nxtlev,ptx,lev);
        if(mulop==times)
        {
            gendo(opr,0,4);          /*生成乘法指令*/
        }
        else
        {
            gendo(opr,0,5);           /*生成除法指令*/
        }
    }
     return 0;
}
/*
*因子处理
*/
int factor(bool*fsys,int *ptx,int lev)
 {
 	//fsys为复制过来的nxtlev 
    int i;
    bool nxtlev[symnum];
    testdo(facbegsys,fsys,24);           /*检测因子的开始符好号*/
    while(inset(sym,facbegsys))          /*循环直到不是因子开始符号*/
    {
        if(sym==ident)                   /*因子为常量或者变量*/
        {
            i=position(id,*ptx);        /*查找名字*/
            if(i==0)
            {
                error(11);               /*标识符未声明*/
            }
            else
            {
				switch(table[i].kind)
				{
					case constant:                                      /*名字为常量*/
					    if(table[i].tp==real){
					    	gen2(lit,0,table[i].realnumber);/*浮点数生成指令*/
						}else{
							gendo(lit,0,table[i].val);     /*直接把常量的值入栈*/
						}						                  
						break;
					case variable:                                      /*名字为变量*/
						gendo(lod,lev-table[i].level,table[i].adr);      /*找到变量地址并将其值入栈*/
						break;
					case procedur:                                      /*名字为过程*/
						error(21);                                       /*不能为过程*/
						break;
				}
			}
			getsymdo;
		}
		else
		{
			if(sym==number)                                             /*因子为数*/
			{
				if(num>amax)
				{
					error(31);
					num=0;
				}
				if(intorreal==integer){
					gendo(lit,0,num);
				}else{
					gen2(lit,0,realnum);//生成虚拟机代码 lit 0 realnum 
				}				
								 
				getsymdo;
			}
			else
			{
				if(sym==lparen)                                           /*因子为表达式*/
				{
					getsymdo;
					memcpy(nxtlev,fsys,sizeof(bool)*symnum);
					nxtlev[rparen]=true;
					expressiondo(nxtlev,ptx,lev);
					if(sym==rparen)
					{
						getsymdo;
					}
					else
					{
						error(22);                                       /*缺少右括号*/
					}
				}
				testdo(fsys,facbegsys,23);                        /*银子后有非法符号*/
			}
		}
	}
	return 0;
}
/*
 条件处理*/
int condition(bool* fsys,int* ptx,int lev)
{
    enum symbol relop;
    bool nxtlev[symnum];
    if(sym==oddsym)                        /*准备按照odd运算处理*/
   	{
		getsymdo;
		expressiondo(fsys,ptx,lev);
		gendo(opr,0,6);                              /*生成odd指令*/
    }
    else
    {
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[eql]=true;
		nxtlev[neq]=true;
		nxtlev[lss]=true;
		nxtlev[leq]=true;
		nxtlev[gtr]=true;
		nxtlev[geq]=true;
		dealarray(nxtlev,ptx,lev);//既可以处理数组表达式，也可以处理普通的表达式 
		if(sym!=eql&&sym!=neq&&sym!=lss&&sym!=leq&&sym!=gtr&&sym!=geq)
		{
			error(20);
		}
		else
		{
			relop=sym;
			getsymdo;
			dealarray(fsys,ptx,lev);
			switch(relop)
			{
				case eql:
					gendo(opr,0,8);
					break;
				case neq:
					gendo(opr,0,9);
					break;
				case lss:
					gendo(opr,0,10);
					break;
				case geq:
					gendo(opr,0,11);
					break;
				case gtr:
					gendo(opr,0,12);
					break;
				case leq:
					gendo(opr,0,13);
					break;
			}

		}
    }
    return 0;
}                                                  /*解释程序*/
int forcondition(bool*fsys,int*ptx,int lev,int p,int cx3){
	bool nxtlev[symnum];
    if(sym==oddsym)                        /*准备按照odd运算处理*/
   	{
		getsymdo;
		expressiondo(fsys,ptx,lev);
		gendo(opr,0,6);                              /*生成odd指令*/
    }else{
    	expressiondo(fsys,ptx,lev);//取到step 
    	gendo(opr,0,2);//对i进行相加 
    	gendo(sto,lev,table[p].adr);
    	
    	gendo(lod,lev,table[p].adr);
    	code[cx3].a=cx;
    	//放进去在拿出来 
    	if(sym!=untilsym){
    		//报错 
    		error(38); 
		}else{
			getsymdo;
			expressiondo(fsys,ptx,lev);//取until值 
			gendo(opr,0,10);//栈顶与次栈顶进行比较			 
		} 
	}
	return 0;
}
void interpret()
{
	int p,b,t;             /*指令指针，指令基址，栈顶指针*/
	struct instruction i;  /*存放当前指令*/
	float s[stacksize];      /*整数栈*/	
	printf("start pl0\n");
	t=0;//栈顶指针 
	b=0;//指令基址 
	p=0;//指令指针 
	s[0]=s[1]=s[2]=0;
	int k=0;
	do{
		i=code[p];         /*读当前指令*/
		p++;
		switch(i.f)
		{
			case lit:        /*将a的值取到栈顶*/
				s[t]=i.a;
				t++;
				break;
			case outs:	/*输出字符串指令*/
				for(k=i.l;k<int(i.a);k++){ 
					printf("%c",ss[k]);
				}
				break;
			case opr:        /*数字、逻辑运算*/
				switch(int(i.a))
				{
					case 0:
						t=b;
						p=s[t+2];
						b=s[t+1];
						break;
					case 1:
						s[t-1]=-s[t-1];
						break;
					case 2:
						t--;
						s[t-1]=s[t-1]+s[t];
						break;
					case 3:
						t--;
						s[t-1]=s[t-1]-s[t];
						break;
					case 4:
						t--;
						s[t-1]=s[t-1]*s[t];
						break;
					case 5:
						t--;
						s[t-1]=s[t-1]/s[t];
             			break;
					case 6:
						s[t-1]=int(s[t-1])%2;
						break;
					case 8:
						t--;
						s[t-1]=(s[t-1]==s[t]);
 						break;
					case 9:
						t--;
						s[t-1]=(s[t-1]!=s[t]);
 						break;
					case 10:
						t--;
						s[t-1]=(s[t-1]<s[t]);
 						break;
					case 11:
						t--;
						s[t-1]=(s[t-1]>=s[t]);
 						break;
					case 12:
						t--;
						s[t-1]=(s[t-1]>s[t]);
 						break;
					case 13:
						t--;
						s[t-1]=(s[t-1]<=s[t]);
 						break;
					case 14://输出浮点数 
						printf("%f",s[t-1]);
						fprintf(fa2,"%f",s[t-1]);
						t--;
						break;
					case 15:
						printf("\n");
						fprintf(fa2,"\n");
						break;
					case 16:
						printf("?");
						fprintf(fa2,"?");
						scanf("%f",&(s[t]));
						fprintf(fa2,"%f\n",s[t]);
						t++;
						break;
					case 17://输出整数 
						printf("%d",int(s[t-1]));
						fprintf(fa2,"%d",int(s[t-1]));
						t--;
						break;
				}
				break;
			case lod:       /*取相对当前过程的数据基地址为ａ的内存的值到栈顶*/
				s[t]=s[base(i.l,s,b)+int(i.a)];
				t++;
				break;
			case lod1://次栈顶代表地址 
				t--;
				//进行上下界的处理				
				if(int(s[t])<i.l||int(s[t])>arraybound[int(i.a)]){ 
					error(39);//数组越界 
					printf("数组越界\n"); 
				} 
				s[t]=arraystore[int(i.a)+int(s[t])];
				t++;
				break;
			case sto1:// 次栈顶为地址 栈顶为右边表达式的值 
				t--;
				if(int(s[t-1])<i.l||int(s[t-1])>arraybound[int(i.a)]){
					printf("数组越界\n"); 
					error(39);//数组越界 
				} 
				arraystore[int(i.a)+int(s[t-1])]=s[t];
				t--;
				break; 
			case sto:       /*栈顶的值存到相对当前过程的数据基地址为ａ的内存*/
				t--;
	            s[base(i.l,s,b)+int(i.a)]=s[t];
				break;
			case cal:              /*调用子程序*/
				s[t]=base(i.l,s,b); /*将父过程基地址入栈*/
				s[t+1]=b;           /*将本过程基地址入栈，此两项用于base函数*/
				s[t+2]=p;           /*将当前指令指针入栈*/
				b=t;                /*改变基地址指针值为新过程的基地址*/
				p=i.a;              /*跳转*/
				break;
			case inte:             /*分配内存*/
				t+=i.a;
				break;
			case jmp:             /*直接跳转*/
				p=i.a;
				break;
			case jpc:              /*条件跳转*/
				t--;
				if(s[t]==0)
				{
					p=int(i.a);
				}
	    		break;
		}
	}while (p!=0);
}
/*通过过程基址求上1层过程的基址*/
int base(int l,float * s,int b)
{
	int b1;
	b1=b;
	 while(l>0)
	 {
		 b1=int(s[b1]);
     	 l--;
	 }
	 return b1;
}


