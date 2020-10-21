#include <ctime>
#include <string>

using namespace std;


string getDateStr(){
	string out = "";
	time_t now = time(0);
	
	tm *ltm = localtime(&now);
	
   out.append( to_string(1900 + ltm->tm_year)); out.append("_");
   out.append( to_string(1 + ltm->tm_mon) ); out.append("_");
   out.append( to_string(ltm->tm_mday) ); out.append("-");
   out.append( to_string(ltm->tm_hour) ); out.append("h");
   out.append( to_string(ltm->tm_min) ); out.append("m");
   out.append( to_string(ltm->tm_sec) ); out.append("s");
   
   return out;
}
