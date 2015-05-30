#include <iostream>
#include <map>
#include <vector>
#include <cstring>
#include<cstdio>
#include<algorithm>
#include <fstream>
#include <set>
#include "stage2.h"
using namespace std;
extern int linecount;
map<long int,vector<int> > hash_table;
/*the hast table record format is
				0			1			2
 index  - >  FREQUENCY	PRIORITY	SENTENCE NUMS…
*/
set<int> sentence_list;
int max_freq=0;


void write_record1(int sentence_num,long int h)
{
	
	vector<int> row;
	
	int i,flag;
	cout<<"hash = "<<h;

	if(hash_table.count(h)==0)
	{
		row.push_back(1);
		row.push_back(0);
		row.push_back(sentence_num);
		hash_table[h]=row;
		//cout<<"sentence number" <<hash_table[h][2];
		
	}
	else
	{	
		hash_table[h][0]++;
	
		flag=0;
		for(i=2;i<hash_table[h].size();i++)
		{
			if(sentence_num==hash_table[h][i])
			{
				flag=1;
				break;
			}
		}
			if(flag==0)
			{
				hash_table[h].push_back(sentence_num);
			}
	}
	if (hash_table[h][0]>max_freq)
	{
		max_freq=hash_table[h][0];
	}
}

void temp_display()
{
	map<long int,vector<int> >::iterator it;
	int i;
	for(it=hash_table.begin();it!=hash_table.end();it++)
	{
		cout<<"\nHash value ="<<(*it).first;
		cout<<" Frequency ="<<(*it).second[0]<<" Priority="<<(*it).second[1];
		cout<<"Sentence numbers=";
		for(i=2;i<((*it).second).size();i++)
			cout<<(*it).second[i]<<" ";
	}	
	cout<<"Size of hash table="<<hash_table.size();
}	

void ranking()
{	
	map<long int,vector<int> >::iterator it;
	cout<<"\n RANKING\n";
	for(it=hash_table.begin();it!=hash_table.end();it++)
	{
		((*it).second)[1]=max_freq-((*it).second)[0];
	//	cout<<((*it).second)[1]<<endl;
	}
	cout << "ranking done\n";
}

void generate_summary() //requires the sentence per_line file-pointer to be called per_line
{
	int i,j; 
	ofstream outfile ("output.txt", ios::out | ios::trunc);
	ifstream infile ("perline.txt");
	string temp_sentence;
	set<int>::iterator it;
	for (it=sentence_list.begin(); it!=sentence_list.end(); it++)
	{	
		infile.seekg(0,ios::beg);
		for (j=0; j<(*it); j++)
		{
			getline(infile,temp_sentence);
		}
		getline(infile,temp_sentence);
		cout<<temp_sentence;
		outfile<<temp_sentence<<" "; //SPACE NEEDED?
	}
}	
void generate_sentence_list(int percentage)
{
	int nooflines,i,j,cur_priority=0,cur_line=1,flag;
	nooflines =((float)percentage/100) * linecount; 
	cout<<"No of lines="<<nooflines<<endl;
	sentence_list.clear();
	map<long int, vector<int> >::iterator it;
	set<int>::iterator jt;
	while(cur_line<=nooflines)
	{	//cout<<"yes!";
		flag=0;
	for(it=hash_table.begin();it!=hash_table.end();it++)
	{
		if(((*it).second)[1]==cur_priority)
		{	//flag=1;
	//	cout<<"yes";
			for (j=2; j<((*it).second).size(); j++)
			{
				//cout<<"yes";
				//cout<<"VALUE="<<((*it).second)[j]<<endl;
				if(sentence_list.count(((*it).second)[j])==0)
				{
					if(sentence_list.size()>=nooflines)
						return;
					cout << "Pushing "<<((*it).second)[j] ;   
					sentence_list.insert( ((*it).second)[j] );
					cur_line++;
					cout<<"\ncurline = "<<cur_line<<endl;
				}
			}
		}
	}
		//if(flag==0)
			cur_priority++;
	}
	//sort(sentence_list.begin(),sentence_list.end());
	cout << "\nSentence list ";
	for (jt=sentence_list.begin(); jt!=sentence_list.end(); jt++) 
	{
		cout << (*jt)<<" ";
	}
}


