//
// Created by DarsenOP on 2/8/25.
//

#include "Logging/Log.h"
#include "Manager/Manager.h"

int main(const int argc, const char* argv[])
{
    Log::Init();

    if (argc == 1) {
        Manager::GetInstance().Start();
        return 0;
    } else {
        Log::LOG(LogLevel::ERROR, "Invalid Command: To enter the File Manager, type: [filemanager] and press enter.");
    }
}