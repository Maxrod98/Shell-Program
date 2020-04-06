#include "io.h"

using namespace std;


void test1()
{
	cout << "test1" << endl;
	//string dat = "echo dat | iss here";
	vector <string> result;

	//result = split(dat, "|");
	//print(result);
}

void test2()
{
	queue<string> syms;
	queue<string> elems;

	//tokenSeparator2("awk - '{print $1$11}'","-" , elems, syms);



	print(syms);

	print(elems);

	//parse(syms, elems);
}



void test3()
{
	shell();
}

void test4()
{
	queue <string> syms;
	//syms.push("9");
	
	queue <string> parts = split("ps -aux | great < heythere", "<|>", syms, false);
	cout << "before" << endl;
	print(syms);
}



int main (){
	//test2();
	test3();
	//test4();
}


