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
        void AppendMACFilter(std::string macAddress);
        void ShowList( bool verbose = false);
        int Result(void){return m_result; };
        std::string &operator()(void);

    protected:

    private:
        MyPviConnection();
        static MyPviConnection *m_instance;
        void WaitUntilConnected(void);
        int ExecuteCommand(void);
        int GetPlcDetails( std::string macAddress, std::vector<std::string> &details );
        std::string m_output;
        std::vector<std::string> m_macFilter;
        bool m_list, m_detailedList;
        int m_result;

};

#endif // MYPVICONNECTION_H
