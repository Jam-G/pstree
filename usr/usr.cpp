#include<iostream>
#include<fstream>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<list>
#include<dirent.h>
using namespace std;
struct Thread_h{
	int pid;
	string name;
	int ppid;	
	list<Thread_h *> children;
};

typedef struct Thread_h TH;

const int MAX_NUM = 10000;
int count = 0;//the total number of thread
TH t_array[MAX_NUM];//the array to store thread

void getAThread(TH &t, string path){
	string prepath("/proc/");
	string flopath("/status");
	string abspath(prepath + path + flopath);
	ifstream fin(abspath.c_str());
	string temp;
	while(fin>>temp){
		if(temp == "Name:")
			fin >> t.name;
		else if(temp == "Pid:")
			fin >> t.pid;
		else if(temp == "PPid:")
			fin >> t.ppid;
	}
	fin.close(); 
}


int threadfilter(const struct dirent *dir){
	const char * s = dir->d_name;
	for(int i = 0; s[i] != '\0' ; i ++){
		if(s[i] < '0' || s[i] > '9')
			return 0;
	}
	return 1;
}

void read(){
	struct dirent ** eps;
	count = scandir("/proc", &eps, threadfilter, alphasort);
	if(count >= 0){
		for(int i = 0; i < count; i ++){
			getAThread(t_array[i], eps[i]->d_name);
		}	
	}
}

TH *findByPid(int pid){
	for(int i = 0; i < count; i ++){
		if(t_array[i].pid == pid)
			return &t_array[i];
	}
	return NULL;
}

void setChild(){
	for(int i = 0; i < count; i ++){
		if(t_array[i].ppid > 0){
			(findByPid(t_array[i].ppid) -> children).push_back(&t_array[i]);
		}
	}
}




void printTree(TH *start, int *pre = NULL, int prenum = 0, int offset = 0){
	cout << start->name;
	if(start->children.size() == 1){
		cout << "───";
		printTree(start->children.front(), pre, prenum, offset + start->name.length() + 3);
	}else if(start -> children.size() != 0){
		for(list<TH *>::iterator it = start->children.begin(); it != start->children.end(); it ++){
			int *t = new int[prenum + 1];
			for(int i = 0; i < prenum; i ++)
				t[i] = pre[i];
			t[prenum] = offset + start->name.length() + 1;
			if(*it == start->children.front()){
				cout << "─┬─";
				printTree(*it, t, prenum + 1, offset + start->name.length() + 3);
			}else if(*it != start->children.back()){
				int index = 0;
				for(int i = 0; i < offset + start->name.length(); i++){
					if(index < prenum && i == pre[index]){
						cout << "│";
						index ++;
					}
					else cout << ' ';
				}
				cout << " ├─";
				printTree(*it, t, prenum + 1, offset + start->name.length() + 3);
			}else{
				int index = 0;
                                for(int i = 0; i < offset + start->name.length(); i++){
                                        if(index < prenum && i == pre[index]){
                                                cout << "│";
                                                index ++;
                                        }
                                        else cout << ' ';
				}
				cout << " └─";
				printTree(*it, pre, prenum, offset + start->name.length() + 3);
			}
			if(t != NULL){
				delete []t;
				t = NULL;
			}
		}
	}else{
 		cout << endl;
	}
}



int main(int argc, char** argv){
	read();
	setChild();
	if(argc <= 1){
		printTree(findByPid(1));
	}else if(argc == 2){
		printTree(findByPid(atoi(argv[1])));
	}else exit(-1);
	return 0;
}	
