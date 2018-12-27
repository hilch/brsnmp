
# brsnmp 
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

Execute PVI-SNMP commands for B&amp;R plcs

## Example application

Use within Runtime Utility Center *.pil file to setup a new CPU in BOOT state via network

see [Examples](https://github.com/hilch/brsnmp/blob/master/examples.md)

## Usage

```batch

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

sets a filter for the operations to be executed (highly recommended if you plan to set the ip settings :grimacing: ). The filter is applied to a --details - output.
The filter is an ECMA Regual Expression (https://regex101.com/#javascript).
But in most cases a simple text should be sufficient.

```--filter=targetTypeDescription.+X20CP\d{4}```

sets filter to all X20-CPUs *(regular expression)*

```--filter=arVersion.+I.4\.34```

sets filter to AR-Version I4.3.4  *(regular expression)*

```--filter=macAddress.+00-60-65-3a-39-10```

sets filter to given MAC address *(regular expression)*

```--filter=X20CP3585```

sets filter for X20CP3585. *(simple filter, no regex. 'X20CP3585' must not have be used as hostname in this case)*

```--filter=@LAST```
sets filter to the expression from previous call (V1.0++)
(all filter expressions are stored in TEMP directory)

```--filter=```

clears previous filter settings and sets filter to 'all' (V1.0++)

```--filter=.*```

sets filter to 'all'. (As this expression is default, it can be omitted.)


### `--list`

outputs a list of MAC of reachable PLCs as JSON array e.g.
```json
[
"00-60-65-16-fd-da"
,"00-60-65-23-bd-1a"
,"00-60-65-32-cb-48"
,"00-60-65-44-92-0f"
]
```

### `--details`

outputs a detailed list of reachable PLCs as JSON array of objects e.g.
```json
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

possible values and their meanings:
![SNMPVariables](https://github.com/hilch/brsnmp/blob/master/doc/SnmpVariables.PNG)


**FAQ**: how can I store this list ?

**Answer**: ```brsnmp --details > details.json```

### `--<NAME>=<VALUE>`

sets parameter `<NAME>` to `<VALUE>` e.g.

```batch

ipAddress=192.168.0.14
subnetMask=255.255.255.0

```
if <VALUE> is surrounded by '%' character an environment variable is used e.g.

```batch

ipAddress=%IP_ADDRESS%

```


(only parameters which are declared RW can be written. see description of --details)

## Disclaimer

brsnmp comes „as is“, e.g. without support and warranty.
You can freely copy it but use it at your own risk.

## PVI

brsnmp requires PVI 4.x.
it needs a previously installed [PVI Development Setup](https://www.br-automation.com/en/downloads/#categories=Software/Automation+NET%2FPVI) to run.
Beware: if you do not own a PVI license **1TG0500.02** (+ TG Guard e.t. 0TG1000.02) PVI will run for two hours only. After this period brsnmp will stop working and PVI-Manager must be stopped and restarted again.
Contact your local B&R office for a license.

If using brsnmp with Runtime Utility Center

```batch

StartPviMan "LoadLocal"

```

at the beginning of *.pil and

```batch

StopPviMan
```  

at the end of *.pil is recommended.

## Development

### Compiler

[Mingw32 (32-Bit)](http://www.mingw.org/)

### IDE

[Code::Blocks](http://www.codeblocks.org/)

### Dependencies

Thanks to https://github.com/adishavit/argh for the command line parser
