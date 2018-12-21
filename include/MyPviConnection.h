#ifndef MYPVICONNECTION_H
#define MYPVICONNECTION_H

#include "PviCom.h"
#include <string>
#include <vector>


class MyPviConnection
{

public:
    static MyPviConnection& getInstance();
    ~MyPviConnection();
    int SetFilter(std::string filterRegEx);
    int SetTimeout(int timeout);
    void ShowList( bool verbose = false);
    void AppendVariable( std::string name, std::string value );
    const std::vector<std::string> GetWritablePvars(void);
    int Result(void)
    {
        return m_result;
    };
    std::string &operator()(void);

protected:

private:
    struct ProcessVariable
    {
        std::string name;
        std::string value;
    };
    MyPviConnection();
    static MyPviConnection *m_instance;
    void WaitUntilPviIsConnected(void);
    int ExecuteCommand(void);
    int GetPlcDetails( std::string macAddress, std::vector<std::string> &details );
    int WriteSnmpVariable( std::string macAddress, std::string name, std::string value);
    int SaveLastFilterResults(void);
    std::string LoadLastFilterResults(void);
    std::vector<ProcessVariable> m_processVariables;
    std::string m_output;
    std::string m_filter;
    bool m_listRequested, m_detailedListRequested;
    int m_timeout;
    std::string m_lastFilterResult;
    std::string m_lastFilterResultFileName;
    bool m_saveLastFilter;
    int m_result;

};

#endif // MYPVICONNECTION_H
