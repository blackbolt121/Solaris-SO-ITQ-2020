#include <iostream>
#include "InstallerAsistant.h"
using namespace std;
int main(){

	char r;
	cout << "Es su primera vez en instalar (S/N): "; cin>>r;
	if(r == 's' || r == 'S'){
		InstallerAsistant aux;
		aux.installer();
	}else{
		InstallerAsistant aux = InstallerAsistant::leerArchivo("save.conf");
		aux.installer2();
	}
	
	return 0;
}