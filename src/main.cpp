#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <functional>

#include "VirtualDisk.h"


int mapCommandToInt(const std::string& command) {
    static const std::unordered_map<std::string, int> commandMap = {
        {"import", 1},
        {"export", 2},
        {"mkdir", 3},
        {"rmdir", 4},
        {"ln", 5},
        {"rm", 6},
        {"ls", 7},
        {"expand", 8},
        {"shrink", 9},
        {"info", 10}
    };

    auto it = commandMap.find(command);
    return (it != commandMap.end()) ? it->second : 0;
}


int main() {
    std::string diskName;
    VirtualDisk disk;

    std::cout << "Enter the name of the virtual disk: ";
    std::getline(std::cin, diskName);

    std::ifstream file(diskName);

    if (!file) {
        std::cout << "File does not exist. Do you want to create it? (yes/no): ";
        std::string response;
        std::cin >> response;
        std::cin.ignore();

        if (response == "yes") {
            std::cout << "Enter size of the disk in bytes: ";
            size_t size;
            std::cin >> size;
            std::cin.ignore();
            disk = VirtualDisk(size, diskName);
        }
        else {
            std::cout << "Exiting program." << std::endl;
            return 0;
        }
    }
    else {
        file.close();
        disk = VirtualDisk::loadFromFile(diskName);
    }

    std::string input;

    while (true) {
        try {
            std::cout << "> ";
            std::getline(std::cin, input);

            if (input == "exit") {
                std::cout << "Exiting..." << std::endl;
                disk.saveToFile();
                break;
            }

            std::istringstream iss(input);
            std::string command;
            std::vector<std::string> args;

            iss >> command;
            std::string arg;
            while (iss >> arg) {
                args.push_back(arg);
            }

            disk.saveToFile();

            switch (mapCommandToInt(command)) {
            case 1: {
                // import
                if (args.size() == 2) {
                    const std::string& sourceName = args[0];
                    const std::string& targetName = args[1];
                    disk.copyFromSystemDrive(sourceName, targetName);
                }
                else {
                    std::cout << "Error - usage: import <sourceName> <targetName>" <<
                        std::endl;
                }
                break;
            }
            case 2: {
                // export
                if (args.size() >= 2) {
                    const std::string& sourceName = args[0];
                    const std::string& targetName = args[1];
                    disk.copyToSystemDrive(sourceName, targetName);
                }
                else {
                    std::cout << "Error - usage: export <sourceName> <targetName>" <<
                        std::endl;
                }
                break;
            }
            case 3: {
                // mkdir
                if (args.size() == 1) {
                    const std::string& name = args[0];
                    disk.makeDirectory(name);
                }
                else {
                    std::cout << "Error - usage: mkdir <name>" << std::endl;
                }
                break;
            }
            case 4: {
                // rmdir
                if (args.size() >= 1) {
                    std::string name = args[0];
                    // Handle "rmdir name"
                }
                else {
                    std::cout << "Error - usage: rmdir <name>" << std::endl;
                }
                break;
            }
            case 5: {
                // ln
                if (args.size() >= 1) {
                    std::string filename = args[0];
                    // Handle "ln filename"
                }
                else {
                    std::cout << "Error - usage: ln <filename>" << std::endl;
                }
                break;
            }
            case 6: {
                // rm
                if (args.size() >= 1) {
                    std::string filename = args[0];
                    // Handle "rm filename"
                }
                else {
                    std::cout << "Error - usage: rm <filename>" << std::endl;
                }
                break;
            }
            case 7: {
                // ls
                // Handle "ls"
                break;
            }
            case 8: {
                // expand
                if (args.size() >= 2) {
                    std::string filename = args[0];
                    std::string extraSize = args[1];
                    // Handle "expand filename extraSize"
                }
                else {
                    std::cout << "Error - usage: expand <filename> <extraSize>" << std::endl;
                }
                break;
            }
            case 9: {
                // shrink
                if (args.size() >= 2) {
                    std::string filename = args[0];
                    std::string sizeToReduce = args[1];
                    // Handle "shrink filename sizeToReduce"
                }
                else {
                    std::cout << "Error - usage: shrink <filename> <sizeToReduce>" <<
                        std::endl;
                }
                break;
            }
            case 10: {
                // info
                // Handle "info"
                break;
            }
            default:
                std::cout << "Unknown command: " << command << std::endl;
                break;
            }
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            disk.saveToFile();
        }
    }
    return 0;
}
