#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<string>
#include<fstream>
#include<map>
#include<sstream>
#include "write_record.h"
using namespace std;
extern int linecount;
map<string,int> stopword_map;
void create_sw_map()
{
	ifstream sw;
	sw.open("stopword.txt");
	string temp;
	while (sw.good())
	{ 
		temp.clear();
		getline (sw,temp);
		stopword_map[temp]=1;
	}
}
void prearrange(char *x,char *y)
{
    	FILE *f1,*f2;
    	char c,b[1000],p[100],file1[30],file2[30];
    	int i,j,k,m;
    	long l;
    	strcpy(file1,x);
    	strcpy(file2,y);
    	f1=fopen(file1,"r");
    	f2=fopen(file2,"w+");

    	while((c=getc(f1))!=EOF)
    	{
		if(c=='\n'||c=='\t')
        	{}
        	else
        	{ 
        		putc(c,f2);
          		if(c=='.'||c=='?'||c=='!')
          		{
          			putc('\n',f2);
          		}
        	}
   	 }
   	fclose(f1);
	fclose(f2);
}
void preprocess(char *f)
{
	char fn[30],j[10];
	int i;
	char fl[30],ft[30];
	FILE *fp1,*fp2;
	fp1=fopen("filelist.txt","w+");
	fp2=fopen("taggedfilelist.txt","w+");
	strcpy(fn,f);
	string filename (fn);
	ifstream infile;
	infile.open(filename.c_str());
	linecount=0;
	string line;
	while(!infile.eof())
	{
		getline(infile,line);
		linecount++;
	}
	for(i=1;i<linecount;i++)
	{
		strcpy(fl,"file");
		strcpy(ft,"taggedfile");
		sprintf(j,"%d",i);
		strcat(fl,j);
		strcat(ft,j);
		strcat(fl,".txt\n");
		strcat(ft,".txt\n");
		fputs(fl,fp1);
		fputs(ft,fp2);
	}
	linecount--;
	cout<<"linecount = "<<linecount<<endl;
	fclose(fp1);
	fclose(fp2);
}
void perline(char *f)
{
	char fn[30],currf[30];
	char *text;
	int i;
	FILE *fl,*fc;
	fl=fopen("filelist.txt","r");
	strcpy(fn,f);
	string filename (fn);
	ifstream infile;
	infile.open(filename.c_str());
	string line;
	i=1;
	while(i<=linecount)
	{
		fscanf(fl,"%s",currf);
		fc=fopen(currf,"w+");
		ofstream ff(currf);
		getline(infile,line);
		ff<<line<<endl;
		cout<<line<<endl;
		ff.close();
		fclose(fc);
		i++;
	}
	fclose(fl);
}
void partofspeech()
{
	FILE *fl,*ft;
	int i;
	char cfl[30],cft[30],cmnd[100];
	fl=fopen("filelist.txt","r");
	ft=fopen("taggedfilelist.txt","r");
	i=1;
	while(i<=linecount)
	{
		fscanf(fl,"%s",cfl);
		fscanf(ft,"%s",cft);
		strcpy(cmnd,"./parse<");
		strcat(cmnd,cfl);
		strcat(cmnd,">");
		strcat(cmnd,cft);
		system(cmnd);
		i++;
	}
}
void prune()
{
	FILE *f1,*f2,*fl,*ft;
	char a[100],b[100],text[1000];
	int len1,len2,i;
	char file1[30],file2[30];
	fl=fopen("filelist.txt","r");
	ft=fopen("taggedfilelist.txt","r");
	i=1;
	while(i<=linecount)
	{
		text[0]='\0';
		fscanf(fl,"%s",file1);
		fscanf(ft,"%s",file2);
		f1=fopen(file1,"r");
		f2=fopen(file2,"r");
		while(!feof(f1))
		{
			a[0]='\0';
			fscanf(f1,"%s",a);
			//puts(a);
			if((strstr(a,","))||(strstr(a,"."))||(strstr(a,"!"))||(strstr(a,"?"))||(strstr(a,";")))
			{
				len1=strlen(a);
				a[len1-1]='\0';	
			}
			while(!feof(f2))
			{
				fscanf(f2,"%s",b);
				if((a[0]==b[0])||(a[0]==(b[0]+32))||(a[0]==(b[0]-32)))
				{
					if((a[0]==(b[0]+32))||(a[0]==(b[0]-32)))
						b[0]=a[0];
					if(strstr(b,a))
					{
						len1=strlen(a);
						len2=strlen(b);
						if(len1==len2)
						{
							strcat(text,b);
							strcat(text," ");
							//puts(b);
							break;
						}
						else if(len2==(len1+2))
						{
							if(b[len2-2]=='.')
							{
								strcat(text,b);
								strcat(text," ");
							//	puts(b);
								break;
							}
						}
						else if(len2==(len1+5))
						{
							if((b[len2-5]=='[')&&(b[len2-4]=='?')&&(b[len2-3]==']')&&(b[len2-2]=='.')&&(b[len2-1]=='n'))
							{
								strcat(text,b);
								strcat(text," ");
							//	puts(b);
								break;
							}
						}
						else if(len2==(len1+3))
						{
							if((b[len2-5]=='[')&&(b[len2-4]=='?')&&(b[len2-3]==']'))
							{
								strcat(text,b);
								strcat(text," ");
							//	puts(b);
								break;
							}	
						}
					}
				}
			}
			rewind(f2);
		}
		strcat(text,"\n");
		fclose(f1);
		fclose(f2);
		f1=fopen(file2,"w+");
		fputs(text,f1);
		fclose(f1);
		i++;
	}
}
void perlinetagged()
{
	FILE *f1,*f2;
	int i;
	char file[30];
	string line;
	ifstream infile;
	f1=fopen("taggedfilelist.txt","r");
	f2=fopen("perlinepos.txt","w+");
	i=1;
	while(i<=linecount)
	{
		fscanf(f1,"%s",file);
		string filename (file);
		infile.open(filename.c_str());
		getline(infile,line);
		line.append("\n");
		fputs(line.c_str(),f2);
		infile.close();
		i++;
	}
	fclose(f1);
	fclose(f2);
}
void deletefiles()
{
	char file[30],cmnd[100];
	FILE *f1,*f2;
	int i;
	f1=fopen("filelist.txt","r");
	f2=fopen("taggedfilelist.txt","r");
	i=1;
	while(i<=linecount)
	{
		fscanf(f1,"%s",file);
		strcpy(cmnd,"rm ");
		strcat(cmnd, file);
		system(cmnd);
		fscanf(f2,"%s",file);
		strcpy(cmnd,"rm ");
		strcat(cmnd, file);
		system(cmnd);
		i++;
	}
}

void perlinetagged_sw()
{
	ifstream infile ("perlinepos.txt");
	fstream outfile ("perlinepos-sw.txt", ios::out | ios::in | ios::trunc);
	string temp,output_string,temp2;
	string line;
	int counter=0;
	while(infile.good())
	{	
		getline (infile,line);
		istringstream input (line,istringstream::in);
		while (input>>temp)
		{	
			cout<<temp<<endl;
			
			if(temp[temp.length()-2]=='.')
			{	
				temp2.clear();
				temp2=temp;
				temp.resize(temp.length()-2);
			}
			if(stopword_map[temp]!=1)
			{
				cout<<temp<<endl;
				if(output_string.empty())
					output_string=temp2;
				else
				{
					output_string.append(" ");
					output_string.append(temp2);
				}
			}
			temp.clear();
			temp2.clear();
		}
		cout<<output_string<<endl;
		if(++counter==linecount)
			outfile<<output_string;
		else 
			outfile<<output_string<<"\n";
		output_string.clear();	
	}
}				
