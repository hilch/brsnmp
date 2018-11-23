#include <iostream>
#include "MyPviConnection.h"
#include "argh.h"

using namespace std;

int main(int argc, char* argv[] )
{
    argh::parser cmdl({ "-h","--help", "-v","--version", "-l", "--list", "-v", "--verbose", "-m", "--mac" });
    cmdl.parse(argc,argv);

    #ifdef _DEBUG
    cout << "Positional args:\n";
    for (auto& pos_arg : cmdl.pos_args())
      cout << '\t' << pos_arg << '\n';

    cout << "\nFlags:\n";
    for (auto& flag : cmdl.flags())
      cout << '\t' << flag << '\n';

    cout << "\nParameters:\n";
    for (auto& param : cmdl.params())
      cout << '\t' << param.first << " : " << param.second << '\n';
    #endif


    if (cmdl[ { "-h","--help" }] || argc == 1 )
    {
        string exename;
        std::cout << "brsnmp.exe :  Execute PVI-SNMP commands for B&R plc\n";
        std::cout << "usage:\n\n";
        std::cout << "-h, --help\tthis help\n";
        std::cout << "-v, --version\tsoftware version\n";
        std::cout << "-l, --list\tlist MAC addresses of available B&R plc\n";
        std::cout << "-d, --details\tlist detailed properties of available B&R plc\n";
        std::cout << "-m, --mac\tlimit execution to a MAC address\n";
        std::cout << "\n\nexamples:\n\n";
        std::cout << "brsnmp --list --mac=00-60-65-24-7d-2e\n";
        return argc == 1 ? -1 : 0;
    }
    if (cmdl[ { "-v","--version" }] )
    {
        std::cout << "brsnmp V1.0\n";
    }

    MyPviConnection con = MyPviConnection::getInstance();
    string mac = cmdl({"-m", "--mac"}).str();
    if( mac.length() )
        con.AppendMACFilter(mac);
    if (cmdl[{ "-l", "--list" }])
        con.ShowList(false);
    if (cmdl[{ "-d", "--details" }])
        con.ShowList(true);

    cout <<  con() << endl;



    return con.Result();
}
