#include <bits/stdc++.h>
#include <sys/time.h>
#include <sys/wait.h> 
#include <sys/types.h> 
#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h> 
#include <unistd.h>

using namespace std;

namespace constant {
	double TIME = 1;
	string VERSION = "1.0.0";
	string IN_SUFFIX = ".in";
	string OUT_SUFFIX = ".out";
	//string PREFIX = "";
	string COMPLIE_ARGUMENT = "";
	string EXE = ".exe";
	string OUT = ".out";
	string ERR = ".err";
	string AC  = "\033[42m         Accept        \033[0m", AC_c  = "\033[42m";
	string CE  = "\033[43m     Compile Error     \033[0m", CE_c  = "\033[43m";
	string RE  = "\033[45m     Runtime Error     \033[0m", RE_c  = "\033[45m";
	string WA  = "\033[41m      Wrong Answer     \033[0m", WA_c  = "\033[41m";
	string TLE = "\033[44m  Time Limit Exceeded  \033[0m", TLE_c = "\033[44m";
	string MLE = "\033[46m Memory Limit Exceeded \033[0m", MLE_c = "\033[46m";
	string UKE = "\033[41m     Unknown Error     \033[0m", UKE_c = "\033[41m";
	
	string AK  = "\033[42m       All Killed      \033[0m";
	string PC  = "\033[43m   Partically Correct  \033[0m";
	string GG  = "\033[41m  E  \033[0m";
	string END_c = "\033[0m";
} using namespace constant;

namespace error {
	void access_fail(string str) {
		cout << "jdg: cannot access '" << str << "': No such file or directory" << endl;
	}
	void cannot_find_data(string str) {
		cout << "jdg: cannot find data at " << str << endl;
	}
	void data_code_missing_print() {
		cout << "jdg: missing data or code operand" << endl;
		cout << "Try 'jdg --help' for more information." << endl;
	}
	void argument_missing_print(string str) {
		cout << "jdg: option requires an argument -- '" << str << "'" << endl;
		cout << "Try 'jdg --help' for more information." << endl;
	}
	void invalid_print(string str) {
		cout << "jdg: invalid option -- '" << str << "'" << endl;
		cout << "Try 'jdg --help' for more information." << endl;
	}
	void error_print(string str) {
		cout << "jdg: some thing error at '" << str << "'" << endl;
		cout << "Try 'jdg --help' for more information." << endl; 
	}
  	void system_error() {
  		cout << "jdg: unknown system error" << endl;
  	}
} using namespace error;

namespace tool {
	bool StartWith(string mainstr, string substr) {
		if (mainstr.length() < substr.length()) return false;
		return mainstr.find(substr) == 0 ? true : false;
	}
	bool EndWith(string mainstr, string substr) {
		if (mainstr.length() < substr.length()) return false;
		return mainstr.find(substr) == mainstr.length() - substr.length() ? true : false;
	}
	vector<string>get_list(string data_dir) {
		ostringstream cmd; 	
		cmd << "cd " << data_dir << endl;
		cmd << "ls > .list" << endl;
		system(cmd.str().c_str());
		cmd.str("");
		
		string list_dir = data_dir + "/.list";
		
		ifstream fin; 
		vector<string>data_list;
		string file;
		
		fin.open(list_dir);
		while (fin >> file) data_list.push_back(file);
		fin.close();
		
		cmd << "cd " << data_dir << endl;
		cmd << "rm -rf .list" << endl;
		system(cmd.str().c_str());
		cmd.str("");
		return data_list;
	}
	int read_num(string str) {
		int u = 0, i = 0;
		for (; i < str.length() &&!isdigit(str[i]); i++);
		for (; i < str.length() && isdigit(str[i]); i++) u = u * 10 + str[i] - '0';
		return u;
	}
	string show_center(string str, int length, char blank = ' ') { // length 
		bool st = false;
		if (str.length() < length) {
			while (str.length() < length) {
				if (st) 
					str = str + blank;
				else 
					str = blank + str;
				st = !st;
			}
			return str;		
		} else if (str.length() > length) {
			string pre = "", mid = "", suf = "";
			int l = 0, r = str.length() - 1, m = min(3, max(0, length - 1));
			
			while (pre.length() + suf.length() + m < length) {
				if (st)
					pre = pre + str[l], l++;				
				else 	
					suf = str[r] + suf, r--;
				st = !st;
			}
			for (int i = 0; i < m; i++) mid = mid + ".";
			return pre + mid + suf;
		} else return str;
	}
} using namespace tool;


 
/*
Usage: jdg [OPTION]... DATA... CODE...
Judge the code with data and illustrate the results with a chart

Mandatory arguments to long options are mandatory for short options too.
  -i, --input-suffix=IN
  -o, 
  -c, -C[] 
  -t, --time-limit=SECOND      
                         set the time limit of each test with a variable
                         of type double, default value will be 1 second
  -h, --help     display this help and exit
  -v, --version  output version information and exit                         
*/

namespace option {
	bool help_check(string &str) { return str == "-h" || str == "--help";  }
	void help_print() {
		cout << "Usage: jdg [OPTION]... DATA... CODE..." << endl;
		cout << "Judge the code with data and illustrate the results with a chart" << endl;
		cout << endl;
		cout << "Mandatory arguments to long options are mandatory for short options too." << endl;
		cout << endl;
		cout << "  -t, --time=SECOND      set the time limit of each test with a variable" << endl;
		cout << "                         of type double, default value will be 1 second" << endl;
		cout << "  -h, --help     display this help and exit" << endl;
		cout << "  -v, --version  output version information and exit" << endl;
		cout << endl;
	}

	bool version_check(string &str) { return str == "-v" || str == "--version"; }
	void version_print() {
		cout << "jdg (LocalJudge) " << VERSION << endl;
		cout << "Copyright (C) 2018 " << endl;
		cout << "Code by Anoxiacxy. <https://anoxiacxy.github.io>" << endl;
	}

	bool time_check(string str) { if (!(StartWith(str, "-t") || StartWith(str, "--time"))) return false; }
	void time_set(string str) {
		int pos = str.find('=');
		if (pos == -1) {
			if (StartWith(str, "-t")) { argument_missing_print("-t"); exit(-1); }
			if (StartWith(str, "--time")) { argument_missing_print("--time"); exit(-1); }
			error_print(str); exit(0);
		}
		istringstream sin(str.substr(pos + 1, str.length() - pos - 1));
		sin >> TIME;
	}
} using namespace option;

namespace check {
	void complie(string data_dir, string code_dir) {
		ostringstream cmd;
		cmd << "g++ " << COMPLIE_ARGUMENT << " " << code_dir << " -o " << data_dir << "/" << EXE << endl;
		if (system(cmd.str().c_str())) exit(-1);
	}

	void clean(string data_dir) {
		ostringstream cmd;
		cmd << "cd " << data_dir << endl;
		cmd << "rm " << EXE << endl;
		cmd << "rm " << OUT << endl;
		cmd << "rm " << ERR << endl;
		if (system(cmd.str().c_str())) exit(-1);
	}

	struct Result {
		string type;
		string name;
		double score;
		double time;
		double memory;
		void print() {
			char tmp[20];
			sprintf(tmp, "%.3fms", time * 1000);

			// name length = 16
			// type length = 23
			// score length = 9
			// time length = 12
			name = show_center(name, 12);
			string time_s(tmp), score_s; 
			if (score == int(score))
				score_s = show_center(to_string(int(score)), 5);
			else {
				sprintf(tmp, "%.1f", score);
				score_s = show_center(string(tmp), 5);
			}
			if (time < 0)
				time_s = show_center("oo", 13);
			else 
				time_s = show_center(time_s, 13);
			cout << "|  " << name << "  |   " << type << "   |   " << score_s << "   |   " << time_s << "   |" << endl;  
			cout << "--------------------------------------------------------------------------------" << endl;
		}
	};

	struct Result test(string data_dir, pair<string, string>data_test) {
		pid_t timepid, codepid;
		struct timeval start, end, time;
		data_test.first  = data_dir + "/" + data_test.first;
		data_test.second = data_dir + "/" + data_test.second;
		string output = data_dir + "/" + ".out", errput = data_dir + "/" + ".err", temp;
		codepid = fork();
		if (codepid == 0) {
			
			//exec();
			ostringstream cmd;
			cmd << "cd " << data_dir << endl;			
			cmd << "./" << EXE << " < " << data_test.first << " 1> " << output << " 2>" << errput << endl;
			system(cmd.str().c_str());
			//
			exit(EXIT_SUCCESS);
		}
		timepid = fork();
		if (timepid == 0) {
			usleep(TIME * 1e6);
 			exit(EXIT_SUCCESS);
		}
		
		
		gettimeofday(&start, NULL);
		pid_t firstpid = wait(NULL);
		gettimeofday(&end, NULL);
		timersub(&end, &start, &time);
		
		kill(timepid ^ codepid ^ firstpid, SIGKILL);
		
		wait(NULL);
		
		Result rst;
		/*
		time
		type
		score
		*/
		if (firstpid == timepid) {//TLE
			rst.time = -1;
			rst.type = TLE;
			rst.score = 0;
			return rst;
		}
		
		rst.time = time.tv_sec + time.tv_usec / 1e6;
		
		ifstream fin(errput);
		if (fin >> temp) {//RE
			rst.type = RE;
			rst.score = 0;
			return rst;
		}
		fin.close();
		
		ostringstream cmd;
		cmd << "diff -b -q " << data_test.second << " " << output << " > " << errput << endl;	
		fin.open(errput);
		if (fin >> temp) {//WA
			rst.type = WA;
			rst.score = 0;
			return rst;
		}
		fin.close();
		
		rst.score = 100;
		rst.type = AC;		
		return rst;
	}
	
	void work(string data_dir, string code_dir) {
		if (access(data_dir.c_str(), 0) == -1) access_fail(data_dir), exit(-1);
		if (access(code_dir.c_str(), 0) == -1) access_fail(code_dir), exit(-1);	
		
		vector<string>data_list = get_list(data_dir);
		
		//cout << "list()" << endl;
		
		map<int, pair<string, string> > test_data;
		
		for (auto str : data_list) {
			if (EndWith(str, IN_SUFFIX)) 
				test_data[read_num(str)].first = str;	
			
			if (EndWith(str, OUT_SUFFIX)) 
				test_data[read_num(str)].second = str;	
		}
		
		if (!test_data.size()) cannot_find_data(data_dir), exit(-1);
		
		string name = "";
		string temp = test_data.begin()->second.first.length() ? 
			test_data.begin()->second.first : test_data.begin()->second.second;
		
		for (int i = 0; i < temp.length() && !isdigit(temp[i]); i++) name += temp[i];
		
		//cout << "complie()" << endl;	
			
		complie(data_dir, code_dir);
		
		//cout << "test()" << endl;
		
		string head = show_center("==" + name + "==", 80);
		
		cout << head << endl;
		cout << "--------------------------------------------------------------------------------" << endl;			
		
		Result all_rst;
		all_rst.score = 0;
		all_rst.time = 0;
		all_rst.name = name;
		
		bool ak = true, gg = true;
		
		
		
		for (auto test_p : test_data) {
			struct Result rst = test(data_dir, test_p.second);
			rst.name = name + to_string(test_p.first);
			rst.score *= (1.0 / test_data.size());
			
			all_rst.score += rst.score;
			all_rst.time += max(0.0, rst.time);
			ak &= rst.type == AC;
			gg &= rst.type != AC;
			
			rst.print();			
		}
		
		if (gg) all_rst.type = GG;
		else if (ak) all_rst.type = AK;
		else all_rst.type = PC;
		
		all_rst.print();
		
		clean(data_dir);
	}
} using namespace check;

int main (int argc, char const* argv[])
{
	vector<string>option;
	for (int i = 1; i < argc; i++) 
		if (argv[i][0] == '-') 
			option.push_back(string(argv[i]));
	
	for (auto str : option) {
		if (help_check(str)) { help_print(); exit(0); }
		else 
		if (version_check(str)) { version_print(); exit(0); }
		else 
		if (time_check(str)) { time_set(str); }
		else { invalid_print(str); exit(-1); }
	}
			
	//
	
	string data_dir, code_dir;
	
	 for (int i = 1; i < argc; i++) if (argv[i][0] != '-') {
		if (!data_dir.length()) data_dir = argv[i];
		else 
		if (!code_dir.length()) code_dir = argv[i];
		else {
			
			//
			
		}
	}
	
	if (!code_dir.length() || !data_dir.length()) data_code_missing_print(), exit(-1);
		
	work(data_dir, code_dir); 
		 	
	return 0;
}
