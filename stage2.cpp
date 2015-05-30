#include <iostream>
#include "wn.h"
#include <vector>
#include <cstring>
#include <cmath>
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <map>
#include "write_record.h"
using namespace std;
#define D 10
extern int linecount;
struct node{
	double WP;
	long int hash_tag;
	string def;
	SynsetPtr h_ptr;
	vector<struct node*> in_list; 
	vector<struct node*> out_list;
	vector<double> in_weight;
	vector<double> out_weight;
};
vector<struct node> n;
int nodecount=0,cur_word=0,num_words=0;
vector<int> num_senses;
vector<double> temp_weight;
vector<string> words; //global for printing output 
string overlap;
extern map<string,int> stopword_map;
map<long int,int> table;

//HAVE TO SET MAXDIST
char mean(char word[])
{
	int l=strlen(word);
	if(word[l-2]!='.')
		return 'F';
	else	
	      return word[l-1];
}
long int hash(char* a,SynsetPtr s )				//Hash function
{
	int l=strlen(a),acc=0, i;  //acc is the accumulator
	
	for(i=0;i<l;i++)
		acc+=(a[i]*(53));             //adding all the ascii values of individual characters 
	acc=acc/(l+3)+(acc%l);
	/*if(s!=NULL)
		for(i=0;i<(s->wcount);i++)
			acc*=strlen(s->words[i]);*/
	
	return acc;
}


/*int punctuation(char a)
{
	if( a=='.' || a==',' || a=='"' || a=='!' || a=='?' || a=='(' || a==')' )
		return 1;
	else 
		return 0;
}
/*void create_sw_map()
{
	ifstream sw;
	sw.open("stopword.txt");
	string temp;
	while (sw.good())
	{	temp.clear();
		getline (sw,temp);
		stopword_map[temp]=1;
	}
}*/

double pop_map(SynsetPtr s)
{
	int level,length;
	long int h;
	float distance,max_distance=0;
	if(s)
	{	
		level=1;
		do
		{	h=hash(s->defn,s);
			cout<<"\nHash="<<h;
			if(table.count(h)!=0)
			{		
				cout<<"\nMatch! ";
				cout << "\ntable1="<<table[h]<<" table2= "<<level<<endl;
				length=table[h]+level;
				distance= - 1 * log(float(length)/(2*D));
				//	cout <<"distance "<< distance;
				if(distance>max_distance)
					max_distance=distance;
			}
			
			else 
			{
				table[h]=level;
				//	cout << "\ntable[h]="<<table[h];
			}
			
			level++;
			
			if(level>5)
				break;
		}while (s=s->ptrlist);
	}
	return max_distance;
}

double semantic_distance(SynsetPtr s1, SynsetPtr s2)
{
	//cout<<"map1= "<<pop_map(s1);
	double x= pop_map(s2);
	cout <<"\nSemdist "<< x;
	return x;
}


void add_vertex(SynsetPtr s) //function to create node, assign hash tag and initial WP 
{	
	struct node temp_node;
	string temp_string;
	temp_node.h_ptr=s;
	temp_node.WP=1.00; //initial probability assignment
	temp_node.hash_tag=hash(s->defn,s );
	temp_string.assign(s->defn);
	temp_node.def=temp_string;
	n.push_back (temp_node);
	nodecount++;
}
void get_add(char word[],int pos) //get senses and add to vertex
{
	SynsetPtr s;
	string temp;
	char word2[1000];
	int sum,j;
	word[strlen(word)-2]='\0';
	cout<<endl<<pos<<" "<<word;
	s=findtheinfo_ds(word,pos,-HYPERPTR,ALLSENSES);
	if(s)
	{	
		do {
			add_vertex(s);
		} while (s=s->nextss);
		
		num_words++;
		temp.assign(word);
		words.push_back(temp);	
		if(words.size()>=1)
		{	sum=0;
			for(j=0;j<num_senses.size();j++)
				sum+=num_senses[j];
			
			num_senses.push_back(nodecount-sum);
			cout << "\nNumsenses "<<nodecount-sum;
		}
		else if(words.empty())
		{
			num_senses.push_back(nodecount);
			cout << "\nNumsenses "<<nodecount;
		}
	}
	else {
		if(morphword(word,pos)!=NULL){
			strcpy(word2,morphword(word,pos));
			get_add(word2,pos);
		}
	}

}

void assign_weights()
{
	int i,j,k,startj,endj,startk;
	double w=0.0;
	for(i=0;i<num_words-1;i++)
	{	
		if(i==0)
			startj=0;
		else 
			startj=endj;
		endj=startj+num_senses[i];
		startk=endj;
		for(j=startj; j<endj; j++)
		{
			for (k=startk;k<nodecount;k++)
			{
				w=semantic_distance(n[j].h_ptr,n[k].h_ptr);
				//cout << "\nWeights= "<<w;
				if(w!=0)
				{	
					n[j].out_list.push_back(&n[k]);
					(n[j].out_weight).push_back (w);
			
					(n[k].out_list).push_back (&n[j]);
					(n[k].out_weight).push_back (w);
				
					(n[j].in_list).push_back (&n[k]);
					(n[j].in_weight).push_back (w);
				
					(n[k].in_list).push_back (&n[j]);
					(n[k].in_weight).push_back (w);
				}
			}
		}
	}
}

int converge()
{
	int i,flag=1;
	for (i=0; i<nodecount; i++)
	{
		if(n[i].WP!=temp_weight[i])
		{
			flag=0;
			n[i].WP=temp_weight[i];
		}		
	}
	return flag;
}
void calculate_WP()
{	
	int i,j,k,l;
	struct node * vb;
	struct node *va;
	
	double WP1,acc=0.0,denom=0.0,wba;
	do {
		temp_weight.clear();
		for (i=0; i<nodecount; i++) //iterate through Va all nodes
		{	
			denom=0.0;
			acc=0.0;
			va= &n[i];
			for (j=0; j<n[i].in_list.size(); j++) //all Vb that belong to In(Va)
			{	
				vb=n[i].in_list[j];
				for (k=0; k<  vb->out_weight.size(); k++) //all Vc that belong to out Vb
				{	
					denom+=vb->out_weight[k];
				}
				if(denom!=0)
				{
					for (l =0; l<vb->out_list.size(); l++)
					{
						if (vb->out_list[l]==va)
						{
							wba=vb->out_weight[l];
						}
					}
				}
				acc+=(wba* vb->WP)/denom;
			}
			WP1= 0.15 + 0.85*(acc);
			temp_weight.push_back(WP1);
		}
	}while(!converge());
			
}

void populate(char word[],int s_num) //populate initial graph
{
	char pos=mean(word);
	string temp;
	int j,sum;
	cout<<"\nPOS="<<pos;
	if(pos=='F')//F is for fail. Pervert.
	{	
		write_record1(s_num,hash(word,NULL));
		return;
		//call WRITE RECORD!"
	}
	else
	{
	   if(pos=='n')
		get_add(word,NOUN);
	   else if(pos=='v')
		get_add(word,VERB);
	   else if(pos=='a')
		get_add(word,ADJ);
	   else
		get_add(word,ADV);
			
	}
	
	
}

void display(int s_num)
{
	int i,j,max_index,start=0;
	double maximum=0.0;
	
	for (i=0; i<num_words; i++) 
	{	
		
		for (j=start; j<(start+num_senses[i]); j++) 
		{	
			if(n[j].WP>maximum)
			{
				maximum=n[j].WP;
				max_index=j;
			}
		}
		//cout << maximum<<endl;
		maximum=0.0;
		start=j;
		//cout<<endl<<"Word: "<<words[i]<<" Sense: " << (n[max_index].def)<<endl;
		write_record1(s_num,n[max_index].hash_tag);
		cout<<"\nSentence number "<<s_num<<"Hash tag="<<n[max_index].hash_tag;
	}
}

void wordsensedisambiguation()
{
	char *pch;
	wninit();
	//create_sw_map();
	ifstream infile2("perlinepos-sw.txt");
	cout << "opened";
	int q=1,i,j,sum;
	string linepos,tempy;
	while(q<=linecount)
	{
		j=1;
		num_words=0;
		getline(infile2,linepos);
		cout << linepos<<" ";
		istringstream iss (linepos,istringstream::in);
		nodecount=0;
		words.clear();
		num_senses.clear();
		n.clear();
		while(iss>>tempy)
		{	
			pch=new char [tempy.size()+1];
			strcpy(pch,tempy.c_str());
			cout<<"\nPCH= "<<pch; 
			populate(pch,q);
			cout<<" Nodecount= "<<nodecount;
			delete [] pch;
		}
		
		for (i=0; i<num_words; i++) {
			cout<<"Num_senses "<<words[i]<<" "<<num_senses[i]<<endl;
		}
	
		assign_weights();
		cout << "\nAssign weights done\n";
		calculate_WP();
		cout << "\nDone calculating\n";
		display(q);	//modify to cal max and print write record
		q++;
	}
	infile2.close();
}

