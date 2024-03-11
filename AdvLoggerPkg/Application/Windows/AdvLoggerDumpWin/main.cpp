#include "pch.h"
#include "main.h"
#include <Windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>

using namespace winrt;
using namespace Windows::Foundation;
using namespace std;

//
// Create log file by retrieving AdvancedLogger variables from UEFI interface
//
int ReadLogFromUefiInterface(fstream& lfile)
{
    cout << "in ReadLogFromUefiInterface\n" << endl;

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

int ProcessMessages(fstream& logfile, ofstream& outfstream)
{
    int Status = SUCCESS;
    cout << "in ProcessMessages\n";
    //
    // Get logfile size
    //
    logfile.seekg(0, ios::beg);
    logfile.seekg(0, ios::end);
    size_t lfsize = logfile.tellg();
    logfile.seekg(0, ios::beg);

    if (lfsize == 0) {
        Status = FILE_ERROR;
        return Status;
    }

    //can i just convert the file to a char buffer?
    vector<unsigned char> buffer(std::istreambuf_iterator<char>(logfile), {});
    /*vector<char> logBuffer(lfsize);
    logfile.read(logBuffer.data(), lfsize);*/
    /* OR todo dynamically allocate output buffer
    vector<char> retBuffer(lfsize);*/

    // TODO process logBuffer into output buffer


    const char* constBuf = reinterpret_cast<const char*>(buffer.data());
    outfstream.write(constBuf, lfsize);
    if (outfstream.fail()) {
        cout << "failed to write to file\n";
		Status = FILE_ERROR;
	}   

    //   // print buffer to file
//   // todo get 
//   //outfstream.write(buffer.data(), lfsize);

	return Status;
}


int main(int argc, char** argv)
{

    // todo break out priviledge adjustment into a function
    fstream logfile;
    ofstream outfstream;
    char* filename = argv[1];
    const char* newRawFilename = "C:\\Users\\vnowkakeane\\Documents\\Scratch\\new_raw_logfile.bin";
    const char* newOutFilename = "C:\\Users\\vnowkakeane\\Documents\\Scratch\\new_parsed_logfile.txt";
    HANDLE ProcessHandle = GetCurrentProcess();
    DWORD DesiredAccess = TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY;
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

    //
    // Create or open log file
    //
    if (argc < 2) {
        // create
        cout << "call func with logfile\n";
        // create temporary file
        // FILE* pFile = tmpfile();
        logfile.open(newRawFilename, ios::out);
        if (!logfile.is_open()) {
            cout << "new logfile can't be opened\n";
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
        logfile.open(filename, ios::in | ios::binary);
    }

    if (!logfile.is_open()) {
        cerr << "Failed to open log file: " << filename << endl;
        return CONS_ERROR;
    }

    outfstream.open(newOutFilename, ios::out);
    if (!logfile.is_open()) {
        cout << "new logfile can't be opened\n";
        Status = CONS_ERROR;
        return Status;
    }


    //
    // Process Message
    //
    try {
        ProcessMessages(logfile, outfstream);
    }
    catch (const exception& e) {
		cerr << "Error: " << e.what() << endl;
		return CONS_ERROR;
	}

    logfile.close();
    outfstream.close();
    return SUCCESS;
}
