/**
 * @file
 *
 * @author OmniBlade
 *
 * @brief Setup the crash handling functions to generate crash reports.
 *
 * @copyright Thyme is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#include "crashpadhandler.h"

#ifdef CRASHPAD_ENABLED

#include <string>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <chrono>

#include <client/crash_report_database.h>
#include <client/crashpad_client.h>
#include <client/settings.h>

#include "exceptionhandler.h"
#include "vinifera_gitinfo.h"
#include "tspp_gitinfo.h"

extern int Execute_Day;
extern int Execute_Month;
extern int Execute_Year;
extern int Execute_Hour;
extern int Execute_Min;
extern int Execute_Sec;

bool IsCrashpadEnabled = false;
bool IsCrashpadUploadsAllowed = true;
Wstring CrashpadUploadURL = "";

#define UNIQUE_HASH_LENGTH 16


/**
 *  Generate a unique random number for idenitfying the upload data.
 */
static std::string Generate_Unique_Hash()
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    /**
     *  Create a unique filename for the crash dump based on the time of execution.
     */
    char time_buffer[512];
    std::snprintf(time_buffer, sizeof(time_buffer), "%02u-%02u-%04u_%02u-%02u-%02u",
        Execute_Day, Execute_Month, Execute_Year, Execute_Hour, Execute_Min, Execute_Sec);

    return str.substr(0, UNIQUE_HASH_LENGTH) + "-" + time_buffer; // Assumes LENGTH < number of characters in str.
}


/**
 *  x
 */
LONG Vinifera_Crashpad_Handler(unsigned int e_code, struct _EXCEPTION_POINTERS *e_info)
{
    /**
     *  Dump the exception information to file.
     */
    LONG retval = Vinifera_Exception_Handler(e_code, e_info);
    
    /**
     *  x
     */
    if (IsCrashpadEnabled) {
        crashpad::CrashpadClient::DumpAndCrash(e_info);
    }

    return retval;
}


/**
 *  x
 */
bool Crashpad_Setup()
{
    std::wstring homedir;
    std::wstring handler_path = L"ViniferaCrashHandler.exe";

    homedir += _wgetenv(L"USERPROFILE");
    homedir += L"\\Documents\\Vinifera Data\\CrashLogs";
    
    /**
     *  Cache directory that will store crashpad information and minidumps.
     */
    base::FilePath database(homedir);
    
    /**
     *  Cache directory that will store metrics information.
     */
    base::FilePath metrics(homedir);
    
    /**
     *  Path to the out-of-process handler executable.
     */
    base::FilePath handler(handler_path);
    
    /**
     *  URL used to submit minidumps to.
     */
    std::string url = CrashpadUploadURL.Peek_Buffer();

    /**
     *  Optional annotations passed via "--annotations" to the handler.
     */
    std::map<std::string, std::string> annotations = {
        { "unique_hash", Generate_Unique_Hash() },
        { "vinifera_commit", Vinifera_Git_Hash_Short() },
        { "vinifera_branch", Vinifera_Git_Branch() },
        { "vinifera_author", Vinifera_Git_Author() },
        { "tspp_commit", TSPP_Git_Hash_Short() },
        { "tspp_author", TSPP_Git_Author() }
    };
    
    /**
     *  Optional arguments to pass to the handler.
     */
    std::vector<std::string> arguments;
    
    /**
     *  Optional attachments to pass to the handler.
     */
    std::vector<base::FilePath> attachments;
    attachments.push_back(base::FilePath(L"GAME.EXE"));
    attachments.push_back(base::FilePath(L"Vinifera.dll"));
    attachments.push_back(base::FilePath(L"Vinifera.pdb"));

    std::unique_ptr<crashpad::CrashReportDatabase> db = crashpad::CrashReportDatabase::Initialize(database);

    if (db != nullptr && db->GetSettings() != nullptr) {
        db->GetSettings()->SetUploadsEnabled(IsCrashpadUploadsAllowed);
    }

    /**
     *  x
     */
    crashpad::CrashpadClient client;
    bool success = client.StartHandler(handler,
                                       database,
                                       metrics,
                                       url,
                                       annotations,
                                       arguments,
                                       /* restartable */ true,
                                       /* asynchronous_start */ false,
                                       attachments);

    return success;
}

#endif
