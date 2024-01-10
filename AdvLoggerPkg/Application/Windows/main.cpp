#include "pch.h"
#include "main.h"
#include <Windows.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace std;

//
// Create log file by retrieving AdvancedLogger
//
fstream ReadLogFromUefiInterface()
{
    cout << "in func\n";

    int  Status = 0;
    int i = 0;
    DWORD length;
    DWORD err;

    stringstream varName;
    wchar_t *varBuffer = new wchar_t[MAX_VAR_LENGTH + 1];
    PVOID* var = (PVOID*) varBuffer;

    // string to LPCWSTR conversion
    string tmpGuid = "{a021bf2b-34ed-4a98-859c-420ef94f3e94}";
    wstring tmpGuidW = wstring(tmpGuid.begin(), tmpGuid.end());
    LPCWSTR guid = tmpGuidW.c_str();

    // create temporary file
    // FILE* pFile = tmpfile();
    fstream logfile = fstream();

    if (logfile.fail()) {
        cout << "file can't be opened\n";

        Status = CONS_ERROR;
        return logfile;
    }

    //
    // Parse variables by index until reached end of log
    //
    cout << "enter while\n";

    while (Status == 0) {
        varName << "V" << i;
        cout << "iteraion" << i << "\n";
        string tempStr = varName.str();
        wstring tmp = wstring(tempStr.begin(), tempStr.end());
        LPCWSTR wideString = tmp.c_str();
        wchar_t *varNameP = new wchar_t[tmp.size() + 1];
        wcscpy_s(varNameP, tmp.size()+1, wideString);
        LPCWSTR varNameConst = varNameP;

        //
        // Retrieve one advanced logger indexed variable via kernel32 API
        //
        length = GetFirmwareEnvironmentVariableW(varNameConst, guid, var, MAX_VAR_LENGTH);
        cout << "FW Var length " << length << "\n";


        if (length == 0) {
            err = GetLastError();
            cout << "errnno " << err << "\n";


            // If error is NOT FOUND, reached end of variables, otherwise process error
            if (err != 203) {
                Status = EFI_ERROR;
                return logfile;
            }
            else {
                Status = (int)err;
            }
        }

        if (Status == 0) {
            i += 1;
            logfile << var;
            // fprintf(fp, "Name: %s\t Roll no: %d \tMarks: %f \n", name, roll_no, marks);

        }
        else if (i == 0) {
            printf("file can't be opened\n");
            Status = CONS_ERROR;
            return logfile;
        }
        else {
           printf("finished reading %d variables\n", i);
        }

        delete[] varNameP;
        ZeroMemory(varBuffer, MAX_VAR_LENGTH);
    }

    Status = SUCCESS;
    return logfile;
}



int main(int argc, char** argv)
{
    fstream logfile;
    char* filename = argv[1];

    //
    // Create or open log file
    //
    if (argc < 2) {
        // create
        cout << "call func with logfile\n";
        logfile = ReadLogFromUefiInterface();
    }
    else {
        logfile = fstream(filename);
    }

    if (!logfile.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return CONS_ERROR;
    }

    //
    // Get logfile size
    //
    logfile.seekg(0, ios::end);
    size_t lfsize = logfile.tellg();
    logfile.seekg(0, ios::beg);

    vector<char> buffer(lfsize);
    logfile.read(buffer.data(), lfsize);

    //
    // TODO Process Message
    //
  
    // try { 
    //  ProcessMessages(outfstream);
    // }

    return SUCCESS;
}
