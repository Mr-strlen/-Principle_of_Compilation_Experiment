//A.2     C      ��   ��

/*��������л���:
*1Visual C++6.0,VisualC++.NET and Visual C++.NET 2003
*WinNT, Win 200, WinXP and  Win2003 
*2 gcc version 3.3.2  20031022(Red Hat Linux 3.3.2-1)
*Redhat Fedora core 1
*Intel 32 platform
*ʹ�÷���:
*���к�����PL/0 Դ�����ļ���
*�ش��Ƿ�������������
*�ش��Ƿ�������ֱ�
*fa.tmp ������������
*fa1.tmp  ���Դ�ļ�������ж�Ӧ���׵�ַ
*fa2.tmp  ������ 
*fas.tmp  ������ֱ�
*/
#include<stdio.h>
#include<stdlib.h>
#include"pl0.h"
#include"string.h"
/*����ִ��ʱʹ�õ�ջ*/
#define stacksize 500
int main()
{
	bool nxtlev[symnum];
	printf("Input pl/0 file ?");
	scanf("%s",fname);                                     /*�����ļ���*/
	fin=fopen(fname,"r");
	if(fin)
	{
		//��ʼ������ջ
		memset(arraystore,0,sizeof(arraystore)); 
		printf("List object code ?(Y/N)");                /*�Ƿ�������������*/
		scanf("%s",fname);
		listswitch=(fname[0]=='y'||fname[0]=='Y');
		printf("List symbol table ? (Y/N)");             /*�Ƿ�������ֱ�*/
		scanf("%s",fname);
		tableswitch=(fname[0]=='y'||fname[0]=='Y');
		fa1=fopen("fa1.tmp","w");//���Դ�ļ� 
		fprintf(fa1,"Iput pl/0 file ?");
		fprintf(fa1,"%s\n", fname);
		init();                                          /*��ʼ��*/
		err=0;
		cc=cx=ll=0;
		ch=' ';
		if(-1!=getsym())
		{
			fa=fopen("fa.tmp","w");
			fas=fopen("fas.tmp","w");
			addset(nxtlev,declbegsys,statbegsys,symnum);
			nxtlev[period]=true;    //��� 
			if(-1==block(0,0,nxtlev))	//�����ֱ�Ϊ level	/*lev:��ǰ�ֳ������ڲ�// *tx:���ֱ�ǰβָ��//*fsys:��ǰģ�������ż���/*���ñ������*/
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
*��ʼ��
*/
void init()
{
	int i;
	for(i=0;i<=255;i++)
	{
		ssym[i]=nul;
	}
	//���ַ���˵�� 
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
	ssym['\'']=lquotation;//������������
	 
	
	/*���ñ���������,������ĸ˳��,�����۰����*/
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
	
	
	/*���ñ����ַ���*/
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

	/*����ָ������*/
	strcpy(&(mnemonic[lit][0]),"lit");
	strcpy(&(mnemonic[opr][0]),"opr");
	strcpy(&(mnemonic[lod][0]),"lod");
	strcpy(&(mnemonic[sto][0]),"sto");
	strcpy(&(mnemonic[cal][0]),"cal");
	strcpy(&(mnemonic[inte][0]),"int");
	strcpy(&(mnemonic[jmp][0]),"jmp");
	strcpy(&(mnemonic[jpc][0]),"jpc");
	strcpy(&(mnemonic[outs][0]),"outs");//����ַ���ָ�� 
	strcpy(&(mnemonic[sto1][0]),"sto1");//����ַ���ָ�� 
	strcpy(&(mnemonic[lod1][0]),"lod1");//����ַ���ָ�� 
	/*���÷��ż�*/
	for(i=0;i<symnum;i++)
	{
		declbegsys[i]=false;
		statbegsys[i]=false;
		facbegsys[i]=false;
	}

	/*����������ʼ���ż�*/
	declbegsys[constsym]=true;
	declbegsys[varsym]=true;
	declbegsys[procsym]=true;
	/*������俪ʼ���ż�*/
	statbegsys[beginsym]=true;
	statbegsys[callsym]=true;
	statbegsys[ifsym]=true;
	statbegsys[whilesym]=true;
	/*�������ӿ�ʼ���ż�*/
	facbegsys[ident]=true;
	facbegsys[number]=true;
	facbegsys[lparen]=true;
	facbegsys[leftbracket]=true;
}
 /*
  *������ʵ�ּ��ϵļ�������
  */
int inset(int e,bool* s)
{
    return s[e];
}
int addset(bool* sr,bool* s1,bool* s2,int n)//�߼������� 
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
 *��������ӡ����λ�úʹ������
 */
void error(int n)
 
{
	char space[81];
	memset(space,32,81); printf("-------%c\n",ch);
	space[cc-1]=0;//����ʱ��ǰ�����Ѿ����꣬����cc-1
	printf("****%s!%d\n",space,n);
	err++;
}
/*
 *  ©���ո񣬶�ȡһ���ַ�
 *
 *  ÿ�ζ�һ�У�����line��������line��getsymȡ�պ��ٶ�һ��
 *
 *  ������getsym����
 */
int getch()
{
	if(cc==ll)//�ճ�ʼ�����cc,cx,ll��Ϊ0 ��ȡ��һ��֮��ll��Ϊһ�е�ĩ�ˣ�ccΪһ�е��׶� 
    {
		if(feof(fin))//�ж��ļ��Ƿ���� 
		{
			printf("program incomplete");
			return -1;
		}
		ll=0;
		cc=0;
		printf("%d ",cx );
		fprintf(fa1,"%d ",cx);
		ch=' ';
		while(ch!=10)//10��ʾ���з� 
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
 /*�ʷ���������ȡһ������
 */

int getsym()
{
	int i,j,k;
	while( ch==' '||ch==10||ch==9)//9��ˮƽ�Ʊ�� 
	{
		getchdo;//˵����ʱδ��ȡ�У���Ҫ����getch()���� 
	}
	if(ch>='a'&&ch<='z')//fibonacci�ĵ�һ�ж���Ϊch=c 
	{ 
		k=0;
		do{
			if(k<al)//�ж��Ƿ񳬳���󵥴ʳ���10 
			{ 
				a[k]=ch;
				k++;
			}
			getchdo; 
		}while(ch>='a'&&ch<='z'||ch>='0'&&ch<='9');
		a[k]=0;//�ַ����Ľ�����־ 
		strcpy(id,a);//�������� 
		i=0;
		j=norw-1;
		do{//�ж��Ƿ��ǹؼ��ֳ��� 
			k=(i+j)/2;
			if(strcmp(id,word[k])<=0)
			{
				j=k-1;
			}
			if(strcmp(id,word[k])>=0)
			{
				i=k+1;
			}
		}while(i<=j);//���ַ��ؼ��ֶԱ� 
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
			if(intorreal==integer){//����Ǳ����ĸ�ֵ��������intorreal�����ǳ�������intorrealΪĬ�� 
				k=0;
				num=0;
				sym=number;
				do{
					num=10*num+ch-'0';
					k++;
					getchdo;
				}while(ch>='0'&&ch<='9'); /*��ȡ���ֵ�ֵ*/
				k--;
				if(k>nmax)
				{
					error(30);
				}
			}else if(intorreal==real){
				char floatnum[20];//���ڴ�Ÿ�����
				k=0;
				sym=number;
				realnum=0;
				do{
					floatnum[k]=ch; 
					k++;
					getchdo;
				}while((ch>='0'&&ch<='9')||(ch=='.')); /*��ȡ���ֵ�ֵ  :���ڳ����ĸ�ֵ )���ڼ������ı��ʽ*/
				floatnum[k]='\0';//ch!=';'&& ch!=':'&&ch!=','&&ch!=')'
				realnum=atof(floatnum);
			} 			
		}
		else
		{
			if(ch==':')             /*��⸳ֵ����*/
			{
				getchdo;
				if(ch=='=')
				{
					sym=becomes;
					getchdo;
				}
				else
				{
					sym=colon;        //�������͵ķ���--��    /*����ʶ��ķ���*/
				}
			}
			else if(ch=='['){//ֱ�Ӷ�ȡ��������½� 
				sym=leftbracket;				
			}	
			else if(ch==']'){
				sym=rightbracket;
				getchdo;
			}		
			else{
				if(ch=='<')         /*���С�ڻ�С�ڵ��ڷ���*/
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
					if(ch=='>')          /*�����ڻ���ڵ��ڷ���*/
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
						sym=ssym[ch];/* �����Ų�������������ʱ��ȫ�����յ��ַ��Ŵ���*/
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
int getsym2(char as[]){//ר�����ڶ�ȡ�ַ��� 
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
	getchdo;// ��ȡ�Ͻ� 
	if(ch>='a'&&ch<='z'){//��ʶ�� 
	    sym=ident;	 
		k=0;
		do{
			if(k<al)//�ж��Ƿ񳬳���󵥴ʳ���10 
			{ 
				a[k]=ch;
				k++;
			}
			getchdo; 
		}while(ch>='a'&&ch<='z'||ch>='0'&&ch<='9');
		a[k]=0;//�ַ����Ľ�����־ 
		strcpy(id,a);//�������� 
		int p=position(id,*ptx);
		if(p==0){
			error(11);//��ʶ��δ˵�� 
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
		}while(ch>='0'&&ch<='9'); /*��ȡ���ֵ�ֵ*/
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
*�������������
*
*x:instruction.f;
*y:instruction.l;
*z:instruction.a;
*/
int gen(enum fct x,int y,int z)//gendo(jmp,0,a); jmp��������ת ��ַa ��ʱΪ0 
{
	if(cx>=cxmax)//cxmax��������������Ϊ200 cx��ʼΪ0 
	{
		printf("Program too long"); /*�������*/
		return -1;
	}
	code[cx].f=x;//����������ָ����� 
	code[cx].l=y;//���� 
	code[cx].a=z;// ��ֵ 
	cx++;//��һ��ָ�� 
	return 0;
}
//���ɴ�ȡ��������ָ�� 
int gen2(enum fct x,int y,float z)
{
	if(cx>=cxmax)//cxmax��������������Ϊ200 cx��ʼΪ0 
	{
		printf("Program too long"); /*�������*/
		return -1;
	}
	code[cx].f=x;//����������ָ����� 
	code[cx].l=y;//���� 
	code[cx].a=z;// ��ֵ 
	cx++;//��һ��ָ�� 
	return 0;
}
/*
*���Ե�ǰ�����Ƿ�Ϸ�
*
*��ĳһ���֣���һ����䣬һ�����ʽ����Ҫ����ʱʱ����ϣ����һ����������ĳ����
*���ò��ֵĺ�����ţ� test ���������⣬���Ҹ��𵱼�ⲻͨ��ʱ�Ĳ��ȴ�ʩ
*��������Ҫ���ʱָ����ǰ��Ҫ�ķ��ż��ϺͲ����õļ��ϣ���֮ǰδ��ɲ��ֵĺ��
*���ţ����Լ���ͨ��ʱ�Ĵ����
*
*S1��������Ҫ�ķ���  beginsym
*s2:�������������Ҫ�ģ�����Ҫһ�������õļ��� endsym
*n:�����
*/


int test(bool* s1,bool* s2,int n)//testdo(nxtlev,declbegsys,7) 
{
    if(! inset(sym,s1))
    {
		error(n);
		/*����ⲻͨ��ʱ����ͣ��ȡ���ţ�ֱ����������Ҫ�ļ��ϻ򲹾ȵļ���*/
		while((! inset(sym,s1))&&(! inset(sym,s2)))
		{
			getsymdo; 
		}
    }
    return 0;
}
/*
 *�����������
 *
 *lev:��ǰ�ֳ������ڲ�
 *tx:���ֱ�ǰβָ��
 *fsys:��ǰģ�������ż���
 */
int block(int lev,int tx,bool* fsys)
{
    int i;
    int dx;                         /*���ַ��䵽����Ե�ַ*/
    int tx0;                        /*������ʼtx ���ֱ� */
    int cx0;                        /*������ʼcx cxΪ���������ָ��  ��ʱ��δ�� ��fa.tmp*/
    bool nxtlev[symnum];            /*���¼������Ĳ����У����ż��Ͼ�Ϊֵ�Σ�������ʹ������
                                      ʵ�֣����ݽ�������ָ�룬Ϊ��ֹ�¼������ı��ϼ�������
                                      ���ϣ������µĿռ䴫�ݸ��¼�����*/
    dx=3;							//��������˵����ʼΪ3
    tx0=tx;                         /*��¼�������ֵĳ�ʼλ��   ��ʼΪ0*/
    table[tx].adr=cx;
    gendo(jmp,0,0);
    if(lev > levmax)//���Ƕ�ײ���Ϊ3 
    {
		error(32);                                                                                                                                         
    }
    do{
        if(sym==constsym)         /*�յ������������ţ���ʼ����������*/
        {
			getsymdo;
			do{
                constdeclarationdo(&tx,lev,&dx);   /*dx��ֵ�ᱻconstdeclaration�ı䣬ʹ��ָ��*/
                start=tx;                                   
                while(sym==comma)
                {
                   getsymdo;
                   constdeclarationdo(&tx,lev,&dx);
                }
                endnum=tx;
                if(sym==colon){//�������ʹ��� 	(�����ͣ������͵�)	����ޣ����ո������ʹ���			
					getsymdo;
					if(!strcmp(id,"real")){
						enum numtype ob=real;
						for(int s=start;s<=endnum;s++){
							table[s].tp=ob;//real���͵����� 
						}
					}else if(!strcmp(id,"integer")){//�������� 
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
					error(5); /*©���˶��Ż��߷ֺ�*/
				}
			}while(sym==ident);
		}
		if(sym==varsym)/*�յ������������ţ���ʼ�����������*/
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
					if(!strcmp(id,"real")){//���������͵Ĵ��� 
						enum numtype ob=real;
						for(int s=start;s<=endnum;s++){
							table[s].tp=ob;
						}
					}else if(!strcmp(id,"integer")){//�������͵Ĵ��� 
						enum numtype ob=integer;
						for(int s=start;s<=endnum;s++){
							table[s].tp=ob;
						}
					}else if(sym==arraysym){//�������͵Ĵ��� !strcmp(id,"array") ��������ϵͳ�Ĵ��� 
						getsymdo; 
						if(sym==leftbracket){//��getsymdo���Ѿ�����˶��������½�ķ��� 
							//���������½�Ĵ��� 
							getarraybound(low,&tx);					
							getchdo;//��ȡ. 
							//ע������Ĵ����� 
							getarraybound(high,&tx);
							if(high>=low){
								getsymdo; //�ж���right ����ch=" " //���ǼӴ����� 
								getsymdo; //��ȡof							 
								if(!strcmp(id,"of")){
									getsymdo;//��ȡ��������� 
									if(!strcmp(id,"real")){//���������͵Ĵ��� 
										enum numtype ob=real;
										for(int s=start;s<=endnum;s++){
											table[s].tp=ob;
											table[s].kind=array;//����Ϊ���� 
											table[s].low=low;//������������½� 
											table[s].high=high;
											table[s].base=con;
											table[s].size=(high-low+1);
											arraybound[con]=high; 											
											con+=(high-low+1); 
										}
									}else if(!strcmp(id,"integer")){//�������͵Ĵ��� 
										enum numtype ob=integer;
										for(int s=start;s<=endnum;s++){
											table[s].tp=ob;
											table[s].kind=array;//����Ϊ���� 
											table[s].low=low;
											table[s].high=high;
											table[s].base=con;
											table[s].size=(high-low+1);
											arraybound[con]=high; 										
											con+=(high-low+1); 
										}
									}
								}else{
									error(36);//ȱ��of 
								}					
							}else{
								//���½�ļ��,�����鲻ͨ��������ͬʱtx-1
								tx-=1; 
								error(38);
							}									 
						}else{
							//������
							error(35);//ȱ�������� 
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
		while(sym==procsym)/*�յ������������ţ���ʼ�����������*/
		{
			getsymdo;
			if(sym==ident)
			{
				enter(procedur,&tx,lev,&dx);/*��¼��������*/
				getsymdo;
			}
			else
			{
				error(4);/*procedure��ӦΪ��ʶ��*/
			}
			if(sym==semicolon)
			{
				getsymdo;
			}
			else
			{
				error(5);/*©���˷ֺ�*/
			}
			memcpy(nxtlev,fsys,sizeof(bool)*symnum);
			nxtlev[semicolon]=true;
			if(-1==block(lev+1,tx,nxtlev))
			{
				return -1;/*�ݹ����*/
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
                 error(5);                       /*©���˷ֺ�*/
             }
        }
        memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
        nxtlev[ident]=true;
        nxtlev[period]=true;
        nxtlev[varsym]=true; 
        nxtlev[constsym]=true;
        testdo(nxtlev,declbegsys,7);//�˳������﷨��λ 
    }while(inset(sym,declbegsys));                /*ֱ��û����������*/
    code[table[tx0].adr].a=cx;      //������������  ?????            /*��ʼ���ɵ�ǰ���̴���*/
    table[tx0].adr=cx;                            /*��ǰ���̴����ַ*/
    table[tx0].size=dx;                           /*����������ÿ����һ�����������dx����1,���������Ѿ�����,dx���ǵ�ǰ�������ݵ�size*/
        
    cx0=cx; 
    gendo(inte,0,dx);                             /*���ɷ����ڴ����*/
    if(tableswitch)                               /*������ֱ�*/
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
                case variable://�����������͵���� 
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
	/*���������Ϊ�ֺŻ�end*/
	memcpy(nxtlev,fsys,sizeof(bool)*symnum);/*ÿ��������ż��Ͷ������ϲ������ż��ͣ��Ա㲹��*/
	nxtlev[semicolon]=true;
	nxtlev[endsym]=true;
	statementdo(nxtlev,&tx,lev);
	gendo(opr,0,0); /*ÿ�����̳��ڶ�Ҫʹ�õ��ͷ����ݶ�����*/
	memset(nxtlev,0,sizeof(bool)*symnum); /*�ֳ���û�в��ȼ���*/
	test(fsys,nxtlev,8);                  /*�����������ȷ��*/
	listcode(cx0);                        /*�������*/
	return 0;
}
/*
*�����ֱ��м���һ��
*
*k:��������const,var or procedure
*ptx:���ֱ�βָ���ָ�룬Ϊ�˿��Ըı����ֱ�βָ�����ֵ
*lev:�������ڵĲ�Σ��Ժ����е�lev��������
*pdx:Ϊ��ǰӦ����ı�������Ե�ַ�������Ҫ����1
*/
void enter (enum object k,int *ptx,int lev, int *pdx)
{
	(*ptx)++;	 
	strcpy(table[(*ptx)].name,id);       /*ȫ�ֱ���id���Ѵ��е�ǰ���ֵ�����*/
	table[(*ptx)].kind=k;
	switch(k)
	{
		case constant:                      /*��������*/
			if (num>amax)
			{
				error(31);
				num=0;
			}			 
			table[(*ptx)].realnumber=realnum;
			break;
		case variable:                     /*��������*/
			table[(*ptx)].level=lev;
			table[(*ptx)].adr=(*pdx);
			(*pdx)++;
			break;                          /*��������*/
		case procedur:
			table[(*ptx)].level=lev;
			break;
	}

}
/*
 *�������ֵ�λ��
 *�ҵ��򷵻������ֱ��е�λ�ã����򷵻�0
 *
 *idt: Ҫ���ҵ�����
 *tx:����ǰ���ֱ�βָ��
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
	intorreal=table[i].tp;//��ǰ��ȡ���ݵ����� 
	return i;
}
/*
 *������������
 */
int constdeclaration(int *  ptx,int lev,int *  pdx)
{
	if(sym==ident)//������������Ҫ�Ǳ�ʶ�� 
	{
		getsymdo;
		if(sym==eql ||sym==becomes)//becomes�Ǹ�ֵ���� 
		{
			if(sym==becomes)
			{
				error(1);                     /*��=д�����ˣ�=*/
			}
			getsymdo;
			if(sym==number)
			{
				enter(constant,ptx,lev,pdx);
				getsymdo;
			}
			else
			{
				error(2);                  /*����˵��=��Ӧ������*/
			}         
		}
		else
		{
			error(3);                       /*����˵����ʶ��Ӧ��=*/
		}
	}
	else
	{
		error(4);                        /*const��Ӧ�Ǳ�ʶ*/
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
 		enter(variable,ptx,lev,pdx);//��д���ֱ�
 		getsymdo;
 	}
 	else
 	{
 		error(4);
    }
    return 0;
}
 /*
  *����Ŀ������嵥
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
*��䴦��
*/
int statement(bool* fsys,int * ptx,int lev)
{
	int i,cx1,cx2;
   	bool nxtlev[symnum];
   	int arraypos=0;//���������λ��,ȡ����ַ��ʱ���� 
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
   	 			//������Ԫ�صĴ��� 
   	 			getsymdo;//��ȡ��[
				if(sym==leftbracket){					
					getchdo;
					getsymdo;
					memcpy(nxtlev,fsys,sizeof(bool)* symnum);	
					expressiondo(nxtlev,ptx,lev);//�洢������ 
					if(sym==rightbracket){
						getsymdo;
		   	 			if(sym==becomes){		   	 			
		   	 				getsymdo;
		   	 			}else{
		   	 			 	error(13);
		   	 			}		
						i=position(id,*ptx);					
						memcpy(nxtlev,fsys,sizeof(bool)* symnum);
						dealarray(nxtlev,ptx,lev); //�����ұߵı��ʽ
//						if(table[i].kind==array){//���õݹ鴦��array 
//		   	 				//�����ұߵı��ʽ 
//		   	 				dealarray(nxtlev,ptx,lev); 		   	 				
//						}else{							
//		   	 				expressiondo(nxtlev,ptx,lev);//�洢������ 
//						}	   	 					   	 			
						if(i!=0){
							gendo(sto1,table[i].low,table[arraypos].base);
							//�洢��ֵ����  arrayposΪ��������λ�� 
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
   	 			//����b:=a[1] �����ָ�ֵ����    	 			 
   	 			memcpy(nxtlev,fsys,sizeof(bool)* symnum);
   	 			if(sym==ident){
   	 				dealarray(nxtlev,ptx,lev);//�����ұ߸�ֵ����ı��ʽ 
				}else{
					expressiondo(nxtlev,ptx,lev);//�洢������ 					
				}   	 			
   	 			if(i!=0)
   	 			{
   	 			 	gendo(sto,lev-table[i].level,table[i].adr);//����������ֵ����Ӧ�ı��� 
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
    	   	   		}else if(table[i].kind==array){//��������Ķ��� 
    	   	   		    //�ȴ����ַ
    	   	   		    readflag=1;//��ʾ��ȡ���� 
						dealarray(nxtlev,ptx,lev);  
    	   	   			gendo(opr,0,16);//ֵ     	   	   			
    	   	   			//����������洢�������� 
    	   	   			gendo(sto1,table[i].low,table[i].base);
				    } 
    	   	   		else
    	   	   		{
    	   	   			gendo(opr,0,16);
						gendo(sto,lev-table[i].level,table[i].adr);	/* ���浽����*/
						getsymdo;
					}
					
				}while (sym==comma);	/*һ��read���ɶ�������� */
				readflag=0; 
			}
			if(sym!=rparen)
			{
				error(33);			/* ��ʽ����Ӧ��������*/
				while(!inset(sym,fsys))/* �����ȣ�ֱ���յ��ϲ㺯���ĺ������*/
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
			if(sym==writesym)			/* ׼������write��䴦����read����*/
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
								gendo(opr,0,17);/* �������ָ����ջ����ֵ*/	
							}else if(table[p].tp==real){
								gendo(opr,0,14);/* �������ָ����ջ����ֵ*/	
							}
						}else if(sym==lquotation){//�����ַ�������� 									
							getsym2(st);//ȡ���ַ��� 
							int s=stail;
							for(int p=stail;p<stail+strlen(st);p++){
								ss[p]=st[p-stail];
							}								
							stail+=strlen(st);		
							gendo(outs,s,stail);//�ӷ��ű�������ַ��� 
							getsymdo;//ȡ��' 
							getsymdo;//ȡ����			
						}else{//�������ַ�������� 
							memcpy(nxtlev,fsys,sizeof(bool)*symnum);
							nxtlev[rparen]=true;
							nxtlev[comma]=true;		/* write�ĺ������Ϊ��or��*/
							nxtlev[lquotation]=true;
							expressiondo(nxtlev,ptx,lev);/* ���ñ��ʽ�����˴���read��ͬ��readΪ��������ֵ*/
							if(intorreal==integer){
								gendo(opr,0,17);/* �������ָ����ջ����ֵ*/	
							}else if(intorreal==real){
								gendo(opr,0,14);/* �������ָ����ջ����ֵ*/	
							}							
						}						
					}while(sym==comma);							
					if(sym!=rparen)
					{
						error(33);/* write()ӦΪ�������ʽ*/
					}
					else
					{
						getsymdo;
					}										
				}
				gendo(opr,0,15);		/* �������*/
			}
			else
			{
				if(sym==callsym)		/* ׼������call��䴦��*/
				{
					getsymdo;
					if(sym!=ident)
					{
						error(14);           /*call��ӦΪ��ʶ��*/
					}
					else
					{
						i=position(id,*ptx);
						if(i==0)
						{
							error(11);          /*����δ�ҵ�*/
						}
						else
						{
							if(table[i].kind==procedur)
							{
								gendo(cal,lev-table[i].level,table[i].adr);  /*����callָ��*/
							}
							else
							{
								error(15);      /*call���ʶ��ӦΪ����*/
							}
						}
						getsymdo;
					}
				}
				else
				{
					if(sym==ifsym)     /*׼������if��䴦��*/
					{
						getsymdo;
						memcpy(nxtlev,fsys,sizeof(bool)*symnum);
						nxtlev[thensym]=true;
						nxtlev[dosym]=true;    /*�������Ϊthen��do*/
						conditiondo(nxtlev,ptx,lev);   /*�������������߼����㣩����*/
						if(sym==thensym)
						
						{
							getsymdo;
						}
						else
						{
							error(16);          /*ȱ��then*/
						}
						cx1=cx;                /*���浱ǰָ���ַ*/
						gendo(jpc,0,0);        /*����������תָ���ת��ַ��д0*/
						statementdo(fsys,ptx,lev);   /*����then������*/
						code[cx1].a=cx;      /*��statement�����cxΪthen�����ִ��
                            ���λ�ã�������ǰ��δ������ת��ַ*/
					}
					else
					{
						if(sym==beginsym)   /*׼�����ո�����䴦��*/
						{
							getsymdo;
							memcpy(nxtlev,fsys,sizeof(bool)*symnum);
							nxtlev[semicolon]=true;
							nxtlev[endsym]=true;/*�������Ϊ�ֺŻ�end*/
							/*ѭ��������䴦������ֱ����һ�����Ų�����俪ʼ���Ż��յ�end*/
							statementdo(nxtlev,ptx,lev);
							while(inset(sym,statbegsys)||sym==semicolon)
							{
								if(sym==semicolon)
								{
									getsymdo;
								}
								else
								{
									error(10);/*ȱ�ٷֺ�*/
								}
								statementdo(nxtlev,ptx,lev);
							}
							if(sym==endsym)
							{
								getsymdo;
							}
							else
							{
								error(17); /*ȱ��end��ֺ�*/
							}
						}
						else
						{
							if(sym==whilesym)/*׼������while��䴦��*/
							{
								cx1=cx;        /*�����ж�����������λ��*/ //cxΪ�����ָ��λ�� 
								getsymdo;
								memcpy(nxtlev,fsys,sizeof(bool)*symnum);
								nxtlev[dosym]=true;/*�������Ϊdo*/
								conditiondo(nxtlev,ptx,lev);  /*������������*/
								cx2=cx;       /*����ѭ����Ľ�������һ��λ��*/
								gendo(jpc,0,0);/*����������ת��������ѭ���ĵ�ַδ֪*/
								if(sym==dosym)
								{
									getsymdo;
								}
								else
								{
									error(18);      /*ȱ��do*/
								}
								statementdo(fsys,ptx,lev); /*ѭ����*/
								gendo(jmp,0,cx1);/*��ͷ�����ж�����*/
								code[cx2].a=cx;   /*��������ѭ���ĵ�ַ����if����*/
							}else if(sym==forsym){								
								getsymdo;//�õ�id
								int p=position(id,*ptx); 
								statementdo(fsys,ptx,lev); /*����i�ĸ�ֵ*/
								gendo(lod,lev,table[p].adr); 
								int cx3=cx;
								gendo(jmp,0,0);//������һ����step ��������ת 
								cx1=cx;
								gendo(lod,lev,table[p].adr);
								//����һ������i��ȡֵ 
								/*�����ж�����������λ��*/ //cxΪ�����ָ��λ�� 
								//����step��until
								if(sym!=stepsym){
									//���� 
									error(37);//ȱ��step 
								}else{
									getsymdo;									
									forcondition(nxtlev,ptx,lev,p,cx3);  /*������������*/
									cx2=cx;       /*����ѭ����Ľ�������һ��λ��*/
									gendo(jpc,0,0);/*����������ת��������ѭ���ĵ�ַδ֪*/
								} 
								memcpy(nxtlev,fsys,sizeof(bool)*symnum);
								nxtlev[dosym]=true;/*�������Ϊdo*/
								if(sym==dosym)
								{
									getsymdo;
								}
								else
								{
									error(18);      /*ȱ��do*/
								}
								statementdo(fsys,ptx,lev); /*ѭ����*/
								gendo(jmp,0,cx1);/*��ͷ�����ж�����*/
								code[cx2].a=cx;   								
							}
							else
							{
								memset(nxtlev,0,sizeof(bool)*symnum);/*�������޲��ȼ���*/
								testdo(fsys,nxtlev,19);/*�������������ȷ��*/
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
	//���������ұߵı��ʽ ���ʽΪ���鿪ͷ 
	//2+a[3]�Ĵ��� ?  
	bool nxtlev[symnum];
	memcpy(nxtlev,fsys,sizeof(bool)*symnum);
	int arraypos=0;//���������λ��,ȡ����ַ��ʱ���� 
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
   	 		if(table[i].kind==constant){//������ 
   	 			if(table[i].tp==real){
					gen2(lit,0,table[i].realnumber);/*����������ָ��*/
				}else{
					gendo(lit,0,table[i].val);     /*ֱ�Ӱѳ�����ֵ��ջ*/
				}
   	 			getsymdo;
			}
   	 		else if(table[i].kind!=variable&&table[i].kind!=array)
   	 		{
   	 		 	error(12);
   	 		 	i=0;
   	 		}
   	 		else if(table[i].kind==array){
   	 			arraypos=i;//��������λ�ã����ڼ������ַ 
   	 			//������Ԫ�صĴ��� 
   	 			getsymdo;//��ȡ��[
				if(sym==leftbracket){					
					getchdo;
					getsymdo;
					memcpy(nxtlev,fsys,sizeof(bool)* symnum);	
					expressiondo(nxtlev,ptx,lev);//����[]�ı��ʽ 
					if(sym==rightbracket){
						if(i!=0&&readflag!=1){//������readflag����ᵼ�¶���ʱ����һ��ָ�������ɲ���ȷ�ĸ�ֵ 
							gendo(lod1,table[i].low,table[arraypos].base);
							//�洢��ֵ����  arrayposΪ��������λ�� 
						}
						getsymdo;
//		   	 			if(sym==becomes){		 //�������������  	 			
//		   	 				getsymdo;
//		   	 			}else 
						if(sym==plus||sym==minus||sym==times||sym==slash){
							addop=sym; 
		   	 				getsymdo;	
							dealarray(nxtlev,ptx,lev);	
							if(addop==plus){
								gendo(opr,0,2);                    /*���ɼӷ�ָ��*/
							}else if(addop==minus){
								 gendo(opr,0,3);                /*���ɼ���ָ��*/
							}else if(addop==times){
								gendo(opr,0,4); 
							}else if(addop==slash){
								gendo(opr,0,5); 
							}							   	 				
						}else if(sym==semicolon||sym==comma||sym==rparen||sym==lss||sym==eql||sym==neq||sym==leq||sym==gtr||sym==geq||sym==thensym||sym==stepsym){
							//��������ִ��
							//comma�����ָ����Ҫ��
							//semicolon�Ƿ־���Ҫ�� 
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
						gendo(opr,0,2);                    /*���ɼӷ�ָ��*/
					}else if(addop==minus){
						 gendo(opr,0,3);                /*���ɼ���ָ��*/
					}else if(addop==times){
						gendo(opr,0,4); 
					}else if(addop==slash){
						gendo(opr,0,5); 
					}							   	 				
				}else if(sym==semicolon||sym==comma||sym==rparen||sym==stepsym||sym==lss||sym==eql||sym==neq||sym==leq||sym==gtr||sym==geq||sym==thensym){
					//��������ִ��
					//comma�����ָ����Ҫ��
					//semicolon�Ƿ־���Ҫ�� 
				}else{
   	 			 	error(13);
   	 			}					
   	 	    }
		} 
	}else if(sym==number){//����1+a[2] 
		if(intorreal==real){
			gen2(lit,0,realnum);/*����������ָ��*/					
		}else{
			gendo(lit,0,num);     /*ֱ�Ӱѳ�����ֵ��ջ*/
		} 
		getsymdo; 
		if(sym==plus||sym==minus||sym==times||sym==slash){
			addop=sym; 
			getsymdo;	
			dealarray(nxtlev,ptx,lev);	
			if(addop==plus){
				gendo(opr,0,2);                    /*���ɼӷ�ָ��*/
			}else if(addop==minus){
				 gendo(opr,0,3);                /*���ɼ���ָ��*/
			}else if(addop==times){
				gendo(opr,0,4); 
			}else if(addop==slash){
				gendo(opr,0,5); 
			}							   	 				
		}else if(sym==semicolon||sym==comma||sym==rparen){
			//��������ִ��
			//comma�����ָ����Ҫ��
			//semicolon�Ƿ־���Ҫ�� 
		}else{
		 	error(13);
		}			
	}
}
 
/*
*���ʽ����
*/
int expression(bool*fsys,int*ptx,int lev)
{
	enum symbol addop;                    /*���ڱ���������*/
	bool nxtlev[symnum];
	if(sym==plus||sym==minus)             /*��ͷ�������ţ���ʱ��ǰ���ʽ������һ�����Ļ򸺵���*/
	{
		addop=sym;                    /*���濪ͷ��������*/
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);                /*������*/
		if(addop==minus)
		{
			gendo(opr,0,1);                   /*�����ͷΪ��������ȡ��ָ��*/
		}
	}
	else                             /*��ʱ���ʽ��������ļӼ�*/
	{
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;//�ӡ��� 
		termdo(nxtlev,ptx,lev);            /*������*/
	}
	while(sym==plus||sym==minus)
	{
		addop=sym;
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);              /*������*/
		if(addop==plus)
		{
			gendo(opr,0,2);                    /*���ɼӷ�ָ��*/
		}
		else
		{
			 gendo(opr,0,3);                /*���ɼ���ָ��*/
		}
	}
	return 0;
}
/*
*���
*/
int term(bool*fsys,int *ptx,int lev)
{
    enum symbol mulop;               /*���ڱ���˳�������*/
    bool nxtlev[symnum];
    memcpy(nxtlev,fsys,sizeof(bool)*symnum) ;
    nxtlev[times]=true;
    nxtlev[slash]=true;
    factordo(nxtlev,ptx,lev);       /*��������*/
    while(sym==times||sym==slash)
    {
		mulop=sym;
        getsymdo;
        factordo(nxtlev,ptx,lev);
        if(mulop==times)
        {
            gendo(opr,0,4);          /*���ɳ˷�ָ��*/
        }
        else
        {
            gendo(opr,0,5);           /*���ɳ���ָ��*/
        }
    }
     return 0;
}
/*
*���Ӵ���
*/
int factor(bool*fsys,int *ptx,int lev)
 {
 	//fsysΪ���ƹ�����nxtlev 
    int i;
    bool nxtlev[symnum];
    testdo(facbegsys,fsys,24);           /*������ӵĿ�ʼ���ú�*/
    while(inset(sym,facbegsys))          /*ѭ��ֱ���������ӿ�ʼ����*/
    {
        if(sym==ident)                   /*����Ϊ�������߱���*/
        {
            i=position(id,*ptx);        /*��������*/
            if(i==0)
            {
                error(11);               /*��ʶ��δ����*/
            }
            else
            {
				switch(table[i].kind)
				{
					case constant:                                      /*����Ϊ����*/
					    if(table[i].tp==real){
					    	gen2(lit,0,table[i].realnumber);/*����������ָ��*/
						}else{
							gendo(lit,0,table[i].val);     /*ֱ�Ӱѳ�����ֵ��ջ*/
						}						                  
						break;
					case variable:                                      /*����Ϊ����*/
						gendo(lod,lev-table[i].level,table[i].adr);      /*�ҵ�������ַ������ֵ��ջ*/
						break;
					case procedur:                                      /*����Ϊ����*/
						error(21);                                       /*����Ϊ����*/
						break;
				}
			}
			getsymdo;
		}
		else
		{
			if(sym==number)                                             /*����Ϊ��*/
			{
				if(num>amax)
				{
					error(31);
					num=0;
				}
				if(intorreal==integer){
					gendo(lit,0,num);
				}else{
					gen2(lit,0,realnum);//������������� lit 0 realnum 
				}				
								 
				getsymdo;
			}
			else
			{
				if(sym==lparen)                                           /*����Ϊ���ʽ*/
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
						error(22);                                       /*ȱ��������*/
					}
				}
				testdo(fsys,facbegsys,23);                        /*���Ӻ��зǷ�����*/
			}
		}
	}
	return 0;
}
/*
 ��������*/
int condition(bool* fsys,int* ptx,int lev)
{
    enum symbol relop;
    bool nxtlev[symnum];
    if(sym==oddsym)                        /*׼������odd���㴦��*/
   	{
		getsymdo;
		expressiondo(fsys,ptx,lev);
		gendo(opr,0,6);                              /*����oddָ��*/
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
		dealarray(nxtlev,ptx,lev);//�ȿ��Դ���������ʽ��Ҳ���Դ�����ͨ�ı��ʽ 
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
}                                                  /*���ͳ���*/
int forcondition(bool*fsys,int*ptx,int lev,int p,int cx3){
	bool nxtlev[symnum];
    if(sym==oddsym)                        /*׼������odd���㴦��*/
   	{
		getsymdo;
		expressiondo(fsys,ptx,lev);
		gendo(opr,0,6);                              /*����oddָ��*/
    }else{
    	expressiondo(fsys,ptx,lev);//ȡ��step 
    	gendo(opr,0,2);//��i������� 
    	gendo(sto,lev,table[p].adr);
    	
    	gendo(lod,lev,table[p].adr);
    	code[cx3].a=cx;
    	//�Ž�ȥ���ó��� 
    	if(sym!=untilsym){
    		//���� 
    		error(38); 
		}else{
			getsymdo;
			expressiondo(fsys,ptx,lev);//ȡuntilֵ 
			gendo(opr,0,10);//ջ�����ջ�����бȽ�			 
		} 
	}
	return 0;
}
void interpret()
{
	int p,b,t;             /*ָ��ָ�룬ָ���ַ��ջ��ָ��*/
	struct instruction i;  /*��ŵ�ǰָ��*/
	float s[stacksize];      /*����ջ*/	
	printf("start pl0\n");
	t=0;//ջ��ָ�� 
	b=0;//ָ���ַ 
	p=0;//ָ��ָ�� 
	s[0]=s[1]=s[2]=0;
	int k=0;
	do{
		i=code[p];         /*����ǰָ��*/
		p++;
		switch(i.f)
		{
			case lit:        /*��a��ֵȡ��ջ��*/
				s[t]=i.a;
				t++;
				break;
			case outs:	/*����ַ���ָ��*/
				for(k=i.l;k<int(i.a);k++){ 
					printf("%c",ss[k]);
				}
				break;
			case opr:        /*���֡��߼�����*/
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
					case 14://��������� 
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
					case 17://������� 
						printf("%d",int(s[t-1]));
						fprintf(fa2,"%d",int(s[t-1]));
						t--;
						break;
				}
				break;
			case lod:       /*ȡ��Ե�ǰ���̵����ݻ���ַΪ����ڴ��ֵ��ջ��*/
				s[t]=s[base(i.l,s,b)+int(i.a)];
				t++;
				break;
			case lod1://��ջ�������ַ 
				t--;
				//�������½�Ĵ���				
				if(int(s[t])<i.l||int(s[t])>arraybound[int(i.a)]){ 
					error(39);//����Խ�� 
					printf("����Խ��\n"); 
				} 
				s[t]=arraystore[int(i.a)+int(s[t])];
				t++;
				break;
			case sto1:// ��ջ��Ϊ��ַ ջ��Ϊ�ұ߱��ʽ��ֵ 
				t--;
				if(int(s[t-1])<i.l||int(s[t-1])>arraybound[int(i.a)]){
					printf("����Խ��\n"); 
					error(39);//����Խ�� 
				} 
				arraystore[int(i.a)+int(s[t-1])]=s[t];
				t--;
				break; 
			case sto:       /*ջ����ֵ�浽��Ե�ǰ���̵����ݻ���ַΪ����ڴ�*/
				t--;
	            s[base(i.l,s,b)+int(i.a)]=s[t];
				break;
			case cal:              /*�����ӳ���*/
				s[t]=base(i.l,s,b); /*�������̻���ַ��ջ*/
				s[t+1]=b;           /*�������̻���ַ��ջ������������base����*/
				s[t+2]=p;           /*����ǰָ��ָ����ջ*/
				b=t;                /*�ı����ַָ��ֵΪ�¹��̵Ļ���ַ*/
				p=i.a;              /*��ת*/
				break;
			case inte:             /*�����ڴ�*/
				t+=i.a;
				break;
			case jmp:             /*ֱ����ת*/
				p=i.a;
				break;
			case jpc:              /*������ת*/
				t--;
				if(s[t]==0)
				{
					p=int(i.a);
				}
	    		break;
		}
	}while (p!=0);
}
/*ͨ�����̻�ַ����1����̵Ļ�ַ*/
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


