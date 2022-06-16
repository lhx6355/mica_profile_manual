
/* This is for debug */
//#include "random32.cpp"
//#include "genRandInstStruct.h"
//by pz
#include "statistic_trace.h"
#include <string>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <Windows.h>

//#define TEST_SINGLE_INST									//手动输入测试的指令
//#define NOINTERVAL	//解决不按interval分析情况下的空格问题
//example arg: E:\VS2022\projects\mica_profile\instrace.new.log E:\VS2022\projects\mica_profile 100000 instrace.test AARCH64
int main(int argc, char* argv[])
{
	if (argc == 6)
	{
		//读取参数
		char* file_in = argv[1];
		char* project_path = argv[2];
		char* size_char = argv[3];

		char name[FILENAME_SIZE];
		strcpy(name, argv[4]);
		std::strcat(name, ".txt");
		char* arch = argv[5];

		//char型size转换为int型
		int size = 0;
		int tmp;
		int len = strlen(size_char);
		for (int i = 0; i < len; i++) {
			tmp = size_char[i] - '0';
			size *= 10;
			size += tmp;
		}

		//文件名的连接
		char path_out[FILENAME_SIZE];
		strcpy(path_out, project_path);
		strcat(path_out, "\\Trace_Summary");

		char* file_fea_path = (char*)malloc(sizeof(char) * FILENAME_SIZE);
		char* file_mica_path = (char*)malloc(sizeof(char) * FILENAME_SIZE);

		char copypath[FILENAME_SIZE];
		strcpy(copypath, path_out);
		strcat(copypath, "%s%s");									  								//构建一个用于sprintf_s的输出格式
		sprintf_s(file_fea_path, FILENAME_SIZE, copypath, "fea\\", name);							//输出到file_fea_path路径中，E:\VS2022\projects\readtostr\Trace_Summaryfea\name
		sprintf_s(file_mica_path, FILENAME_SIZE, copypath, "mica\\", name);

		/*删除已有文件*/
		std::remove(file_fea_path);
		std::remove(file_mica_path);

#ifndef TEST_SINGLE_INST
		int i = 0;
		string line;
		string pcstr;				//含pc的string
		string opcodestr1;			//用于判断ISET
		string opcodestr;
		ifstream in(file_in, ios::in);
		int n = 1;					//第n个interval

		if (!in.is_open())
		{
			std::cout << "Error: opening file fail" << endl;
		}
		else

			while (!in.eof())
			{
				InstTrace* instTrace;
				instTrace = new InstTrace[size];

				if (strcmp(arch, "AARCH64") == 0)			//64位trace
				{
					//运行一个interval
					while (i < size && (!in.eof()))
					{
						getline(in, line);			//提取一行		
						if (line == "")
							break;
						instTrace[i].inst_iset = ISET_AARCH64;
						int firstSpaceNum = line.find(' ');
						int secondSpaceNum = line.find("  0x");
						int len_of_Line = line.length();

						if (secondSpaceNum != string::npos)		//是访存指令
						{
							//提取PC
							pcstr = line.substr(0, firstSpaceNum);
							instTrace[i].pc = stoull(pcstr, 0, 16);
							instTrace[i].inst_paddr = instTrace[i].pc;

							//提取opcode
							opcodestr = line.substr(firstSpaceNum + 2, secondSpaceNum - firstSpaceNum - 2);
							instTrace[i].opcode = stoull(opcodestr, 0, 16);

							//提取访存地址
							string paddr_str = line.substr(secondSpaceNum + 2, len_of_Line - secondSpaceNum - 2);
							instTrace[i].load_paddr = stoull(paddr_str, 0, 16);
							instTrace[i].store_paddr = instTrace[i].load_paddr;
						}

						else									//不是访存指令
						{
							//提取PC
							pcstr = line.substr(0, firstSpaceNum);
							instTrace[i].pc = stoull(pcstr, 0, 16);
							instTrace[i].inst_paddr = instTrace[i].pc;

							//提取opcode
							opcodestr = line.substr(firstSpaceNum + 2, len_of_Line - firstSpaceNum - 2);
							instTrace[i].opcode = stoull(opcodestr, 0, 16);

						}

						++i;
					}
				}

				else if (strcmp(arch, "AARCH32") == 0)			//32位trace
				{
					//	运行一个interval
					while (i < size && (!in.eof()))
					{
						getline(in, line);			//提取一行		
						if (line == "")
							break;
						getline(in, line);			//提取一行		
						int firstSpaceNum = line.find(' ');
						int secondSpaceNum = line.find("  0x");
						int len_of_Line = line.length();

						if (secondSpaceNum != string::npos)		//是访存指令
						{
							//根据opcode长度判断是arm还是thumb
							if (secondSpaceNum - firstSpaceNum - 2 == 8)
								instTrace[i].inst_iset = ISET_ARM;
							else
								instTrace[i].inst_iset = ISET_THUMB;

							//提取PC
							pcstr = line.substr(0, firstSpaceNum);
							instTrace[i].pc = stoull(pcstr, 0, 16);
							instTrace[i].inst_paddr = instTrace[i].pc;

							//提取opcode
							opcodestr = line.substr(firstSpaceNum + 2, secondSpaceNum - firstSpaceNum - 2);
							if (secondSpaceNum - firstSpaceNum - 2 == 9)		//thumb2指令
							{
								opcodestr = opcodestr.erase(4, 1);
							}

							instTrace[i].opcode = stoull(opcodestr, 0, 16);

							//提取访存地址
							string paddr_str = line.substr(secondSpaceNum + 2, len_of_Line - secondSpaceNum - 2);
							instTrace[i].load_paddr = stoull(paddr_str, 0, 16);
							instTrace[i].store_paddr = instTrace[i].load_paddr;
						}

						else									//不是访存指令
						{
							if (len_of_Line - firstSpaceNum - 2 == 8)
								instTrace[i].inst_iset = ISET_ARM;
							else
								instTrace[i].inst_iset = ISET_THUMB;

							//提取PC
							pcstr = line.substr(0, firstSpaceNum);
							instTrace[i].pc = stoull(pcstr, 0, 16);
							instTrace[i].inst_paddr = instTrace[i].pc;

							//提取opcode
							opcodestr = line.substr(firstSpaceNum + 2, len_of_Line - firstSpaceNum - 2);
							if (len_of_Line - firstSpaceNum - 2 == 9)		//thumb2指令
							{
								opcodestr = opcodestr.erase(4, 1);
							}

							instTrace[i].opcode = stoull(opcodestr, 0, 16);
						}

						++i;
					}
				}

				else
				{
					cout << "wrong arch format！" << endl;
					break;
				}

				std::cout << "running:interval_" << n << "......" << endl;

				statisticTrace(instTrace, size, file_fea_path, file_mica_path);

				++n;
#ifdef NOINTERVAL
				if (i == size )
					break;
#endif
				i = 0;
			}
		in.close();
		std::system("pause");


#endif

#ifdef TEST_SINGLE_INST
		//测试手动输入的指令
		InstTrace* instTrace;
		instTrace = new InstTrace[1];

		instTrace[0].pc = 0xefeed7d8;
		instTrace[0].opcode = 0xf943aa80;
		instTrace[0].inst_paddr = instTrace[0].pc;
		instTrace[0].inst_iset = ISET_THUMB;
		instTrace[0].load_paddr = 1;

		statisticTrace(instTrace, 1, file_fea_path, file_mica_path);

		system("pause");
#endif
	}
	else cout << "wrong arg format！" << endl;
}


/*stringstream ss;
stringstream dd;
int i = 0;

string line;
string pcstr;
string opcodestr1;
string opcodestr;
ifstream in("trace-pc.txt", ios::in);

if (!in.is_open())
{
	cout << "Error: opening file fail" << endl;
	exit(1);
}
else

	while (i < SIZE && (!in.eof()))
	{
		getline(in, line);			//提取一行
		pcstr = line.substr(2, 10);	//提取PC
		instTrace[i].pc = stoull(pcstr, 0, 16);

		opcodestr1 = line.substr(14, 8);			//提取opcode前8位以判断ISET类型
		if (opcodestr1.find(' ') != string::npos) instTrace[i].inst_iset = ISET_THUMB;
		else instTrace[i].inst_iset = ISET_ARM;
		opcodestr = line.substr(14, 9);
		opcodestr.erase(std::remove(opcodestr.begin(), opcodestr.end(), ' '), opcodestr.end());			//去掉空格
		instTrace[i].opcode = stoull(opcodestr, 0, 16);

		i++;
	}
in.close();*/






//serial block size
/*instTrace[0].opcode = 0;
instTrace[1].opcode = 0xD503309F;
instTrace[2].opcode = 0;
instTrace[3].opcode = 0;
instTrace[4].opcode = 0;
instTrace[5].opcode = 0;
instTrace[6].opcode = 0;
instTrace[7].opcode = 0;
instTrace[8].opcode = 0xD503309F;	//size = 6 >> ans = 3
instTrace[9].opcode = 0;
instTrace[10].opcode = 0xD503309F;	//size = 1 >> ans = 1
*/
//branth beehavior
/*instTrace[0].inst_paddr = 50000;
instTrace[0].opcode = 0x14000000;
instTrace[0].pc = 100;

instTrace[1].opcode = 0;
instTrace[1].pc = 200;//jump backward

instTrace[2].opcode = 0;
instTrace[3].opcode = 0;
instTrace[4].opcode = 0;
instTrace[5].opcode = 0;
instTrace[6].opcode = 0;

instTrace[7].opcode = 0x14000000;
instTrace[7].pc = 500;
instTrace[7].inst_paddr = 1200;	//block size is 488 -> 10

instTrace[8].opcode = 0;
instTrace[8].pc = 504;//jump notTaken


instTrace[9].opcode = 0x14000000;
instTrace[9].pc = 300;
instTrace[9].inst_paddr = 1200;//block size is 0

instTrace[10].opcode = 0;
instTrace[10].pc = 400;//jump backward
instTrace[10].inst_paddr = 500;

instTrace[11].opcode = 0x14000000;
instTrace[11].pc = 1000;
instTrace[11].inst_paddr = 500;//block size is 7

instTrace[12].opcode = 0;
instTrace[12].pc = 200;//jump forward
*/
// reg dependency
/*instTrace[0].opcode = 0x5AC00841;	//1 <- 2
instTrace[1].opcode = 0x5AC00883;	//3 <- 4
instTrace[2].opcode = 0x5AC008C5;	//5 <- 6
instTrace[3].opcode = 0x5AC00907;	//7 <- 8
instTrace[4].opcode = 0x5AC00829;	//9 <- 1 -- 4
instTrace[5].opcode = 0x5AC00862;	//2 <- 3
instTrace[6].opcode = 0x5AC0096A;	//10 <- 11
instTrace[7].opcode = 0x5AC009AC;	//12 <- 13
instTrace[8].opcode = 0x5AC0082E;	//14 <- 1 --8
instTrace[9].opcode = 0x5AC0082E;	//14 <- 1 --9
instTrace[10].opcode = 0x5AC00A00;	//16 <- 0
//inst locality
instTrace[0].inst_paddr = 100;
instTrace[1].inst_paddr = 200;
instTrace[2].inst_paddr = 300;
instTrace[3].inst_paddr = 400;
instTrace[4].inst_paddr = 500;
instTrace[5].inst_paddr = 600;
instTrace[6].inst_paddr = 700;
instTrace[7].inst_paddr = 300;
instTrace[8].inst_paddr = 800;
instTrace[9].inst_paddr = 900;
instTrace[10].inst_paddr = 900;
*/
//regDependLength
/*instTrace[0].opcode = 0x5AC00841;	//1 <- 2
instTrace[1].opcode = 0x5AC00883;	//3 <- 4
instTrace[2].opcode = 0x5AC008C5;	//5 <- 6
instTrace[3].opcode = 0x5AC00907;	//7 <- 8
instTrace[4].opcode = 0x5AC00829;	//9 <- 1 -- 4
instTrace[5].opcode = 0x5AC00862;	//2 <- 3
instTrace[6].opcode = 0x5AC0096A;	//10 <- 11
instTrace[7].opcode = 0x5AC009AC;	//12 <- 13
instTrace[8].opcode = 0x5AC0082E;	//14 <- 1 --8
instTrace[9].opcode = 0x5AC0082E;	//14 <- 1 --9
instTrace[10].opcode = 0x5AC00A00;	//16 <- 0
instTrace[11].opcode = 0x5AC00A20;	//17 <- 0
*/
//Load Locality(both temp and spat)
/*	instTrace[0].opcode = 0x58000000;
	instTrace[0].load_paddr = 500;
	instTrace[0].pc = 1000;

	instTrace[1].opcode = 0x58000000;
	instTrace[2].opcode = 0x58000000;//

	instTrace[3].opcode = 0x58000000;
	instTrace[3].load_paddr = 500;          // reuseLocal is 2
	instTrace[3].pc = 1000;		// reuseGlobal is 2

	instTrace[4].opcode = 0x58000000;
	instTrace[5].opcode = 0x58000000;
	instTrace[6].opcode = 0x58000000;
	instTrace[7].opcode = 0x58000000;
	instTrace[8].opcode = 0x58000000;

	instTrace[9].opcode = 0x58000000;
	instTrace[9].load_paddr = 5000;
	instTrace[9].pc = 0;	// reuseGlobal is 4

	instTrace[10].opcode = 0x58000000;
	instTrace[10].load_paddr = 500;
	instTrace[10].pc = 100;	//ldStGlobalspac+1;
*/
// st instTrace data locality
/*instTrace[0].opcode = 0x2c800000;//2c800000
instTrace[0].store_paddr = 500;
instTrace[0].pc = 1000;

instTrace[1].opcode = 0x2c800000;
instTrace[2].opcode = 0x2c800000;//

instTrace[3].opcode = 0x2c800000;
instTrace[3].store_paddr = 500;          // reuseLocal is 2
instTrace[3].pc = 1000;		// reuseGlobal is 2

instTrace[4].opcode = 0x2c800000;
instTrace[5].opcode = 0x2c800000;
instTrace[6].opcode = 0x2c800000;
instTrace[7].opcode = 0x2c800000;
instTrace[8].opcode = 0x2c800000;

instTrace[9].opcode = 0x2c800000;
instTrace[9].store_paddr = 5000;
instTrace[9].pc = 0;	// reuseGlobal is 4

instTrace[10].opcode = 0x2c800000;
instTrace[10].store_paddr = 500;
instTrace[10].pc = 100;	//ldStGlobalspac+1;
*/
// critical path
/*	instTrace[0].opcode = 0x110002A5; // Rn = 21, Rd = 5
	instTrace[1].opcode = 0x110000B4; // Rn = 5, Rd = 20
	instTrace[2].opcode = 0x11000281; // Rn = 20, Rd = 1
	instTrace[3].opcode = 0x11000036; // Rn = 1, Rd = 22
	instTrace[4].opcode = 0x110002D5; // Rn = 22, Rd = 21
	instTrace[5].opcode = 0x1100014B; // Rn = 10, Rd = 11
	instTrace[6].opcode = 0x1100014B; // Rn = 10, Rd = 11
	instTrace[7].opcode = 0x1100014B; // Rn = 10, Rd = 11
	*/
	// FP critical path
	/*	instTrace[6].opcode = 0x1E674815; // Rn=0, Rm=7, Rd=21
		instTrace[5].opcode = 0x1E664807; // Rn=0, Rm=6, Rd=7
		instTrace[4].opcode = 0x1E684806; // Rn=0, Rm=8, Rd=6
		instTrace[3].opcode = 0x1E694808; // Rn=0, Rm=9, Rd=8
		instTrace[2].opcode = 0x1E6A4809; // Rn=0, Rm=10, Rd=9
		instTrace[1].opcode = 0x1E6A4960; // Rn=11, Rm=10, Rd=0
		instTrace[0].opcode = 0x1E6F4AAB; // Rn=21, Rm=15, Rd=11
		*/

		// 
		/*instTrace[0].opcode = 0;
		instTrace[1].opcode = 0xD503309F;
		instTrace[2].opcode = 0;
		instTrace[3].opcode = 0;
		instTrace[4].opcode = 0;
		instTrace[5].opcode = 0;
		instTrace[6].opcode = 0;
		instTrace[7].opcode = 0;
		instTrace[8].opcode = 0xD503309F;	//size = 6 >> ans = 3
		instTrace[9].opcode = 0;
		instTrace[10].opcode = 0xD503309F;	//size = 1 >> ans = 1
		*/

