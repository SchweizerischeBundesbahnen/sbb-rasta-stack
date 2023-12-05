# Software Libary RaSTA Protocol Reference Stack
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://spdx.org/licenses/MIT.html)
[![REUSE status](https://api.reuse.software/badge/github.com/SchweizerischeBundesbahnen/sbb-rasta-stack)](https://api.reuse.software/info/github.com/SchweizerischeBundesbahnen/sbb-rasta-stack)

This framework contains the SBB (Swiss Federal Railways) Software Library RaSTA
Protocol Reference Stack. It allows an integration of the source code in an
application for EULYNX Object Controllers.

:information_source: This Software Library Code was developed for proof of
concept

## Background
In a cross-industry approach with the aviation industry, Schweizerische
Bundesbahnen AG (SBB AG) has procured and integrated a platform that allows SBB
to perform proofs of concept (PoC) for the EULYNX Object Controller (OC). One of
these PoC has demonstrated that it is possible to run both safety-relevant and
non-safety-relevant software simultaneously and independently of each other on a
common processor system using currently available operating systems.

The EULYNX OC behaviour has been demonstrated using a simulated EULYNX
interlocking, a real main signal and other simulated external equipment.

The paper was published in [SIGNAL+DRAHT | Issue 06/2023 | "Integrated safety and security through software-based segregation in the EULYNX Object Controller"](https://eurailpress-archiv.de/SingleView.aspx?show=5301114)

## Content & Version
| Version | Content |
|:---|:---|
| 1.1.0 | Source Code (Safety and Retransmission Layer & Redundancy Layer) <br/>Source Code Documentation <br/>User Manual |

## Getting Started
### Library Integration
* To use this Software Library develop your application according to EN 51026, EN 50128.
* Integrate and adapt the library source code to your application, see [User Manual](docs/SBB-RaSTA-084-UserManual-1.pdf)

### Building Library
A basic CMake build system is integrated so the static libraries can be built
and a `zip`-package be exported including this libraries.
The following tools are need for the instructions below:
* CMake
* Ninja
* GNU C Compiler

| Steps | Details |
|:---|:---|
| 1. Open a ***Bash shell*** | - |
| 2. Navigate into cloned git workspace | - |
| 3. Build cmake project | `cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -S. -B./build -G Ninja` |
| 4. Build RaSTA libraries | `cmake --build ./build --config Debug --target all --` |
| 5. Enter ***build*** folder | `cd build` |
| 6. Run CPack to build zip File | `cpack` |
| 7. Export the generated ***RastaProtocolReferenceStack-*.zip*** Package and use the library | - |

### Further documentation
The following documents are available internally at SBB or on request (please open an [issue](CONTRIBUTING.md#submit-issue)):
* ISA Inspection Report ("Inspection Report about the Software Library RaSTA
  Protocol Reference Stack, Version 1.1.0, for the Integration into an EULYNX OC
  Platform, according to EN 50128:2011")
* Specifications according EN 50128:2011 SIL4
* Verification kit for component tests, integration tests according EN 50128:2011 SIL4
* Unit Tests according EN 50128:2011 SIL4

## License
The code is released under the [MIT](LICENSES/MIT.txt) license.
