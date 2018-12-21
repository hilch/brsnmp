#include <iostream>
#include <algorithm>
#include <array>
#include <regex>
#include <fstream>

#include "MyPviConnection.h"
#include "PviCom.h"
#include "stringsplit.h"

using std::string;

MyPviConnection*  MyPviConnection::m_instance = nullptr;

MyPviConnection::MyPviConnection() : m_listRequested{false}, m_detailedListRequested{false}, m_timeout(3000),
                                    m_lastFilterResult{""}, m_saveLastFilter{true}
{
    m_result = PviInitialize( 0, 0, "", NULL );
#ifdef _DEBUG
    std::cout << "PviInitialize = " << m_result << std::endl ;
#endif

    if( m_result == 0 )
    {
        m_result = PviSetGlobEventMsg( POBJ_EVENT_PVI_CONNECT, PVI_HMSG_NIL, SET_PVIFUNCTION, 0 );
        if( m_result == 0 )
            m_result = PviSetGlobEventMsg( POBJ_EVENT_PVI_DISCONN, PVI_HMSG_NIL, SET_PVIFUNCTION, 0 );
        if( m_result == 0 )
            m_result = PviSetGlobEventMsg( POBJ_EVENT_PVI_ARRANGE, PVI_HMSG_NIL, SET_PVIFUNCTION, 0 );
    }
    char* buffer = new char[MAX_PATH+1];
    size_t length = GetTempPath( MAX_PATH+1, buffer );
    m_lastFilterResultFileName.append( buffer, length );
    delete[] buffer;
    m_lastFilterResultFileName += "brsnmplastfilter.txt";
}

MyPviConnection::~MyPviConnection()
{
    PviDeinitialize();
}


MyPviConnection& MyPviConnection::getInstance()
{
    if( MyPviConnection::m_instance == nullptr )
        MyPviConnection::m_instance = new MyPviConnection();
    return *m_instance;
}

int MyPviConnection::SetFilter(string filterRegEx )
{
    string f = filterRegEx;
    std::transform( f.begin(), f.end(), f.begin(), ::toupper );

    if( f == "$LAST")
    {
        m_filter = LoadLastFilterResults();
        if( m_filter.length() == 0 )
        {
            std::cerr << "no last filter available" << std::endl;
            return -1;
        }
        m_saveLastFilter = false; /* do not refresh the last filter */
    }
    else
    {
        m_filter = filterRegEx;
    }
    return 0;
}


int MyPviConnection::SaveLastFilterResults(void)
{
  std::ofstream ofs;
  ofs.open ( m_lastFilterResultFileName, std::ofstream::out );
  ofs << m_lastFilterResult;
  ofs.close();
  if( ofs.bad() )
  {
    std::cerr << "Error saving last filter" << std::endl;
    return -1;
  }
  return 0;
}

string MyPviConnection::LoadLastFilterResults(void)
{
    std::ifstream ifs;
    string result;

    ifs.open (m_lastFilterResultFileName, std::ifstream::in );
    ifs >> result;
    ifs.close();
    return result;
}


int MyPviConnection::SetTimeout(int timeout )
{
    m_timeout = timeout;
    if( m_timeout < 500 )
        m_timeout = 500;
    if( m_timeout > 10000 )
        m_timeout = 10000;
    return 0;
}

void MyPviConnection::ShowList(bool verbose)
{
    m_listRequested = true;
    m_detailedListRequested = verbose;
}


void MyPviConnection::AppendVariable( string name, string value )
{
    ProcessVariable pvar;
    pvar.name = name;
    pvar.value = value;
    m_processVariables.push_back(pvar);
}

const std::vector<string> MyPviConnection::GetWritablePvars(void)
{
    return { "baudrate", "ipAddress", "subnetMask", "ipMethod", "inaNodeNumber", "inaPortNumber", "hostName",
             "defaultGateway", "dnsActivated", "dnsFromDhcp", "dnsServer1", "dnsServer2", "dnsServer3"     };
}


string &MyPviConnection::operator()()
{
    WaitUntilPviIsConnected();

    if( m_detailedListRequested )
        m_listRequested = false;

    ExecuteCommand();
    if( m_saveLastFilter )
        m_result = SaveLastFilterResults();
    return(m_output);
}


void MyPviConnection::WaitUntilPviIsConnected()
{
    WPARAM wParam;
    LPARAM lParam;
    LPVOID hMsg;
    DWORD  DataLen;
    T_RESPONSE_INFO Info;

    if( m_result == 0 )
    {
        while( true )
        {
            PviGetNextResponse(&wParam, &lParam, &hMsg, 0 );
            if( wParam != 0 )
            {
                PviGetResponseInfo (wParam, NULL, &DataLen, &Info, sizeof (Info));
                // confirm global event:
                PviReadResponse (wParam, NULL, 0);
                switch( Info.nType )
                {

                case POBJ_EVENT_PVI_CONNECT:
#ifdef _DEBUG
                    std::cerr << "POBJ_EVENT_PVI_CONNECT" << std::endl;
#endif
                    break;

                case POBJ_EVENT_PVI_ARRANGE:
#ifdef _DEBUG
                    std::cerr << "POBJ_EVENT_PVI_ARRANGE" << std::endl;
#endif
                    return;
                }
            }

        }
    }
    else
        return;
}

/* get detailed information from 'station' given as MAC address */
int MyPviConnection::GetPlcDetails(string macAddress, std::vector<string> &details)
{
    DWORD linkIdPvar;
    DWORD linkIdStation;
    string stationDescriptor = "CD=\" /CN=" + macAddress;
    std::vector<string> vars;
    constexpr unsigned BUFSIZE = 16384;
    char *buffer = new char[BUFSIZE];

    details.clear();

    m_result = PviCreate( &linkIdStation, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, stationDescriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
    if( m_result == 0 )
    {
        /* get a list of available SNMP parameter */
        PviRead( linkIdStation, POBJ_ACC_LIST_EXTERN, NULL, 0, buffer, BUFSIZE );
        if( m_result == 0 )
        {
            std::vector<string> fields;
            string s(buffer);
            split( fields, s, "\t", false );
            for( string s : fields )
            {
                size_t pos;
                if( (pos = s.find( " " KWDESC_OBJTYPE "=" KWOBJTYPE_PVAR)) != std::string::npos )
                {
                    vars.push_back(s.substr(0, pos));
                }
            }
        }
        /* get value and vartype */
        for( auto v : vars )
        {
            string objectName = "@Pvi/LnSNMP/Device/Station/" + v;
            string descriptor =  "CD=" + v;
            m_result = PviCreate( &linkIdPvar, objectName.c_str(), POBJ_PVAR, descriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
            if( m_result == 0 )
            {
                m_result = PviRead( linkIdPvar, POBJ_ACC_TYPE_EXTERN, NULL, 0, buffer, BUFSIZE );
                if( m_result == 0 )
                {
                    if( strstr(buffer, KWDESC_PVTYPE "=" KWPVTYPE_STRING) )  /* string */
                    {
                        m_result = PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, buffer, BUFSIZE );
                        if( m_result == 0 )
                        {
                            string s( buffer );
                            details.push_back( "\"" + v + "\"" + ":\"" + s + "\"" );
                        }
                    }
                    else if( strstr(buffer, KWDESC_PVTYPE "=" KWPVTYPE_INT32) )  /* int32 */
                    {
                        int32_t i;
                        m_result = PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, &i, sizeof(i) );
                        if( m_result == 0 )
                        {
                            details.push_back( "\"" + v + "\"" + ":\"" + std::to_string(i) + "\"" );
                        }
                    }
                }
                PviUnlink( linkIdPvar);
            }
        }
        PviUnlink( linkIdStation );
    }
    delete[] buffer;
    return m_result;
}



int MyPviConnection::WriteSnmpVariable( string macAddress, string name, string value )
{
    DWORD linkIdPvar;
    DWORD linkIdStation;
    string stationDescriptor = "CD=\" /CN=" + macAddress;
    std::vector<string> vars;
    constexpr unsigned BUFSIZE = 16384;
    char *buffer = new char[BUFSIZE];

    m_result = PviCreate( &linkIdStation, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, stationDescriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
    if( m_result == 0 )
    {
        string objectName = "@Pvi/LnSNMP/Device/Station/" + name;
        string descriptor =  "CD=" + name;
        m_result = PviCreate( &linkIdPvar, objectName.c_str(), POBJ_PVAR, descriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
        if( m_result == 0 )
        {
            m_result = PviRead( linkIdPvar, POBJ_ACC_TYPE_EXTERN, NULL, 0, buffer, BUFSIZE );
            if( m_result == 0 )
            {
                if( strstr(buffer, KWDESC_PVTYPE "=" KWPVTYPE_STRING) )  /* string */
                {
                    value.copy( buffer, value.length(), 0 );
                    buffer[value.length()] = 0;
                    m_result = PviWrite( linkIdPvar, POBJ_ACC_DATA, buffer, value.length(), NULL, 0 );
                }
                else if( strstr(buffer, KWDESC_PVTYPE "=" KWPVTYPE_INT32) )  /* int32 */
                {
                    int32_t i = std::stoi(value);
                    m_result = PviWrite( linkIdPvar, POBJ_ACC_DATA, &i, sizeof(i), NULL, 0 );
                }
            }
        }
        PviUnlink(linkIdStation);
    }
    return m_result;
}


int MyPviConnection::ExecuteCommand()
{
    DWORD linkIdSnmpLine;
    DWORD linkIdDevice;
    std::regex regExFilter(m_filter);

    constexpr int BUFSIZE = 65536;
    char *buffer = new char[BUFSIZE];

    if( m_result == 0 )
    {
        m_output = "";
        m_result = PviCreate( &linkIdSnmpLine, "@Pvi/LnSNMP", POBJ_LINE, "CD=\"LNSNMP\"", PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
        if( m_result == 0 )
        {
            string deviceDescriptor = "CD=\"/IF=snmp /RT=" + std::to_string(m_timeout) + "\"";
            m_result = PviCreate( &linkIdDevice, "@Pvi/LnSNMP/Device", POBJ_DEVICE, deviceDescriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
            if( m_result == 0 )
            {
                /* list available plc in network (MACs) */

                m_result = PviRead( linkIdDevice, POBJ_ACC_LIST_EXTERN, NULL, 0, buffer, BUFSIZE);
                if( m_result == 0 )
                {
                    int cnt = 0;
                    /* start output in JSON format */
                    if( m_listRequested || m_detailedListRequested )
                    {
                        m_output += "[\n";
                        if( cnt > 0 )
                            m_output += ",";

                    }


                    /* extract MACs from list */
                    std::vector<string> fields;
                    string s(buffer);
                    split( fields, s, "\t", false );
                    for( string s : fields )
                    {
                        size_t pos;
                        if( (pos = s.find( " " KWDESC_OBJTYPE "=" KWOBJTYPE_STATION)) != std::string::npos )
                        {
                            string macAddress = s.substr(0,pos);
                            std::vector<string> details;
                            m_result = GetPlcDetails(macAddress, details);
                            bool plcFitsToFilter = false;
                            if(m_filter.length() != 0)     /* apply a filter ? */
                            {
                                std::smatch match;
                                for( auto d : details )
                                {
                                    if( std::regex_search(d,match,regExFilter) )
                                    {
                                        plcFitsToFilter = true;
                                        break;
                                    }
                                }
                            }
                            else  /* no filter is set */
                            {
                                plcFitsToFilter = true;
                            }
                            if( plcFitsToFilter  )
                            {
                                if( cnt > 0 )
                                    m_lastFilterResult += "|";
                                m_lastFilterResult += macAddress;

                                if(m_listRequested || m_detailedListRequested)
                                {
                                    if( cnt > 0 )
                                        m_output += ",";
                                    if( m_listRequested )  /* simple list of MAC addresses */
                                    {
                                        m_output = m_output + "\"" + macAddress + "\"\n";
                                    }
                                    else if( m_detailedListRequested )  /* detailed list */
                                    {
                                        m_output += "{\n";
                                        for( unsigned int n = 0; n < details.size(); ++n )
                                        {
                                            m_output = m_output + "  " + details.at(n) + (n == details.size()-1 ? "\n" : ",\n");
                                        }
                                        m_output += "}\n";
                                    }
                                }
                                /* write to SNMP process variables */
                                for( auto pv : m_processVariables )
                                {
                                    WriteSnmpVariable( macAddress, pv.name, pv.value );
                                }
                                ++cnt;
                            }
                        }
                    }
                }
                if( m_listRequested || m_detailedListRequested )
                {
                    m_output += "\n]\n";
                }
                PviUnlink(linkIdDevice);
            }
            PviUnlink(linkIdSnmpLine);
        }
    }

    delete[] buffer;
    return m_result;
}


