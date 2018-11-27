# brsnmp [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
Execute PVI-SNMP commands for B&amp;R plcs

### Example application
Use within Runtime Utility Center *.pil file to setup a new CPU via network:
```
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --ipMethod=0 --filter=PPC7", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=1000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /ANSL=1 /PT=11169", "WT=30"
Download ".\PPC7xG43.s14", "ROM", "MN=PPC7xG43 MV=1.0"
Call "brsnmp.exe", "--ipAddress=192.168.0.14 --subnetMask=255.255.255.0 --ipMethod=0 --filter=PPC7", "HideWindow=1"
Connection "/IF=tcpip /LOPO=11159 /SA=113", "/RT=1000 /AM=* /SDT=5 /DAIP=192.168.0.14 /REPO=11159 /ANSL=1 /PT=11169", "WT=30"
OnErrorResume
Transfer ".\RucPackage_Config2.zip", "InstallMode=ForceInitialInstallation TryToBootInRUNMode=1 ResumeAfterRestart=1"
ClearError
Coldstart "120"
```

## Usage:
```
-h, --help                       this help
-v, --version                    software version
-f=<EXPR>, --filter=<EXPR>       Filter(regular expression following ECMA syntax)
-t=<MS>, --timeout=<MS>          SNMP Line Timeout [ms], default: <3000>
-l, --list                       list MAC addresses of available B&R plc
-d, --details                    list detailed properties of available B&R plc
--<NAME>=<VALUE>                 set parameter <NAME> to <VALUE> (see --details)
```

### `--version`
outputs the program version in JSON e.g.
```
{
"brsnmp":"0.1",
"build-time":"09:47:24",
"build-date":"Nov 27 2018"
}
```

### `--filter`
sets a filter for the operations to be executed. The filter is applied to a --details - list.
The filter is a ECMA Regual Expression (https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions).
But in most cases a simple text should be sufficient.

### `--list`
outputs a list of MAC addresses (PLCs) as JSON array e.g.
```
[
"00-60-65-16-fd-da"
,"00-60-65-23-bd-1a"
,"00-60-65-32-cb-48"
,"00-60-65-44-92-0f"
]
```

### `--details`
outputs a detailed list of reachable PLCs as JSON array of objects e.g.
```
[
{
  "targetTypeDescription":"X20CP1583",
  "serialNumber":"D45B0168612",
  "cfSerialNumber":"000060076643A1000085",
  "arVersion":"B04.45",
  "arVersionNorm":"04.45.2",
  "arState":"4",
  "arBootPhase":"40",
  "deviceName":"IF2",
  "macAddress":"00-60-65-16-fd-da",
  "baudrate":"0",
  "ipAddress":"192.168.0.14",
  "subnetMask":"255.255.255.0",
  "ipMethod":"0",
  "nodeNumber":"129",
  "inaNodeNumber":"129",
  "inaActivated":"1",
  "inaPortNumber":"11159",
  "snmpMode":"2",
  "clusterHostName":"",
  "anslPortNumber":"11169",
  "processCtrlState":"65535",
  "redAvailable":"0",
  "switchoverLevel":"0",
  "hostName":"br-automation",
  "defaultGateway":"",
  "dnsActivated":"0",
  "dnsFromDhcp":"0",
  "dnsServer1":"",
  "dnsServer2":"",
  "dnsServer3":""
}
]
```

### `--<NAME>=<VALUE>`
sets parameter `<NAME>` to `<VALUE>` e.g.
```
ipAddress=192.168.0.14
subnetMask=255.255.255.0
```

## Disclaimer
brsnmp comes „as is“, e.g. without support and warranty.
You can freely copy it but use it at your own risk.

## PVI
brsnmp requires PVI 4.x.
it needs a previously installed [PVI Development Setup](https://www.br-automation.com/en/downloads/#categories=Software/Automation+NET%2FPVI) to run.
Beware: if you do not own a PVI license **1TG0500.02** (+ TG Guard e.t. 0TG1000.02) PVI will run for two hours only. After this period brwatch will stop working and PVI-Manager must be stopped and restarted again.
So, do not blame brsnmp for that and contact your local B&R office to buy it.

## Development
### Compiler
   Mingw32 (32-Bit)
   http://www.mingw.org/
   
### IDE
   Code::Blocks
   http://www.codeblocks.org/
   
### Dependencies
Thanks to https://github.com/adishavit/argh for the command line parser


