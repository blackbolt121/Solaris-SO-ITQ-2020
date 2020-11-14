#include <iostream>
#include <string>
#include <fstream>
using namespace std;
void capturarDatos();
void archivoDNS();
void archivoDNSReverso();
void crearArchivoDHCPv4();
void crearArchivoDHCPv6();
void namedFile();
void instrucciones();
void installer();
void crearArchivoApagadoReinicio();
void action(const string s);

string dominio, ip2="", ip, reverse_ip, hostname, narch, porcion_red="", porcion_host="", interfaz="", rango_inferior="", rango_superior="";
int main(){

	installer();
	

	
	return 0;
}
void installer(){
	crearArchivoApagadoReinicio();
	//Habilitamos SSH
	system("rm *.db");
	system("rm *.conf");
	system("svcadm enable ssh");
	system("pkg update");
	
	cout << "Generando archivos de configuracion necesarios...." << endl << "Rellene los campos del formulario..." << endl;
	capturarDatos();
	cout << "Creando archivos de DHCPv4 " << endl;
	crearArchivoDHCPv4();
	cout << "Creando archivos de DHCPv6 " << endl;
	crearArchivoDHCPv6();
	archivoDNS();
	action("ls -l");
	archivoDNSReverso();
	action("ls -l");
	namedFile();
	action("ls -l");
	system("clear");
	
	cout << "Se mueve el archivo de configuracion de DHCPv4 a /etc/inet " << endl;
	action("mv dhcpd4.conf /etc/inet");
	cout << "Se mueve el archivo de configuracion de DHCPv6 a /etc/inet " << endl;
	action("mv dhcpd6.conf /etc/inet");
	cout << "Activando servidor DHCPv4..." << endl;
	action("svcadm enable dhcp/server:ipv4");
	action("svcadm enable dhcp/server:ipv6");
	//Instalamos el servicio de DNS
	action("package install pkg install service/network/dns/bind");
	//Creamos los archivos correspondientes
	//Guardamos el nombre de nuestros archivos
	char rpt;
	string filename1 = dominio + ".db";
	string filename2 = reverse_ip + ".db";
	action("mkdir /var/dump");
	action("mkdir /var/stats");
	action("mkdir -p /var/run/namedb || echo \"listo directorio /var/run/namedb/ ya creado\"");
	action("mkdir -p /etc/namedb/master || echo \"listo directorio /etc/namedb/master ya creado\"");
	action("mkdir -p /etc/namedb/working || \"listo directorio /etc/namedb/workingr ya creado\"");

	//Movemos los archivos a las carpetas correspondientes
	action("mv named.conf /etc/");
	cout << "Moviendo: " << filename1 << endl;
	action("mv -f " + filename1 + " /etc/namedb/master/");
	cout << "Moviendo: " << filename2 << endl;
	action("mv -f "  + filename2 + " /etc/namedb/master/");

	//Validamos que la configuracion de nuestros archivos funcionen
	cout << "Moviendo el archivo de configuracion a /etc/" << endl;
	action("named-checkconf /etc/named.conf");
	cout << "Moviendo archivo de configuracion DNS a /var/namedb/master" << endl;
	action("named-checkzone " + dominio + " /etc/namedb/master/"+filename1);
	cout << "Moviendo archivo de configuracion DNS reverso a /var/namedb/master" << endl;
	action("named-checkzone " + dominio + " /etc/namedb/master/"+filename2);

	cout << "Desea habilitar el servidor DNS (S/N): " << endl;
	cin >> rpt;
	if(!(rpt == 's' || rpt == 'S')){
		exit(0);
	}else{
		cout << "Habilitando servicio DNS... " << endl;
		cout << "Refrescando el servicio DNS..." << endl;
		action("sudo svcadm refresh dns/server");
		cout << "Habilitando el servicio DNS..." << endl;
		action("sudo svcadm enable dns/server");
		cout << "Mostrando el status del servicio..." << endl;
		action("svcs dns/server");


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
	//Este metodo solo sirve para redes ccon una IP clase C con una submascara de red /24
	cout << "Digite su dominio: "; getline(cin, dominio, '\n');
	cout << "Digite la ip de su servidor DNS: "; getline(cin, ip, '\n');
	cout << "Digite el nombre de su hostname: "; getline(cin, hostname, '\n');
	cout << "Digite el nombre de la interfaz: "; getline(cin, interfaz, '\n');
	cout << "Digite el rango inferior de su servidor DHCP: "; getline(cin, rango_inferior, '\n');
	cout << "Digite el rango superior de su servidor DHCP: "; getline(cin, rango_superior, '\n');
	string n[4]; //Arreglo para guardar los digitos de la ip inversa
	string var = "";
	for(int i = 0, cont = 0; i<ip.length(); i++){
		if(cont<4){
			if(ip[i]=='.' && cont<4){
				n[cont] = var;
				var="";
				cont++;
			}
			if(ip[i]!='.' && ip[i]!='\n'){
				var+=ip[i];
			}	
		}
	}
	porcion_host = var;
	var = "";
	for(int i = 0; i<3; i++){
		porcion_red += n[i];
		if ( i != 2) {
			porcion_red +=".";
		}
	}
	var="";
	for(int i = 2; i>=0; i--){
		var+=n[i];
		if(i!=0)
			var+=".";
	}
	reverse_ip = var;
}
void archivoDNSReverso(){

	//Se genera el archivo reverso
	narch = reverse_ip + ".db";
	ofstream arch(narch.c_str());
	if(arch.is_open()){
		arch << "$TTL 3h" << endl;
		arch << "@       IN      SOA	"<< hostname <<"."<<dominio << ". " << "root."<<dominio<< ". (" <<endl;
		arch << "        2013022744" << endl  << "        28800" << endl << "        3600" << endl;
		arch << "        604800" << endl << "        38400" << endl << ")" << endl;
		arch << "@" << "\t\tIN\t\tNS\t\t" << hostname << "." << dominio << "." << endl;
		arch << "@" << "\t\tIN\t\tPTR\t\t" << ip << endl;
		arch << "DNS" << "\t\tIN\t\tA\t\t" << ip << endl;
		arch << hostname << "\t\tIN\t\tA\t\t" << ip << endl;
		arch << porcion_host << "\t\tIN\t\tPTR\t\t" << hostname << "." << dominio;
	}
	arch.close();
}
void archivoDNS()
{
	//Se genera el archvio de dominio para el dns
	narch = dominio + ".db";
	ofstream arch(narch.c_str());
	if(arch.is_open()){
		arch << "$TTL 3h" << endl;
		arch << "@       IN      SOA	"<< hostname <<"."<<dominio << ". " << "root."<<hostname<<"."<<dominio<< ". (" <<endl;
		arch << "        2013022744" << endl  << "        28800" << endl << "        3600" << endl;
		arch << "        604800" << endl << "        38400" << endl << ")" << endl;
		arch << dominio << ".\t\tIN\t\tNS\t\t" << hostname << "." << dominio << "." << endl;
		arch << hostname << "\t\tIN\t\tA\t\t" << ip << endl;
	}
	arch.close();
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
		arch << "\ttype master;" << endl;
		arch << "\tfile \"/etc/namedb/master/" << reverse_ip << ".db\";" << endl;
		arch << "};" << endl;
		arch.close();
		getchar();
	}
}

void crearArchivoDHCPv6()
{
	ofstream arch("dhcpd6.conf");
	if(arch.good()){
		arch << "subnet6 2001:db8:0:1::/64{" << endl;
		arch << "\tinterface " << interfaz<< ";" << endl;
		arch << "\trange6 2001:db8:0:1::120 2001:db8:0:1::200;" << endl;
		arch << "}";
		arch.close();
	}
}
void crearArchivoDHCPv4(){
	ofstream file("dhcpd4.conf");
	if(file.good()){
		
		file << "subnet " << porcion_host +".0" << " netmask 255.255.255"  << '{' << endl;
		file << "interface " << interfaz << "; " << endl;
		file << "range " << porcion_red + "." + rango_inferior << " " << porcion_red + "." + rango_superior << "; " << endl;
		file << "option routers " << porcion_red + "." + "0" << "; " << endl;
		file << "option broadcast-address " << porcion_red + ".255" << "; " <<endl;
		file <<'}' << '\n' << endl;
		file.close();
	}
}

void action(const string s){
	cout << "Ejecutando... " << s << endl;
	system(s.c_str());
	getchar();
}

void crearArchivoApagadoReinicio(){

	cout << "Creando archivo de apagado...." << endl;
	ofstream file("shutdown.sh");
	if(file.is_open()){
		file << "#!/usr/bin/env bash" << endl;
		file << "shutdown -y -g10 -i5" << endl;
		file.close();
	}
	file.open("reboot.sh");
	if(file.is_open()){
		file << "#!/usr/bin/env bash" << endl;
		file << "shutdown -y -g10 -i6" << endl;
		file.close();
	}


}
