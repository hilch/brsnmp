#include <iostream>
#include <stdexcept>
#include "MyPviConnection.h"
#include "argh.h"


using namespace std;

string help(void);

int main(int argc, char* argv[] )
{
   // argh::parser cmdl({ "-h","--help", "-v","--version", "-l", "--list", "-v", "--verbose", "-f", "--filter", "-t", "--timeout",

                    //});
                    argh::parser cmdl(argv);
    //cmdl.parse(argc,argv);

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
        cout << help();
        return argc == 1 ? -1 : 0;
    }
    if (cmdl[ { "-v","--version" }] )
    {
        std::cout << "{\n\"brsnmp\":\"0.2\",\n";
        std::cout << "\"build-time\":\"" << __TIME__ << "\",\n";
        std::cout << "\"build-date\":\"" << __DATE__ << "\"\n}\n";
        return 0;
    }

    MyPviConnection con = MyPviConnection::getInstance();

    /* set filter */
    string filter = cmdl({"-f", "--filter"}).str();
    if( filter.length() )
        con.SetFilter(filter);

    /* set communication timeout */
    unsigned int timeout = 0;
    if( !(cmdl({"-t","--timeout"},3000) >> timeout) )
    {
        cerr << "must provide a valid timeout !";
        return -1;
    }
    con.SetTimeout(timeout);

    /* add a SNMP process variable for writing */

    for( auto pv : con.GetWritablePvars() )
    {
        string value = "";
        cmdl(pv) >> value;
        if( value.length() > 0 )
            con.AppendVariable(pv, value );
    }

    /* do we want a list of MACs ? */
    if (cmdl[{ "-l", "--list" }] )
        con.ShowList(false);

    /* or do we need a detailed list ? */
    if (cmdl[{ "-d", "--details" }] )
        con.ShowList(true);

    cout <<  con() << endl;



    return con.Result();
}


string help()
{
    const string s =
    {
        "brsnmp:  Execute PVI-SNMP commands for B&R plc\n\n"
        "https://github.com/hilch/brsnmp\n\n"
        "usage:\n\n"
        "-h, --help                       this help\n"
        "-v, --version                    software version\n"
        "-f=<EXPR>, --filter=<EXPR>       Filter(regular expression following ECMA syntax)\n"
        "-t=<MS>, --timeout=<MS>          SNMP Line Timeout [ms], default: <3000>\n"
        "-l, --list                       list MAC addresses of available B&R plc\n"
        "-d, --details                    list detailed properties of available B&R plc\n"
        "--<NAME>=<VALUE>                 set parameter <NAME> to <VALUE> (see --details)\n"
        "Examples:\n\n"
        "brsnmp --details --filter=00-60-65-24-7d-2e\n\n"
        "brsnmp --ipAddress=192.168.0.13 --subnetMask=255.255.255.0 --ipMethod=0 --filter=4PPC30\n\n"
    };
    return s;
}
