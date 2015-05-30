#include"sumsense.h"
#include<iostream>
#include<cstdio>
#include<cstring>
#include<cstdlib>
using namespace std;
int linecount=0;
char inputfile[100]={'\0'};
char processfile[100]={'\0'};
int main(int argc,char *argv[])
{
	int perc=100;
	if(argc<2)
	{
		cout<<"error in the number of arguments\n";
		exit(0);
	}
	else
	{
		perc=atoi(argv[2]);
		cout<<perc<<endl;
		create_sw_map();
		strcpy(inputfile,argv[1]);
		strcpy(processfile,"perline.txt");
		prearrange(inputfile,processfile);
		preprocess(processfile);
		perline(processfile);
		partofspeech();
		prune();
		perlinetagged();
		deletefiles();
		perlinetagged_sw();
		wordsensedisambiguation();
		temp_display();
		ranking();
		cout<<endl;
		temp_display();
		generate_sentence_list(perc);
		generate_summary();
	}
	return 0;
}
