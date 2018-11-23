#ifndef PLC_H
#define PLC_H

#include <string>

struct Plc
{
    std::string m_targetTypeDescription;
    std::string m_macAddress;
    std::string m_ipAddress;
    std::string m_subnetMask;
    std::string m_arVersion;
    bool m_INA_activated;
    int  m_INA_nodeNumber;
    int m_INA_portNumber;
};

#endif // PLC_H
