#include <iostream>
#include <algorithm>
#include <array>
#include <regex>
#include <fstream>
#include <cstdio>
#include <cstdint>
#include <memory>

#include "MyPviConnection.h"
#include "PviCom.h"
#include "stringsplit.h"
#include "json.hpp"


using std::string;
using json = nlohmann::json;

// Buffer size constants
constexpr unsigned int MyPviConnection::SMALL_BUFSIZE;
constexpr unsigned int MyPviConnection::LARGE_BUFSIZE;

std::unique_ptr<MyPviConnection> MyPviConnection::m_instance = nullptr;

MyPviConnection::MyPviConnection() : m_filter{".*"}, m_listRequested{false}, m_detailedListRequested{false}, m_timeout(3000),
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
    size_t length = GetTempPathA( MAX_PATH+1, buffer );
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
    if(MyPviConnection::m_instance == nullptr)
        MyPviConnection::m_instance = std::unique_ptr<MyPviConnection>(new MyPviConnection());
    return *MyPviConnection::m_instance;
}

int MyPviConnection::SetFilter(const string& filterRegEx)
{
    string f = filterRegEx;
    std::transform(f.begin(), f.end(), f.begin(), ::toupper);

    if(f == "$LAST")
    {
        m_filter = LoadLastFilterResults();
        if(m_filter.length() == 0)
        {
            std::cerr << "no last filter available" << std::endl;
            return -1;
        }
        m_saveLastFilter = false; /* do not refresh the last filter */
    }
    else if(filterRegEx.length() == 0)  /* empty filter expression applied */
    {
       std::remove(m_lastFilterResultFileName.c_str());
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
    ofs.open(m_lastFilterResultFileName, std::ofstream::out);
    if(!ofs.is_open())
    {
        std::cerr << "Error opening file for writing: " << m_lastFilterResultFileName << std::endl;
        return -1;
    }
    ofs << m_lastFilterResult;
    ofs.close();
    if(ofs.fail())
    {
        std::cerr << "Error saving last filter" << std::endl;
        return -1;
    }
    return 0;
}

string MyPviConnection::LoadLastFilterResults(void) const
{
    std::ifstream ifs;
    string result;

    ifs.open(m_lastFilterResultFileName, std::ifstream::in);
    if(!ifs.is_open())
    {
        return result;  // Return empty string if file doesn't exist
    }
    ifs >> result;
    ifs.close();
    return result;
}


int MyPviConnection::SetTimeout(int timeout )
{
    m_timeout = timeout;
    if( m_timeout < 50 )
        m_timeout = 50;
    if( m_timeout > 10000 )
        m_timeout = 10000;
    return 0;
}

void MyPviConnection::ShowList(bool verbose)
{
    m_listRequested = true;
    m_detailedListRequested = verbose;
}


void MyPviConnection::AppendVariable(const string& name, const string& value)
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


std::string MyPviConnection::ReadPviVersionInfo(void)
{
    std::vector<char> buffer(SMALL_BUFSIZE);
    DWORD linkIdPvi;

    WaitUntilPviIsConnected();

    m_result = PviCreate(&linkIdPvi, "@Pvi", POBJ_PVI, "", PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "");
    if(m_result == 0){
        m_result = PviRead(linkIdPvi, POBJ_ACC_VERSION, NULL, 0, buffer.data(), SMALL_BUFSIZE);
        if(m_result == 0)
        {
            string s(buffer.data());

            json result = json::array();
            std::istringstream stream(s);
            std::string line;
    
            // Pattern: name (file) version
            std::regex pattern(R"((.+?)\s+\(([^)]+)\)\s+([^\n\r]+))");

            while (std::getline(stream, line)) {
                if (line.empty()) continue;
        
                std::smatch match;
                if (std::regex_search(line, match, pattern)) {
                    json obj;
                    obj["name"] = match[1].str();
                    obj["file"] = match[2].str();
                    obj["version"] = match[3].str();
                    result.push_back(obj);
                }
            }
            return result.dump(2);
        }

    }
    return "";

}



/* get detailed information from 'station' given as MAC address */
int MyPviConnection::GetPlcDetails(const string& macAddress, std::vector<string>& details)
{
    DWORD linkIdPvar;
    DWORD linkIdStation;
    string stationDescriptor = "CD=\" /CN=" + macAddress;
    std::vector<string> vars;
    std::vector<char> buffer(SMALL_BUFSIZE);

    details.clear();

    m_result = PviCreate(&linkIdStation, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, stationDescriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "");
    if(m_result == 0)
    {
        /* get a list of available SNMP parameter */
        PviRead(linkIdStation, POBJ_ACC_LIST_EXTERN, NULL, 0, buffer.data(), SMALL_BUFSIZE);
        if(m_result == 0)
        {
            std::vector<string> fields;
            string s(buffer.data());
            split(fields, s, "\t", false);
            for(const auto& s : fields)
            {
                size_t pos;
                if((pos = s.find(" " KWDESC_OBJTYPE "=" KWOBJTYPE_PVAR)) != std::string::npos)
                {
                    vars.push_back(s.substr(0, pos));
                }
            }
        }
        /* get value and vartype */
        for(const auto& v : vars)
        {
            string objectName = "@Pvi/LnSNMP/Device/Station/" + v;
            string descriptor = "CD=" + v;
            m_result = PviCreate(&linkIdPvar, objectName.c_str(), POBJ_PVAR, descriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "");
            if(m_result == 0)
            {
                m_result = PviRead(linkIdPvar, POBJ_ACC_TYPE_EXTERN, NULL, 0, buffer.data(), SMALL_BUFSIZE);
                if(m_result == 0)
                {
                    if(strstr(buffer.data(), KWDESC_PVTYPE "=" KWPVTYPE_STRING))  /* string */
                    {
                        m_result = PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, buffer.data(), SMALL_BUFSIZE);
                        if(m_result == 0)
                        {
                            string s(buffer.data());
                            details.push_back("\"" + v + "\"" + ":\"" + s + "\"");
                        }
                    }
                    else if(strstr(buffer.data(), KWDESC_PVTYPE "=" KWPVTYPE_INT32))  /* int32 */
                    {
                        int32_t i;
                        m_result = PviRead(linkIdPvar, POBJ_ACC_DATA, NULL, 0, &i, sizeof(i));
                        if(m_result == 0)
                        {
                            details.push_back("\"" + v + "\"" + ":\"" + std::to_string(i) + "\"");
                        }
                    }
                }
                PviUnlink(linkIdPvar);
            }
        }
        PviUnlink(linkIdStation);
    }
    return m_result;
}



int MyPviConnection::WriteSnmpVariable(const string& macAddress, const string& name, const string& value)
{
    DWORD linkIdPvar;
    DWORD linkIdStation;
    string stationDescriptor = "CD=\" /CN=" + macAddress;
    std::vector<string> vars;
    std::vector<char> buffer(SMALL_BUFSIZE);
    string writeValue = value;

    std::regex regExEnvVariable(R"(%(\w.*)%)");
    std::smatch sm;
    std::regex_match(writeValue, sm, regExEnvVariable);

    if(sm.size() == 2)  /* an environment variable was given as value */
    {
       string envVariableName = sm[1];
       if(GetEnvironmentVariableA(envVariableName.c_str(), buffer.data(), SMALL_BUFSIZE))
       {
           writeValue.assign(buffer.data());  /* use content of environment variable */
       }
       else
       {
           m_result = -1;
           std::cerr << "environment variable not found: \"" << value << "\" !\n";
           return -1;
       }
    }

    m_result = PviCreate(&linkIdStation, "@Pvi/LnSNMP/Device/Station", POBJ_STATION, stationDescriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "");
    if(m_result == 0)
    {
        string objectName = "@Pvi/LnSNMP/Device/Station/" + name;
        string descriptor = "CD=" + name;
        m_result = PviCreate(&linkIdPvar, objectName.c_str(), POBJ_PVAR, descriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "");
        if(m_result == 0)
        {
            m_result = PviRead(linkIdPvar, POBJ_ACC_TYPE_EXTERN, NULL, 0, buffer.data(), SMALL_BUFSIZE);
            if(m_result == 0)
            {
                if(strstr(buffer.data(), KWDESC_PVTYPE "=" KWPVTYPE_STRING))  /* string */
                {
                    writeValue.copy(buffer.data(), writeValue.length(), 0);
                    buffer[writeValue.length()] = 0;
                    m_result = PviWrite(linkIdPvar, POBJ_ACC_DATA, buffer.data(), writeValue.length(), NULL, 0);
                }
                else if(strstr(buffer.data(), KWDESC_PVTYPE "=" KWPVTYPE_INT32))  /* int32 */
                {
                    int32_t i = std::stoi(writeValue);
                    m_result = PviWrite(linkIdPvar, POBJ_ACC_DATA, &i, sizeof(i), NULL, 0);
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

    std::vector<char> buffer(LARGE_BUFSIZE);

    if(m_result == 0)
    {
        m_output.clear();
        m_result = PviCreate(&linkIdSnmpLine, "@Pvi/LnSNMP", POBJ_LINE, "CD=\"LNSNMP\"", PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "");
        if(m_result == 0)
        {
            string deviceDescriptor = "CD=\"/IF=snmp /RT=" + std::to_string(m_timeout) + "\"";
            m_result = PviCreate(&linkIdDevice, "@Pvi/LnSNMP/Device", POBJ_DEVICE, deviceDescriptor.c_str(), PVI_HMSG_NIL, SET_PVIFUNCTION, 0, "");
            if(m_result == 0)
            {
                /* list available plc in network (MACs) */
                m_result = PviRead(linkIdDevice, POBJ_ACC_LIST_EXTERN, NULL, 0, buffer.data(), LARGE_BUFSIZE);
                if(m_result == 0)
                {
                    int cnt = 0;
                    /* Compile regex filter once outside the loop */
                    std::regex regExFilter;
                    bool hasFilter = (m_filter.length() != 0);
                    if(hasFilter)
                    {
                        try
                        {
                            regExFilter.assign(m_filter);
                        }
                        catch(std::regex_error& e)
                        {
                            std::cerr << "illegal filter applied ! (" << e.what() << ")";
                            m_result = -1;
                            return -1;
                        }
                    }

                    /* start output in JSON format */
                    if(m_listRequested || m_detailedListRequested)
                    {
                        m_output += "[\n";
                    }

                    /* extract MACs from list */
                    std::vector<string> fields;
                    string s(buffer.data());
                    split(fields, s, "\t", false);
                    for(const auto& field : fields)
                    {
                        size_t pos;
                        if((pos = field.find(" " KWDESC_OBJTYPE "=" KWOBJTYPE_STATION)) != std::string::npos)
                        {
                            string macAddress = field.substr(0, pos);
                            std::vector<string> details;
                            m_result = GetPlcDetails(macAddress, details);
                            bool plcFitsToFilter = true;

                            if(hasFilter)
                            {
                                plcFitsToFilter = false;
                                std::smatch match;
                                for(const auto& d : details)
                                {
                                    if(std::regex_search(d, match, regExFilter))
                                    {
                                        plcFitsToFilter = true;
                                        break;
                                    }
                                }
                            }

                            if(plcFitsToFilter)
                            {
                                if(cnt > 0)
                                    m_lastFilterResult += "|";
                                m_lastFilterResult += macAddress;

                                if(m_listRequested || m_detailedListRequested)
                                {
                                    if(cnt > 0)
                                        m_output += ",";
                                    if(m_listRequested)  /* simple list of MAC addresses */
                                    {
                                        m_output += "\"" + macAddress + "\"\n";
                                    }
                                    else if(m_detailedListRequested)  /* detailed list */
                                    {
                                        m_output += "{\n";
                                        for(unsigned int n = 0; n < details.size(); ++n)
                                        {
                                            m_output += "  " + details[n] + (n == details.size() - 1 ? "\n" : ",\n");
                                        }
                                        m_output += "}\n";
                                    }
                                }
                                /* write to SNMP process variables */
                                for(const auto& pv : m_processVariables)
                                {
                                    WriteSnmpVariable(macAddress, pv.name, pv.value);
                                }
                                ++cnt;
                            }
                        }
                    }

                    if(m_listRequested || m_detailedListRequested)
                    {
                        m_output += "]\n";
                    }
                    PviUnlink(linkIdDevice);
                }
            }
            PviUnlink(linkIdSnmpLine);
        }
    }

    return m_result;
}


