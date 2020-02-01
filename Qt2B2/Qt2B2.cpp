// Qt2B2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <direct.h>

namespace fs = std::filesystem;
using namespace std;

static bool IsDebug = true;

static ofstream logfile;

string path = IsDebug ? "D:\\286studio\\Ballade1Qt-master\\Ballade1\\script\\chs" : ".";
string output_path = path + "\\OUTPUT";

string GetFilenameFromPath(string path)
{
    const size_t last_slash_idx = path.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        path.erase(0, last_slash_idx + 1);
    }
    return path;
}

bool IsB2ScriptFile(string path)
{
    auto dotPos = path.find_last_of('.');
    if (dotPos != string::npos) {
        return path.substr(dotPos) == ".script";
    }
    return false;
}

string Trim(string s) {
    s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
    return s;
}

string LineNumToString(int linum)
{
    if (linum < 10) return "000" + to_string(linum);
    else if (linum >= 10 && linum < 100) return "00" + to_string(linum);
    else if (linum >= 100 && linum < 1000) return "0" + to_string(linum);
    else return to_string(linum);
}

void processSingleFile(string path)
{
    logfile << "Processing file " << path << "..." << endl;
    ifstream myfile;
    ofstream myoutputfile;
    string line;
    myfile.open(path);
    myoutputfile.open(output_path + "\\" + GetFilenameFromPath(path));
    if (myfile.is_open())
    {
        int linum = 1;
        while (getline(myfile, line))
        {
            bool nochange = true;
            string line2 = line;
            auto commandStartIdx = line.find_first_of('@');
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 4) == "@bgm")
            {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;

                // replace " with '
                for (size_t i = 0; i < line2.length(); ++i) {
                    if (line2[i] == '\"') {
                        line2[i] = '\'';
                    }
                }

                // if 2nd param of @bgm > 1, divide by 1000
                int firstCommaIdx = line2.find_first_of(',');
                int firstRightPrenIdx = line2.find_first_of(')');
                if (firstCommaIdx != string::npos && firstRightPrenIdx != string::npos)
                {
                    string secondPara = line2.substr(firstCommaIdx + 1, firstRightPrenIdx - firstCommaIdx -1);
                    int spi = stoi(secondPara);
                    string st = line2.substr(0, firstCommaIdx + 1);
                    if (spi > 1)
                    {
                        line2 = st + to_string(spi / 1000.0) + ")";
                    }
                    else
                    {
                        line2 = st + Trim(secondPara) + ")";
                    }
                }
            }

            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 4) == "@bgs")
            {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;

                // replace " with '
                for (size_t i = 0; i < line2.length(); ++i) {
                    if (line2[i] == '\"') {
                        line2[i] = '\'';
                    }
                }
            }
            if (nochange) {
                myoutputfile << line << endl;
            }
            else {
                myoutputfile << line2 << endl;
                logfile << LineNumToString(linum) << " REPLACED: " << line2 << endl;
            }
            ++linum;
        }
        myfile.close();
    }
    myoutputfile.close();
}

int main()
{
    if (_mkdir(output_path.c_str()))
        cout << "Create folder " + output_path << endl;
    else
        cout << "Can't create folder " + output_path << endl;

    logfile.open(output_path + "\\log.txt");

    for (const auto& entry : std::filesystem::directory_iterator(path)) 
    {
        if (IsB2ScriptFile(entry.path().string()))
        {
            processSingleFile(entry.path().string());
            logfile << endl;
        }
    }

    logfile.close();
}
