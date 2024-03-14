#include "main.h"
#include <Windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <codecvt>

using namespace winrt;
using namespace Windows::Foundation;
using namespace std;

//
// Elevate current process system environment privileges to admin
//
int ElevateCurrentPrivileges()
{
	HANDLE ProcessHandle = GetCurrentProcess();
    DWORD DesiredAccess = TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY;;
    HANDLE hProcessToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    int Status = 0;

    if (!LookupPrivilegeValue(NULL, L"SeSystemEnvironmentPrivilege", &luid)) {
        Status = GetLastError();
        printf("LookupPrivilegeValue failed with error %u\n", Status);
        return Status;
    }

    if (!OpenProcessToken(ProcessHandle, DesiredAccess, &hProcessToken)) {
        Status = GetLastError();
        cout << "Failed to open process token\n";
        return Status;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hProcessToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
        Status = GetLastError();
        if (Status == ERROR_NOT_ALL_ASSIGNED)

        {
            printf("The token does not have the specified privilege. \n");
            return Status;
        }
        else
        {
            printf("AdjustTokenPrivileges failed with error %u\n", Status);
            return Status;
        }
    }

    CloseHandle(ProcessHandle);
    return SUCCESS;
}

//
// Create log file by retrieving AdvancedLogger variables from UEFI interface
//
int ReadLogFromUefiInterface(fstream& lfile)
{
    cout << "in ReadLogFromUefiInterface\n";

    int  Status = 0;
    int i = 0;
    DWORD length = 0;
    DWORD err = 0;

    stringstream varName;
    char* varBuffer = (char *) malloc(MAX_VAR_LENGTH + 1);

    // string to LPCWSTR conversion
    string tmpGuid = "{a021bf2b-34ed-4a98-859c-420ef94f3e94}";
    wstring tmpGuidW = wstring(tmpGuid.begin(), tmpGuid.end());
    LPCWSTR guid = tmpGuidW.c_str();

    //
    // Parse variables by index until reached end of log
    //
    while (Status == 0) {
        string tmpVarName = "V" + to_string(i);
        wstring tmpVarNameW = wstring(tmpVarName.begin(), tmpVarName.end());
        LPCWSTR varNameConst = tmpVarNameW.c_str();

        //
        // Retrieve one advanced logger indexed variable via kernel32 API
        //
        length = GetFirmwareEnvironmentVariableW(varNameConst, guid, varBuffer, MAX_VAR_LENGTH);
        cout << "FW Var length " << length << "\n";


        if (length == 0) {
            err = GetLastError();
            cout << "errnno " << err << "\n";


            // If error is ERROR_NOT FOUND, reached end of variables
            if (err != 203) {
                Status = EFI_ERROR;
                printf("error\n");
                return Status;
            }
            else {
                Status = (int)err;
            }
        }

        if (Status == 0) {
            i += 1;
            lfile.write(varBuffer, MAX_VAR_LENGTH + 1);
            // fprintf(fp, "Name: %s\t Roll no: %d \tMarks: %f \n", name, roll_no, marks);

        }
        else if (i == 0) {
            printf("file can't be opened\n");
            Status = CONS_ERROR;
            return Status;
        }
        else {
           printf("finished reading %d variables\n", i);
        }

        if (varBuffer) {
            ZeroMemory(varBuffer, MAX_VAR_LENGTH);
        }
    }

    free(varBuffer);

    Status = SUCCESS;
    return Status;
}

int main(int argc, char** argv)
{
    fstream logfile;
    char* argFilename;
    const char* newRawFilename = ".\\new_raw_logfile.bin";
    int Status = 0;

    Status = ElevateCurrentPrivileges();
    if (Status !=0) {
        cout << "Failed to elevate privileges, errno:" << Status << endl;
		return Status;
    }

    //
    // Create or open binary log file
    //
    if (argc < 2) {
        // create temporary file
        // FILE* pFile = tmpfile();
        logfile.open(newRawFilename, ios::out);
        if (!logfile.is_open()) {
            cout << "new raw logfile can't be opened\n";
            Status = CONS_ERROR;
            return Status;
        }

        Status = ReadLogFromUefiInterface(logfile);
        if (Status != SUCCESS) {
			cerr << "Error reading log, exiting\n";
			return CONS_ERROR;
		}
        else {
            cout << "Raw binary file created\n";
        }

        logfile.close();
        if (logfile.is_open()) {
			cout << "file is still open after trying to close\n";
            return CONS_ERROR;
        }

        logfile.open(newRawFilename, ios::in | ios::binary);
    }
    else {
        argFilename = argv[1];
        logfile.open(argFilename, ios::in | ios::binary);
    }

    if (!logfile.is_open()) {
        cerr << "Failed to open raw log file: " << endl;
        return CONS_ERROR;
    }

    logfile.close();
    return SUCCESS;
}
