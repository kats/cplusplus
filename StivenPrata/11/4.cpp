#include <iostream>
#include "mytime.h"
int main()
{
	using std::cout; using std::endl;
	Time aida(3, 35) ;
	Time tosca (2, 48);
	Time temp;
	cout << "Aida and Tosca:" << endl;
	cout << aida << "; " << tosca << endl;
	temp = aida + tosca; // operator ()
	cout << "Aida + Tosca: " << temp << endl;
	temp = aida * 1.17; // функция-член operator*()
	cout << "Aida * 1.17: " << temp << endl;
	cout << "10.0 * Tosca: " << 10.0 * tosca << endl;
	return 0;
}