# @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
#
# @file GenerateMock.py
#
# @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
#
# @version 9f7ad4501687c5a9fd2faf06c5940ca6cbbce24d
#
# @brief Mock file generator

import os
import sys
import re
from pyclibrary import CParser
import numpy as np

################################
# PROPERTIES
################################

nSpacesIndent = 2

################################
# FUNCTIONS
################################

class FuncDef:
    name: str
    retValType: str
    argumentString: str
    argNameList: str

## @brief   read source header file
#
#  This function reads the given header file of the module to be mocked and extracts the function definitions
#
#  @param   filename    header file to be parsed
def readSrcHeaderFile(filename):
    # parse header file
    parser = CParser(files=[filename], macros={})
    #parser.print_all()

    # print unparsed code
    unparsed = parser.process_all(return_unparsed=True)
    unparsed = [x for x in unparsed if x != '\n']
    if (len(unparsed) > 0):
        print('!!!WARNING!!!')
        print('Following elements were not parsed, probably due to undefined macros')
        for u in unparsed:
            print(u)
        print('!!!WARNING!!!')

    # make list of functions
    functionSignature = parser.defs['functions']
    for name, v in functionSignature.items():
        myFunc = FuncDef()
        myFunc.name = name
        retVal = v[0]
        params = v[1]
        myFunc.retValType = retVal.type_spec.strip()
        retQuals = str(retVal.type_quals)
        ret = retQuals.find('inline')
        if ret > 0:
            # function is inline, no need to mock
            continue
        argString = ''
        argList = ''

        for i, pTuple in enumerate(params):
            name = pTuple[0]
            pType = pTuple[1]
            # add qualifiers (like const...)
            quals = pType.type_quals[0]
            for qual in quals:
                argString += qual + ' '
            # add argument type
            argString += pType[0]
            # add declarators (like *)
            decl = pType.declarators
            for d in decl:
                # if there is a [] after the variable name, indicating that it's a pointer
                # the decl list will contain a list where the first element is the length of
                # the array, or -1 if the array type is incomplete
                if type(d) is list:
                    length_value = d[0]
                    if length_value == -1:
                        # it's a empty "[]" which we can translate to a pointer
                        argString += ' *'
                else:
                    argString += ' ' + d
            # add parameter name
            if str(name) != 'None':
                argString += ' ' + str(name)
                argList += str(name)
            # add separator
            if( i != len(params)-1):
                argString += ', '
                argList += ', '

        myFunc.argumentString = argString
        myFunc.argNameList = argList

        #print(myFunc.retValType + ' ' + myFunc.name + '(' + myFunc.argumentString + ')')

        functionList.append(myFunc)

## @brief   read includes from header file
#
#  This function reads the include statements from the given header file
#
#  @param   filename    header file to be parsed
def getIncludes(filename):
    # open the header file and read lines
    f = open(filename, 'r')
    lines = f.readlines()
    f.close()

    # extract includes from header file
    for line in lines:
        if '#include "' in line:
            includesList.append(line.strip())


## @brief   write a line
#
#  This function writes a given string to a file represented by fileHdl with given indent
#  (a line break is added automatically to the end of the string)
#
#  @param   fileHdl     file handle to write to
#  @param   indentLvl   level of the indent (0, 1, 2, ...)
#  @param   str         string to be written
def writeLine(fileHdl, indentLvl, str):
    fileHdl.write((' ' * indentLvl * nSpacesIndent) + str + '\n')

## @brief   write the mock header file
#
#  This function writes the header file for the mock
#
#  @param   outDir      output directory
#  @param   pathname    path name (used for include guard)
#  @param   filename    name of the original header file (used for module name)
def writeMockHeaderFile(outDir, pathname, filename):
    # extract module name from file name and create mock name
    moduleName = filename.split('_')[0]
    mockName = moduleName + 'Mock'

    # create include guard
    pathName = pathname.replace('\\', '_').replace('/', '_')
    guardName = pathName.upper() + '_' + moduleName.upper() + '_MOCK_HH_'

    # create output file name and output file with path
    outFileName = moduleName + '_mock.hh'
    outFile = os.path.join(outDir, outFileName)

    # open the output file
    f = open(outFile, 'w')

    # write file header
    writeLine(f, 0, '/**')
    writeLine(f, 0, ' * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern')
    writeLine(f, 0, ' *')
    writeLine(f, 0, ' * @file ' + outFileName)
    writeLine(f, 0, ' *')
    writeLine(f, 0, ' * @author TST, CSA Engineering AG, CH-4500 Solothurn')
    writeLine(f, 0, ' *')
    writeLine(f, 0, ' * @version 9f7ad4501687c5a9fd2faf06c5940ca6cbbce24d
    writeLine(f, 0, ' *')
    writeLine(f, 0, ' * @brief Mock header file for the ' + moduleName + ' module')
    writeLine(f, 0, ' */')
    writeLine(f, 0, '')

    # write include guard
    writeLine(f, 0, '#ifndef ' + guardName)
    writeLine(f, 0, '#define ' + guardName)
    writeLine(f, 0, '')

    # write includes
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, '// Includes')
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, '#include "gmock/gmock.h"')
    for inc in includesList:
        writeLine(f, 0, inc)
    writeLine(f, 0, '')

    # write mock class
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, '// Mock class')
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, '/**')
    writeLine(f, 0, ' * @brief Mock class for the rasta ' + moduleName + ' class')
    writeLine(f, 0, ' */')
    writeLine(f, 0, 'class ' + mockName + ' {')
    writeLine(f, 0, 'private:')
    writeLine(f, 1, 'static ' + mockName +' *instance;  ///< ' + mockName + ' instance')
    writeLine(f, 0, '')
    writeLine(f, 0, 'public:')
    writeLine(f, 1, '/**')
    writeLine(f, 1, '* @brief ' + mockName + ' constructor')
    writeLine(f, 1, '*/')
    writeLine(f, 1, mockName +'();')
    writeLine(f, 1, '/**')
    writeLine(f, 1, '* @brief ' + mockName + ' destructor')
    writeLine(f, 1, '*/')
    writeLine(f, 1, 'virtual ~' + mockName + '();')
    writeLine(f, 0, '')

    # write MOCK_METHOD macros
    for func in functionList:
        writeLine(f, 1, '/**')
        writeLine(f, 1, '* @brief Mock Method object for the ' + func.name + ' function')
        writeLine(f, 1, '*/')
        if func.argumentString == 'void':
            writeLine(f, 1, 'MOCK_METHOD(' + func.retValType + ', ' + func.name + ', ());')
            writeLine(f, 0, '')
        else:
            writeLine(f, 1, 'MOCK_METHOD(' + func.retValType + ', ' + func.name + ', (' + func.argumentString + '));')
            writeLine(f, 0, '')

    # write mock object instance and getter
    writeLine(f, 1, '/**')
    writeLine(f, 1, '* @brief Get the Instance object')
    writeLine(f, 1, '*')
    writeLine(f, 1, '* @return ' + mockName +'*')
    writeLine(f, 1, '*/')
    writeLine(f, 1, 'static ' + mockName + ' *getInstance() {')
    writeLine(f, 2, 'return instance;')
    writeLine(f, 1, '}')
    writeLine(f, 0, '};')
    writeLine(f, 0, '')

    # end file and close
    writeLine(f, 0, '#endif // ' + guardName)
    f.close()

## @brief   write the mock source file
#
#  This function writes the source file for the mock
#
#  @param   outDir      output directory
#  @param   filename    name of the original header file (used for module name)
def writeMockSourceFile(outDir, filename):
    # extract module name from file name and create mock name
    moduleName = filename.split('_')[0]
    mockName = moduleName + 'Mock'

    # create output file name and output file with path
    outFileName = moduleName + '_mock.cc'
    outFile = os.path.join(outDir, outFileName)

    # open the output file
    f = open(outFile, 'w')

    # write file header
    writeLine(f, 0, '/**')
    writeLine(f, 0, ' * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern')
    writeLine(f, 0, ' *')
    writeLine(f, 0, ' * @file ' + outFileName)
    writeLine(f, 0, ' *')
    writeLine(f, 0, ' * @author TST, CSA Engineering AG, CH-4500 Solothurn')
    writeLine(f, 0, ' *')
    writeLine(f, 0, ' * @version 9f7ad4501687c5a9fd2faf06c5940ca6cbbce24d
    writeLine(f, 0, ' *')
    writeLine(f, 0, ' * @brief Mock source file for the ' + moduleName + ' module')
    writeLine(f, 0, ' */')
    writeLine(f, 0, '')

    # write includes
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, '// Includes')
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, '#include "' + moduleName + '_mock.hh"')
    writeLine(f, 0, '')

    # create instance
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, '// Local Variable Definitions')
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, mockName + ' * ' + mockName + '::instance = NULL;')
    writeLine(f, 0, '')

    # write constructor / destructor
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, '// Global Function Implementations')
    writeLine(f, 0, '// -----------------------------------------------------------------------------')
    writeLine(f, 0, mockName + '::' + mockName + '() {')
    writeLine(f, 1, 'instance = this;')
    writeLine(f, 0, '}')
    writeLine(f, 0, '')
    writeLine(f, 0, mockName + '::~' + mockName + '() {')
    writeLine(f, 1, 'instance = NULL;')
    writeLine(f, 0, '}')
    writeLine(f, 0, '')

    writeLine(f, 0, '#ifndef DOXYGEN_SHOULD_SKIP_THIS')
    writeLine(f, 0, '')

    # write C function wrappers
    writeLine(f, 0, 'extern "C" {')
    writeLine(f, 0, '')

    for func in functionList:
        writeLine(f, 1, func.retValType + ' ' + func.name + '(' + func.argumentString + '){')
        if func.retValType == 'void':
            writeLine(f, 2, 'ASSERT_NE(' + mockName + '::getInstance(), nullptr) << "Mock object not initialized!";')
            writeLine(f, 2, mockName + '::getInstance()->' + func.name + '(' + func.argNameList + ');')
        else:
            writeLine(f, 2, 'if(' + mockName + '::getInstance() == nullptr) {')
            writeLine(f, 3, 'ADD_FAILURE() << "Mock object not initialized!";')
            writeLine(f, 2, '}')
            writeLine(f, 2, 'return ' + mockName + '::getInstance()->' + func.name + '(' + func.argNameList + ');')
        writeLine(f, 1, '}')

    writeLine(f, 0, '}')

    writeLine(f, 0, '')
    writeLine(f, 0, '#endif /* DOXYGEN_SHOULD_SKIP_THIS */')

    # close file
    f.close()

## @brief   print the mock generator header to the console
#
def printHeader():
    print('************************************************')
    print('***************  MOCK GENERATOR  ***************')
    print('************************************************')
    print('')

## @brief   print the usage instructions for the mock generator to the console
#
def usage():
    printHeader()
    print('Usage:')
    print('- Specify the output directory where the mock files shall be placed using the -o or --output option.')
    print('- Specify the header file of the module to be mocked using the -f or --file option.')
    print('  (it is also possible to add more than one header file, if they belong to the same module.)')
    print('')
    print('Example:')
    print('    python GenerateMock.py -o C:/Work/git_repos/windows_rasta/source/modules/rasta_common/tests/mocks -f C:/work/project/inc/myModule_myheader.h')
    print('')
    print('Output:')
    print('The script creates the following files:')
    print('    - <moduleName>_mock.hh       (Mock header file)')
    print('    - <moduleName>_mock.cc       (Mock source file)')

if __name__ == '__main__':
    # global variables
    files = []
    functionList = []
    includesList = []
    args = sys.argv[1:]

    # display help
    if '-h' in args or '--help' in args:
        usage()
        sys.exit(2)
    # check for valid parameter
    if len(args) > 1 and (args[0] == '-o' or args[0] == '--output') and (args[2] == '-f' or args[2] == '--file'):
        # get output directory
        outDir = args[1].strip()
        # get filename
        filename = re.split(r'/|\\', args[3])
        filename = filename[-1]
        # get list of header files to be parsed
        for i in range(len(args) - 3):
            files.append(args[i + 3])

        # print to stdout (header and file list)
        printHeader()
        print('Generating mock files for {}\n...'.format(filename))
        if(len(args) > 4):
            print('File list: ')
            print(*files, sep='\n')

        # parse header files
        for file in files:
            if os.path.exists(file):
                readSrcHeaderFile(file)
                getIncludes(file)
            else:
                print('ERROR: file not found: {}'.format(file))
                print('Check your file list and try again...')
                sys.exit(2)

        # get path name from output directory (used for include guard)
        pathName = os.path.dirname(outDir + '\\')
        pathName = pathName[pathName.index('source'):]

        # remove duplicates from includes list
        np.unique(includesList).tolist()

        # write mock header and source files
        writeMockHeaderFile(outDir, pathName, filename)
        writeMockSourceFile(outDir, filename)

        print('Files successfully created!')

    else:
        usage()
        sys.exit(2)
