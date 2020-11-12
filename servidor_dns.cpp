#include <iostream>
#include <string>
#include <fstream>
using namespace std;
void capturarDatos();
void archivoDNS();
void archivoDNSReverso();
void namedFile();
void instrucciones();
void instalator();
void action(const string s);

string dominio, ip2="", ip, reverse_ip, hostname, narch, command, porcion_host;
int main(){


	
	
	return 0;
}
void installer(){
	//Creamos los archivos correspondientes
	capturarDatos();
	archivoDNS();
	action("ls -l");
	archivoDNSReverso();
	action("ls -l");
	namedFile();
	action("ls -l");

	//Guardamos el nombre de nuestros archivos
	char rpt;
	string filename1 = dominio + ".db";
	string filename2 = reverse_ip + ".db";
	action("mkdir /var/dump");
	action("mkdir /var/stats");
	action("mkdir -p /var/run/namedb");
	action("mkdir -p /etc/namedb/master");
	action("mkdir -p /etc/namedb/working");

	//Movemos los archivos a las carpetas correspondientes
	action("mv named.conf /etc/");
	action("mv " + filename1 + "/etc/namedb/master/");
	action("mv " + filename2 + "/etc/namedb/master/");

	//Validamos que la configuracion de nuestros archivos funcionen
	cout << "Moviendo el archivo de configuracion a /Etc/" << endl;
	action("named-checkconf /etc/named.conf");
	cout << "Moviendo archivo de configuracion DNS a /var/namedb/master" << endl;
	action("named-checkzone " + dominio + "/var/namedb/master/"+filename1);
	cout << "Moviendo archivo de configuracion DNS reverso a /var/namedb/master" << endl;
	action("named-checkzone " + dominio + "/var/namedb/master/"+filename2);

	cout << "Desea habilitar el servidor DNS (S/N): " << endl;
	cin >> rpt;
	if(!(rpt == 's' || rpt == 'S')){
		exit(0);
	}

	//Agregamos a nuestro cliente dns local nuestro servidor dns
	cout << "Habilitando su servidor DNS en el cliente del host local..." << endl;
	action("svccfg -s network/dns/client setprop config/nameserver = net_address: " + ip);
	action("svccfg -s network/dns/client setprop config/domain = astring: " + dominio);
	action("svccfg -s network/dns/client setprop config/search = astring: " + dominio);
	action("svccfg -s name-service/switch setprop config/ipnodes = astring: '\"files dns\"'");
	action("svccfg -s name-service/switch setprop config/host = astring: '\"files dns\"'");
	action("svccfg -s network/dns/client listprop config");

}
void capturarDatos(){
	cout << "Digite su dominio: "; getline(cin, dominio, '\n');
	cout << "Digite la ip de su servidor DNS: "; getline(cin, ip, '\n');
	cout << "Digite el nombre de su hostname: "; getline(cin, hostname, '\n');
	cout << "Digite la porcion de host: "; getline(cin, porcion_host, '\n');
}
void archivoDNSReverso(){
	string n[3];
	string var = "";
	for(int i = 0, cont = 0; i<ip.length(); i++){
		if(cont<3){
			if(ip[i]=='.' && cont<3){
				n[cont] = var;
				var="";
				cont++;

			}else if(cont<3){
				var+=ip[i];
			}else
			{
				break;
			}
		}
	}
	var="";
	for(int i = 2; i>=0; i--){
		var+=n[i];
		if(i!=0)
			var+=".";
	}
	ip2 = var;
	narch = ip2 + ".db";
	ofstream arch(narch.c_str());
	if(arch.is_open()){
		arch << "$TTL 3h" << endl;
		arch << "@       IN      SOA	"<< hostname <<"."<<dominio << ". " << "root."<<dominio<< ". (" <<endl;
		arch << "        2013022744" << endl  << "        28800" << endl << "        3600" << endl;
		arch << "        604800" << endl << "        38400" << endl << ")" << endl;
		arch << "@" << "\t\tIN\t\tNS\t\t" << hostname << "." << dominio << "." << endl;
		arch << "@" << "\t\tIN\t\tPTR\t\t" << ip << "." << endl;
		arch << "DNS" << "\t\tIN\t\tA\t\t" << ip << endl;
		arch << hostname << "\t\tIN\t\tA\t\t" << ip << endl;
		arch << porcion_host << "\t\tIN\t\tPTR\t\t" << hostname << "." << dominio;
	}
	arch.close();
	getchar();
}
void archivoDNS()
{
	narch = dominio + ".db";
	ofstream arch(narch.c_str());
	if(arch.is_open()){
		arch << "$TTL 3h" << endl;
		arch << "@       IN      SOA	"<< hostname <<"."<<dominio << ". " << "root."<<hostname<<"."<<dominio<< ". (" <<endl;
		arch << "        2013022744" << endl  << "        28800" << endl << "        3600" << endl;
		arch << "        604800" << endl << "        38400" << endl << ")" << endl;
		arch << dominio << ".\t\tIN\t\tNS\t\t" << hostname << "." << dominio << "." << endl;
		arch << hostname << "\t\tIN\tA\t" << ip << endl;
	}
	arch.close();
	getchar();
}
void namedFile(){
	narch = "named.conf";
	ofstream arch(narch.c_str());
	if(arch.is_open()){

		arch << "options {" << endl;
		arch << "\tdirectory\t\"/etc/namedb/working\";" << endl;
		arch << "\tpid-file\t\"/var/run/named/pid\";" << endl;
		arch << "\tdump-file\t\"/var/dump/named_dump.db\";" << endl;
		arch << "\tstatistics-file\t\"/var/stats/named.stats\";" << endl;
		arch << "};" << endl;

		arch << "zone " << "\"" << dominio << "\" {" << endl;
		arch << "\ttype master;" << endl;
		arch << "\tfile \"/etc/namedb/master/" << dominio << ".db\";" << endl;
		arch << "};" << endl;

		arch << "zone \"" << reverse_ip << ".in-addr.arpa\" {" << endl;
		arch << "type master;" << endl;
		arch << "file \"/etc/namedb/master/" << reverse_ip << ".db\";" << endl;
		arch << "};" << endl;
		arch.close();
		getchar();
	}
}
void action(const string s){

	system(s.c_str());
	getchar();
}
