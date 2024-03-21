# Software Library RaSTA Protocol Reference Stack according to EN 50128:2011 SIL4
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://spdx.org/licenses/MIT.html)
[![REUSE status](https://api.reuse.software/badge/github.com/SchweizerischeBundesbahnen/sbb-rasta-stack)](https://api.reuse.software/info/github.com/SchweizerischeBundesbahnen/sbb-rasta-stack)

This framework contains the SBB AG (Swiss Federal Railways) Software Library RaSTA
Protocol Reference Stack. It allows an integration of the source code in an
application for e.g. EULYNX Object Controllers.

:information_source: This Software Library Code was developed according to EN 50128:2011 SIL4.
SBB AG used the RaSTA Stack for proof of concept of an EULYNX Object Controller.

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
* Integrate and adapt the library source code to your application, see [User Manual](docs/SBB-RaSTA-084-UserManual-2.pdf)

### Checking File Integrity
To verify the integrity of the source files, their MD5 checksum can be checked against the published MD5 checksums from the ISA Inspection Report. All published checksums are stored in the [rasta_stack_content.md5](rasta_stack_content.md5)-File. They can be checked using a ***bash shell*** with the following command: `md5sum -c rasta_stack_content.md5`
> ***Important***: File checksums were generated with line ending set to `CRLF`.

### Building Library / Running Unit Tests
A basic CMake build system is integrated so the static libraries can be built
and a `zip`-package be exported including this libraries.
Additionally the unit tests can be build and executed.
The following tools are need for the instructions below:
* CMake
* Ninja
* GNU C Compiler
* Google Test/Mock ([Version 1.12.1](https://github.com/google/googletest/releases/tag/release-1.12.1) was used for development)

> `GoogleTest` library must be installed on your system.\
A detailed guide can be found in the official [GoogleTest documentation](https://github.com/google/googletest/blob/release-1.12.1/googletest/README.md#standalone-cmake-project).


| Steps | Details |
|:---|:---|
| 1. Open a ***Bash shell*** | - |
| 2. Navigate into cloned git workspace | - |
| 3. Build cmake project | `cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -S. -B./build -G Ninja` |
| 3.1 If `cmake` is not finding your `GoogleTest` installation by itself, the following ***option*** can be set to specify your installation path | `-DGOOGLE_TEST_PATH:PATH="<path-to-your-google-test-installation>"` |
| 4. Build RaSTA libraries | `cmake --build ./build --config Debug --target all --` |
| 5. Run all unit tests | `ctest --test-dir ./build` |
| To run only a subset of the tests, one of the following `options` can be passed to ctest: | - |
| 5.1 Unit tests ***Safety & Retransmission Layer*** | `-E "^gtest_red.*\|^gtest_srIntegration.*"` |
| 5.2 Unit tests ***Redundancy Layer*** | `-E "^gtest_sr.*\|^gtest_redIntegration.*\|^gtest_ra.*"` |
| 6. Run CPack to build zip File with the static RaSTA libraries | `cpack` |
| 7. Export the generated ***RastaProtocolReferenceStack-*.zip*** Package and use the library | - |

### Further documentation
The following documents are available internally at SBB or on request (please open an [issue](CONTRIBUTING.md#submit-issue)):
* ISA Inspection Report ("Inspection Report about the Software Library RaSTA
  Protocol Reference Stack, Version 1.1.0, for the Integration into an EULYNX OC
  Platform, according to EN 50128:2011")
* Specifications according EN 50128:2011 SIL4
* Verification kit for component tests, integration tests according EN 50128:2011 SIL4

## License
The code is released under the [MIT](LICENSES/MIT.txt) license.
