#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

class InstallerAsistant{

    public:
        InstallerAsistant(){}
        InstallerAsistant(string user, string dominio, string ip, string hostname, string interfaz, string rango_inferior, string rango_superior);
        static InstallerAsistant leerArchivo(string ruta);
        void escribirArchivo();
        void separarIP(string ipv4);
        void capturar();
        void archivoDNSReverso();
        void archivoDNS();
        void namedFile();
        void action(const string s);
        void crearArchivoDHCPv6();
        void crearArchivoDHCPv4();
        void crearArchivoApagadoReinicio();
        void installer();
        void installer2();
        
    private:
        string user;
        string ipv4_segments[4]; //Se modifica en el metodo separarIP
        string dominio;
        string ip; //Importante asignar este parametro en el constructor
        string reverse_ip; //Se agrega en el metodo separarIP
        string hostname;
        string interfaz;
        string rango_inferior;
        string rango_superior;
        string porcion_red; //Se modifica en el metodo separarIP
        string porcion_host; //Se modifica en el metodo sperarIP
        string reverseDNSrute;
        string dnsRute;
};
void InstallerAsistant::escribirArchivo(){
    ofstream file("save.conf");
    if(file.is_open()){
        file << user << endl;
		file << dominio << endl;
        file << ip << endl;
        file << hostname << endl;
        file << interfaz << endl;
        file << rango_inferior << endl;
        file << rango_superior << endl;
        file.close();
    }
}
InstallerAsistant::InstallerAsistant(string user, string dominio, string ip, string hostname, string interfaz, string rango_inferior, string rango_superior){
    this->user = user;
    this->dominio = dominio;
    this->ip = ip;
    this->hostname = hostname;
    this->interfaz = interfaz;
    this->rango_inferior = rango_inferior;
    this->rango_superior = rango_superior;
    this->porcion_host = this->porcion_red = this->reverse_ip = "";
    this->separarIP(ip);
}
InstallerAsistant InstallerAsistant::leerArchivo(string ruta){
    string data[7];
    string aux;
    string dominio, ip, hostname, interfaz, rango_inferior, rango_superior, user;
    ifstream arch(ruta.c_str());
    if(arch.is_open()){
        int i = 0;
        while(arch.eof()){
            getline(arch, aux, '\n');
            data[i] = aux;
            i++;
        }
    }
    for(int i = 0; i<7; i++){
        switch(i){
            case 0:
                dominio = data[i];
                break;
            case 1:
                ip = data[i];
                break;
            case 2:
                hostname = data[i];
                break;
            case 3:
                interfaz = data[i];
                break;
            case 4:
                rango_inferior = data[i];
                break;
            case 5:
                rango_superior = data[i];
                break;
            case 6:
                user = data[i];
                break;
            default:
                break;
        }
    }
    return InstallerAsistant(user, dominio, ip, hostname, interfaz, rango_inferior, rango_superior);
}
void InstallerAsistant::separarIP(string ipv4){
    string var = "";
    for(int i = 0, e = 0; i<ipv4.length(); i++){
        if(ipv4[i] == '.'){
            ipv4_segments[e] = var;
            e++;
            var="";
        }else{
            var+=ipv4[i];
        }
    }
    ipv4_segments[3] = var;
    for(int i = 0; i<3; i++){

        this->porcion_red+=this->ipv4_segments[i];
        if(i<2){
            this->porcion_red += ".";
        }
    }
    for(int i = 2; i>=0; i--){
        this->reverse_ip += this->ipv4_segments[i];
        if(0<i){
            this->reverse_ip += ".";
        }
    }
    this->porcion_host = this->ipv4_segments[3];
    cout << porcion_host << endl;

}
void InstallerAsistant::capturar(){
    cout << "Digite su nombre de usuario: "; getline(cin, this->user, '\n');
    cout << "Digite su dominio: "; getline(cin, this->dominio, '\n');
	cout << "Digite la ip de su servidor DNS: "; getline(cin, ip, '\n');
	cout << "Digite el nombre de su hostname: "; getline(cin, hostname, '\n');
	cout << "Digite el nombre de la interfaz: "; getline(cin, interfaz, '\n');
	cout << "Digite el rango inferior de su servidor DHCP: "; getline(cin, rango_inferior, '\n');
	cout << "Digite el rango superior de su servidor DHCP: "; getline(cin, rango_superior, '\n');
    this->separarIP(this->ip);
}
void InstallerAsistant::archivoDNSReverso(){
	this->reverseDNSrute = reverse_ip + ".db";
	ofstream arch(reverseDNSrute.c_str());
	if(arch.is_open()){
		arch << "$TTL 3h" << endl;
		arch << "@       IN      SOA	"<< hostname <<"."<<dominio << ". " << "root."<<dominio<< ". (" <<endl;
		arch << "        2013022744" << endl  << "        28800" << endl << "        3600" << endl;
		arch << "        604800" << endl << "        38400" << endl << ")" << endl;
		arch << "@" << "\t\tIN\t\tNS\t\t" << hostname << "." << dominio << "." << endl;
		arch << "@" << "\t\tIN\t\tPTR\t\t" << ip << endl;
		arch << "DNS" << "\t\tIN\t\tA\t\t" << ip << endl;
		arch << hostname << "\t\tIN\t\tA\t\t" << ip << endl;
		arch << porcion_host << "\t\tIN\t\tPTR\t\t" << hostname << "." << dominio << endl;
	}
	arch.close();
}
void InstallerAsistant::archivoDNS()
{
	//Se genera el archvio de dominio para el dns
	this->dnsRute = dominio + ".db";
	ofstream arch(dnsRute.c_str());
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
void InstallerAsistant::namedFile(){
	string narch = "named.conf";
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
void InstallerAsistant::action(const string s){
	cout << "Ejecutando... " << s << endl;
	system(s.c_str());
	getchar();
}
void InstallerAsistant::crearArchivoDHCPv6()
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
void InstallerAsistant::crearArchivoDHCPv4(){
	ofstream file("dhcpd4.conf");
	if(file.good()){
		
		file << "subnet " << porcion_red +".0" << " netmask 255.255.255.0"  << '{' << endl;
		file << "\tinterface " << interfaz << "; " << endl;
		file << "\trange " << porcion_red + "." + rango_inferior << " " << porcion_red + "." + rango_superior << "; " << endl;
		file << "\toption routers " << porcion_red + "." + "0" << "; " << endl;
		file << "\toption broadcast-address " << porcion_red + ".255" << "; " <<endl;
		file <<'}' << '\n' << endl;
		file.close();
	}
}
void InstallerAsistant::crearArchivoApagadoReinicio(){

	cout << "Creando archivo de apagado...." << endl;
    string aux = "/export/home/"+this->user+"/shutdown.sh";
	ofstream file(aux.c_str());
	if(file.is_open()){
		file << "#!/usr/bin/env bash" << endl;
		file << "shutdown -y -g10 -i5" << endl;
		file.close();
	}
    aux = "/export/home/"+this->user+"/reboot.sh";
	file.open(aux.c_str());
	if(file.is_open()){
		file << "#!/usr/bin/env bash" << endl;
		file << "shutdown -y -g10 -i6" << endl;
		file.close();
	}


}
void InstallerAsistant::installer(){
    crearArchivoApagadoReinicio();
	//Habilitamos SSH
	action("pkg update");
	cout << "Generando archivos de configuracion necesarios...." << endl << "Rellene los campos del formulario..." << endl;
    this->capturar();
	cout << "Creando archivos de DHCPv4 " << endl;
	this->crearArchivoDHCPv4();
	cout << "Creando archivos de DHCPv6 " << endl;
	this->crearArchivoDHCPv6();
	this->archivoDNS();
	action("ls -l");
	this->archivoDNSReverso();
	action("ls -l");
	this->namedFile();
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
	action("pkg install service/network/dns/bind");
	//Creamos los archivos correspondientes
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
	action("svcadm refresh network/dns/client:default");
	action("svcadm restart dns/client");
	action("nscfg export svc:/network/dns/client:default");
	action("svcadm refresh name-service/switch");
	action("svcadm refresh name-service/cache"),
	action("svcadm restart name-service/switch");
	action("svcadm restart  name-service/cache");
    cout << "Creando sus archivos para poder apagar y prender el servidor..." << endl;
    action("pkg install network/smb/client");
    this->crearArchivoApagadoReinicio();
    this->escribirArchivo();
}

void InstallerAsistant::installer2(){
    string temporalip, activedirectory, netbios;
    cout << "Digite el netbios: "; getline(cin, netbios, '\n');
    cout << "Digite la ip de su servidor de windows server: "; getline(cin, temporalip, '\n');
    cout << "Digite el dominio de su active directory de windows server: "; getline(cin, activedirectory, '\n');
    cout << "A continuacion usted debe de digitar los siguientes comandos ya que no son asistidos, sin embargo copielos y peguelos en otra parte y escribalos en ese orden: " << endl;
    cout << "select dns/client" << endl;
    cout << "setprop config/search = astring: (" << dominio << " " << activedirectory << ")" << endl;
    cout << "setprop config/nameserver = net_address: (" << ip << " " << temporalip << ")";
    cout << "select dns/client:default" << endl;
    cout << "refresh" << endl;
    cout << "validate" << endl;
    cout << "select name-service/switch" << endl;
    cout << "setprop config/host = astring: \"files dns\"" << endl;
    cout << "select system/name-service/switch:default" << endl;
    cout << "refresh" << endl;
    cout << "validate" << endl;
    cout << "exit" << endl;
    action("svccfg");
    cout << "Muy bien, ahora ejecutaremos un script... solo ejecuta el archivo \"auxiliar.sh\"" << endl;
    transform(activedirectory.begin(), activedirectory.end(), activedirectory.begin(), ::toupper);
	string aux2 = activedirectory;
	transform(activedirectory.begin(), activedirectory.end(), activedirectory.begin(), ::tolower);
    action("./auxiliar.sh");
    action("domainjoin-cli join "+aux2+" Administrador@"+activedirectory);
    action("svcadm enable -r network/smb/client");
    action("smbadm show-shares -u Administrador " + activedirectory);
    
}


