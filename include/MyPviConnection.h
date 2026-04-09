#ifndef MYPVICONNECTION_H
#define MYPVICONNECTION_H

#include "PviCom.h"
#include <string>
#include <vector>
#include <memory>


class MyPviConnection
{
    // Buffer size constants
    static constexpr unsigned int SMALL_BUFSIZE = 16384;
    static constexpr unsigned int LARGE_BUFSIZE = 65536;

public:
    static MyPviConnection& getInstance();
    ~MyPviConnection();
    int SetFilter(const std::string& filterRegEx);
    int SetTimeout(int timeout);
    void ShowList(bool verbose = false);
    void AppendVariable(const std::string& name, const std::string& value);
    const std::vector<std::string> GetWritablePvars(void);
    int Result(void) const
    {
        return m_result;
    }
    std::string& operator()(void);

protected:

private:
    struct ProcessVariable
    {
        std::string name;
        std::string value;
    };
    MyPviConnection();
    static std::unique_ptr<MyPviConnection> m_instance;
    void WaitUntilPviIsConnected(void);
    int ExecuteCommand(void);
    int GetPlcDetails(const std::string& macAddress, std::vector<std::string>& details);
    int WriteSnmpVariable(const std::string& macAddress, const std::string& name, const std::string& value);
    int SaveLastFilterResults(void);
    std::string LoadLastFilterResults(void) const;
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
