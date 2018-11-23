#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG
#include <cstring>
#include <algorithm>
#include <array>
#include "MyPviConnection.h"
#include "PviCom.h"



MyPviConnection*  MyPviConnection::m_instance = nullptr;

MyPviConnection::MyPviConnection() : m_list{false}, m_detailedList{false}
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

void MyPviConnection::AppendMACFilter(std::string macAddress)
{
    m_macFilter.push_back(macAddress);
}

void MyPviConnection::ShowList(bool verbose)
{
    m_list = true;
    m_detailedList = verbose;
}


std::string &MyPviConnection::operator()()
{
    WaitUntilConnected();

    if( m_detailedList )
        m_list = false;

    ExecuteCommand();
    return(m_output);
}


void MyPviConnection::WaitUntilConnected()
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


int MyPviConnection::GetPlcDetails(std::string macAddress, std::vector<std::string> &details)
{
    DWORD linkIdPvar;
    DWORD linkIdStation;
    std::string stationDescriptor = "CD=\" /CN=" + macAddress;
    std::vector<std::string> vars;
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
            const char delimiter[] = {"\t"};
            char * p = std::strtok ( buffer, delimiter );
            while (p!=0)
            {
                char *p2;
                if( NULL!= (p2 = strstr(p, " " KWDESC_OBJTYPE "=" KWOBJTYPE_PVAR))  )
                {
                    vars.push_back(std::string(p,p2-p));
                }
                p = std::strtok( NULL, delimiter );
            }
        }
        /* get value and vartype */
        for( auto v : vars )
        {
            std::string objectName = "@Pvi/LnSNMP/Device/Station/" + v;
            std::string descriptor =  "CD=" + v;
            m_result = PviCreate( &linkIdPvar, objectName.c_str(), POBJ_PVAR, descriptor.c_str() , PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
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
                            std::string s( buffer );
                            details.push_back( "\"" + v + "\"" + " : \"" + s + "\"" );
                        }
                    }
                    else if( strstr(buffer, KWDESC_PVTYPE "=" KWPVTYPE_INT32) )  /* int32 */
                    {
                        int32_t i;
                        m_result = PviRead( linkIdPvar, POBJ_ACC_DATA, NULL, 0, &i, sizeof(i) );
                        if( m_result == 0 )
                        {
                            details.push_back( "\"" + v + "\"" + " : \"" + std::to_string(i) + "\"" );
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



int MyPviConnection::ExecuteCommand()
{
    DWORD linkIdSnmpLine;
    DWORD linkIdDevice;
    std::vector<std::string> macListStations;

    if( m_result == 0 )
    {
        m_output = "";
        m_result = PviCreate( &linkIdSnmpLine, "@Pvi/LnSNMP", POBJ_LINE, "CD=\"LNSNMP\"", PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
        if( m_result == 0 )
        {
            m_result = PviCreate( &linkIdDevice, "@Pvi/LnSNMP/Device", POBJ_DEVICE, "CD=\"/IF=snmp /RT=3000\"", PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "" );
            if( m_result == 0 )
            {
                /* list available plc (MACs) */
                char *buffer = new char[65536];
                m_result = PviRead( linkIdDevice, POBJ_ACC_LIST_EXTERN, NULL, 0, buffer, 65536);
                if( m_result == 0 )
                {

                    const char delimiter[] = {"\t"};
                    char * p = std::strtok ( buffer, delimiter );
                    while (p!=0)
                    {
                        if( strstr(p, KWDESC_OBJTYPE "=" KWOBJTYPE_STATION) )
                        {
                            std::string macAddress( p, 17 );
                            bool macFilterActive = !m_macFilter.empty();
                            if( macFilterActive )
                            {
                                if( find_if( m_macFilter.begin(), m_macFilter.end(), [&]( std::string m )
                            {
                                return m == macAddress;
                            } ) != m_macFilter.end() )
                                {
                                    macListStations.push_back(macAddress);
                                }
                            }
                            else
                            {
                                macListStations.push_back(macAddress);
                            }
                        }
                        p = std::strtok( NULL, delimiter );
                    }
                }
                delete[] buffer;
                /* list of MACs in macListStations now */
                if( macListStations.size() > 1 && m_detailedList )
                    m_output += "[\n";

                int cnt = 0;
                for( auto mac : macListStations )
                {
                    if( m_list )  /* simple list of MAC addresses */
                    {
                        m_output += mac;
                        m_output += "\n";
                    }
                    else if( m_detailedList )  /* detailed list */
                    {
                        if( cnt > 0 )
                            m_output += ",";
                        m_output += "{\n";
                        std::vector<std::string> details;
                        m_result = GetPlcDetails(mac, details);
                        for( auto d : details )
                            m_output = m_output + "  " + d + "\n";
                        m_output += "}\n";
                    }
                    ++cnt;
                }
                if( macListStations.size() > 1 && m_detailedList )
                    m_output += "\n]\n";
                PviUnlink(linkIdDevice);
            }
            PviUnlink(linkIdSnmpLine);
        }
    }
    return m_result;
}
