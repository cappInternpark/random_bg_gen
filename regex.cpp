#include <iostream>
#include <string>
#include <regex>

using namespace std;
/*
int main()
{
	//string s = "TunaCan_0_12.jpg";
	string s = "TunaCan_1_12.jpg";
	//regex number("[0-9]+");
	string number("[0-9]+");
	string answer = "TunaCan_0_" + number +".jpg" ;
	regex reg_answer(answer);
	//regex answer("TunaCan_0_[0-9]+.jpg");

	if(regex_match(s, reg_answer)){
		cout<<"CORECT : "<<s<<endl;
	}else{
		cout<<"WRONG : "<<s<<endl;
	}
	return 1;
}
*/
bool is_reg_match(string target, string answer){
	regex reg_answer(answer);
	if(regex_match(target, reg_answer)){
		return true;
	}else{
		return false;
	}
}
int main()
{
	//string s = "TunaCan_0_12.jpg";
	string s = "TunaCan_0_12.jpg";
	//regex number("[0-9]+");
	string number("[0-9]+");
	string answer = "TunaCan_0_" + number +".jpg" ;

	if(is_reg_match(s, answer)){
		cout<<"CORECT : "<<s<<endl;
	}else{
		cout<<"WRONG : "<<s<<endl;
	}
	return 1;
}

