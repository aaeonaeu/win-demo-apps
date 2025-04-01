// UPWDT.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <atomic>
#include <string>
#include "stdafx.h"
#include "aaeonEAPI.h"

void start(int t) {
    EApiLibInitialize();
    if (t > 1024 || t<1)
    {
        std::cout << "Current watchdog timeout support 1-1024 second" << std::endl;

    }
    else
    {
        EApiWDTStart(t);
        std::cout << "Current watchdog start ,set timeout :" << t << std::endl;

    }

}

void stop() {
    EApiLibInitialize();
    EApiWDTStop();
    std::cout << "Current watchdog stop" << std::endl;

}

void reload() {
    EApiLibInitialize();
    EApiWDTReload();
    std::cout << "Current watchdog reload" << std::endl;

}

void getTimeout() {
    EApiLibInitialize();
    uint32_t t;
    EApiWDTGet(&t);
    std::cout << "Current watchdog timeout: " << t << " seconds" << std::endl;
}
void showHelp() {
    std::cout << "Usage: <command> [timeout]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  start <timeout>  - Start the watchdog with the specified timeout in seconds" << std::endl;
    std::cout << "  stop             - Stop the watchdog" << std::endl;
    std::cout << "  reload           - Reload the watchdog" << std::endl;
    std::cout << "  gettimeout       - Get the current watchdog timeout" << std::endl;
    std::cout << "  help             - Show this help message" << std::endl;
}

int main(int argc, char* argv[])
{

    if (argc < 2) {
        showHelp();
        return 1;
    }

    std::string command = argv[1];

    if (command == "start" && argc == 3) {
        int t = std::stoi(argv[2]);
        start(t);
    }
    else if (command == "stop") {
        stop();
    }
    else if (command == "reload") {
        reload();
    }
    else if (command == "gettimeout") {
        getTimeout();
    }
    else {
        std::cerr << "Invalid command or missing timeout value" << std::endl;
        return 1;
    }

    return 0;

}
