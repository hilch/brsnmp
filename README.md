
# brsnmp

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Made For B&R](https://github.com/hilch/BandR-badges/blob/main/Made-For-BrAutomation.svg)](https://www.br-automation.com)

Execute PVI-SNMP commands for B&amp;R plcs

## Intention

SNMP commands enable remote network configuration and monitoring of B&R PLCs without physical access.
Anyone who uses B&R Automation Studio has unconsciously used SNMP commands at some point when temporarily assigning a new IP address to a controller on the network via the "Online/Settings" menu.
Unfortunately, these functions are not yet available in the Runtime Utility Center (RUC).
The SNMP functions are part of PVI. This project therefore utilizes these functions and provides them as simple commands.

Please note that Automation Studio 6 disables the SNMP protocol by default in new projects. However, if the target is switched to BOOT mode, SNMP should be enabled again.

## PVI

Based on my findings so far, PVI version 4.12 can also be used with Automation Runtime >= 6.0. Therefore, brsnmp.exe version 1.0 should still work in this case as well.

However, if only PVI version 6 is installed, brsnmp.exe version 1.1 must be used.
There were some SNMP issues with PVI 6, but these were resolved in PVI 6.5.2.

## Download binary

download MS-Windows binary from [https://github.com/hilch/brsnmp/releases](https://github.com/hilch/brsnmp/releases)

## Building

### Requirements

- **Windows**: 32-bit MinGW compiler (for PviCom.lib compatibility)
- **MSYS2**: Recommended for managed, pre-built toolchain

#### Setup (One-time)

1. Download and run the [MSYS2 installer](https://www.msys2.org/) for Windows
2. Open the **MSYS2 MINGW32** terminal
3. Install the 32-bit MinGW toolchain:

   ```bash
   pacman -S --noconfirm --needed mingw-w64-i686-toolchain
   ```

### Build Methods

#### Option 1: PowerShell Build Script (Recommended)

```powershell
.\build.ps1 release      # Build release executable
.\build.ps1 debug        # Build debug executable  
.\build.ps1 clean        # Clean build outputs
.\build.ps1 -Help        # Show usage
```

#### Option 2: GNU Make (Auto-detects MSYS2)

```powershell
mingw32-make release     # Build release executable
mingw32-make debug       # Build debug executable
mingw32-make clean       # Clean build outputs
```

#### Option 3: VS Code Tasks

- Press `Ctrl+Shift+B` to run the default build task (Release)
- Use `Ctrl+Shift+P` → `Tasks: Run Task` to select other tasks (Debug, Clean, etc.)

### Output

Compiled binaries are placed in:

- **Release**: `bin/Release/brsnmp.exe` (optimized, no symbols)
- **Debug**: `bin/Debug/brsnmp.exe` (with debug symbols)

## Example application(s)

Use within Runtime Utility Center *.pil file to setup a *new* or *unreachable* CPU in BOOT state via network without need of Automation Studio.

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

```powershell
{
"brsnmp":"0.1",
"build-time":"09:47:24",
"build-date":"Nov 27 2018"
}
```

### `--filter`

sets a filter for the operations to be executed (highly recommended if you plan to set the ip settings :grimacing: ). The filter is applied to a --details - output.
The filter is an ECMA Regual Expression [regex101.com](https://regex101.com/#javascript).
But in most cases a simple text should be sufficient.

```--filter=targetTypeDescription.+X20CP\d{4}```

sets filter to all X20-CPUs *(regular expression)*

```--filter=arVersion.+I.4\.34```

sets filter to AR-Version I4.3.4  *(regular expression)*

```--filter=macAddress.+00-60-65-3a-39-10```

sets filter to given MAC address *(regular expression)*

```--filter=X20CP3585```

sets filter for X20CP3585. *(simple filter, no regex. 'X20CP3585' must not have be used as hostname in this case)*

```--filter=$LAST```
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

| Variable name | Data format | I/O | Description |
| --- | --- | --- | --- |
| MAC addresses | string | R | List of MAC addresses for all B&R PLCs with SNMP support that can be reached on the network. Entries are separated by a tab character (0x09). Each entry is 18 characters long (including tab character).<br>Each read access is not ended until the response timeout (/RT parameter) configured for the device object has expired. During this time, response data is gathered from the individual PLCs. |
| snmpMode | number | R | 0 ... SNMP not enabled, 1 ... SNMP read-only, 2 ... SNMP enabled |
| targetTypeDescription | string | R | Description of the CPU (e.g. 3CP360.60-1). |
| serialNumber | string | R | PLC serial number. |
| cfSerialNumber | string | R | CompactFlash serial number |
| arVersion | string | R | AR version |
| arVersionNorm | string | R | Standardized AR version |
| arState | number | R | AR status: -1 ... Undefined, 1 ... Boot, 2 ... Diagnose, 3 ... Service, 4 ... Run |
| arBootPhase | number | R | Boot phase: -1 ... Undefined, 10 ... Boot, 20 ... Hardware init / Firmware update, 30 ... Initialization subroutine, 40 ... AR ready |
| deviceName | string | R | Ethernet device name (e.g. SS1.IF2) |
| macAddress | string | R | MAC address (e.g. 00-60-ef-01-15-5a) |
| baudrate | number | RW | Ethernet baud rate: 0 ... Auto, 10 ... 10 MBit HD (half-duplex), 11 ... 10 MBit FD (full-duplex), 12 ... 10 MBit Auto, 100 ... 100 MBit HD, 101 ... 100 MBit FD, 102 ... 100 MBit Auto |
| ipAddress | string | RW | IP address |
| subnetMask | string | RW | Subnet mask (e.g. 255.255.255.0) |
| ipMethod | number | RW | 0 ... Static IP, 1 ... DHCP client |
| nodeNumber | number | R | Hardware node number. |
| hostName | string | RW | Hostname (e.g. BR_HOST) |
| defaultGateway | string | RW | Default gateway. |
| dnsActivated | number | RW | 0 ... INA not enabled, 1 ... INA enabled |
| dnsFromDhcp | number | RW | Get DNS parameters from DHCP: 0 ... Not enabled, 1 ... Enabled |
| dnsServer1 | string | RW | DNS server 1 |
| dnsServer2 | string | RW | DNS server 2 |
| dnsServer3 | string | RW | DNS server 3 |

**FAQ**: how can I store this list ?

**Answer**: ```brsnmp --details > details.json```

### `--<NAME>=<VALUE>`

sets parameter `<NAME>` to `<VALUE>` e.g.

```batch

ipAddress=192.168.0.14
subnetMask=255.255.255.0

```

```batch
if <VALUE> is surrounded by '%' character an environment variable is used e.g.

```batch

ipAddress=%IP_ADDRESS%

```

(only parameters which are declared RW can be written. see description of --details)

## Disclaimer

brsnmp comes „as is“, e.g. without support and warranty.
You can freely copy it but use it at your own risk.
brsnmp is made available free of charge.

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

If you prefer a IDE [Code::Blocks](http://www.codeblocks.org/) is suitable.
Find a project file for it in the repo.

### Dependencies

Thanks to [https://github.com/adishavit/argh](https://github.com/adishavit/argh) for the command line parser.
Additional thatnks to [https://github.com/nlohmann/json](https://github.com/nlohmann/json) for the JSON lib.

