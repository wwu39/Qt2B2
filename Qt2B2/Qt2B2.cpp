// Qt2B2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <direct.h>
#include <functional>

// Commands
#define BGM
#define BGS
#define FADEBGM
#define FADESE
#define BG
#define SE
#define UNLOCKMUSIC
#define UNLOCKCG
#define BRANCH
#define IMAGE
#define GAME
#define AUTOSAVE
#define WAIT
#define CAMERA

namespace fs = std::filesystem;
using namespace std;

static bool IsDebug = true;

static ofstream logfile;

string path = IsDebug ? "C:\\Users\\24233\\Desktop\\286studio\\Ballade1Qt-master\\Ballade1\\script\\chs" : ".";
string output_path = path + "\\OUTPUT";

hash<string> hasher;

bool isSqrPrent(char c) { return c == '[' || c == ']'; }

string ftos(float f)
{
    string ret = to_string(f);
    ret.erase(ret.find_last_not_of('0') + 1, string::npos);
    if (ret[ret.size() - 1] == '.') {
        ret.erase(ret.end() - 1);
    }
    return ret;
}

string GetFilenameFromPath(string path)
{
    const size_t last_slash_idx = path.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        path.erase(0, last_slash_idx + 1);
    }
    return path;
}

vector<string> GetParams(string line)
{
    size_t lpi = -1, rpi = -1;
    vector<int> cis;
    for (size_t i = 0; i < line.length(); ++i) {
        if (line[i] == '(') lpi = i;
        else if (line[i] == ')') rpi = i;
        else if (line[i] == ',') cis.push_back(i);
    }
    if (cis.size() == 0)
        return { line.substr(lpi + 1, rpi - lpi - 1) };
    else {
        vector<string> ret;
        for (size_t i = 0; i <= cis.size(); ++i) {
            size_t si = i == 0 ? lpi : cis[i - 1];
            size_t ei = i == cis.size() ? rpi : cis[i];
            ret.push_back(line.substr(si + 1, ei - si - 1));
        }
        return ret;
    }
}

string SetParams(string line, vector<string> params)
{
    string ret = line.substr(0, line.find_first_of('(') + 1);
    auto size = params.size();
    for (size_t i = 0; i < size; ++i) ret += params[i] + (i == size - 1 ? ')' : ',');
    return ret;
}

void SingleQuote(string& line) {
    for (size_t i = 0; i < line.length(); ++i) {
        if (line[i] == '\"') {
            line[i] = '\'';
        }
    }
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
            auto commandStartIdx2 = line.find_first_of('+');
            auto semicolonIdx = line.find_first_of(';');
            if (commandStartIdx != string::npos && semicolonIdx < commandStartIdx) continue;
            if (commandStartIdx2 != string::npos && semicolonIdx < commandStartIdx2) continue;
            int firstNotSpace = line.find_first_not_of(' ');
#ifdef BGM
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
#endif // @bgm

#ifdef BGS
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
#endif // @bgs

#ifdef FADEBGM
            // 如果参数2>1，参数2/1000
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 9) == "@fadebgm(")
            {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                auto params = GetParams(line);
                if (params.size() > 1) {
                    int spi = stoi(params[1]);
                    if (spi > 1) {
                        params[1] = to_string(spi / 1000.0);
                        line2 = SetParams(line, params);
                    }
                }
            }

#endif // @fadebgm

#ifdef FADESE
            // 如果参数2>1，参数2/1000
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 8) == "@fadese(")
            {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                auto params = GetParams(line);
                if (params.size() > 1) {
                    int spi = stoi(params[1]);
                    if (spi > 1) {
                        params[1] = to_string(spi / 1000.0);
                        line2 = SetParams(line, params);
                    }
                }
            }
#endif

#ifdef BG
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 4) == "@bg(") {
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
                    string secondPara = line2.substr(firstCommaIdx + 1, firstRightPrenIdx - firstCommaIdx - 1);
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
#endif

#ifdef SE
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 4) == "@se(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                for (size_t i = 0; i < line2.length(); ++i) {
                    if (line2[i] == '\"') {
                        line2[i] = '\'';
                    }
                }
                int firstCommaIdx = line2.find_first_of(',');
                int firstLeftPrenIdx = line2.find_first_of('(');
                if (firstCommaIdx != string::npos && firstLeftPrenIdx != string::npos) {
                    string firstPara = line2.substr(firstLeftPrenIdx + 1, firstCommaIdx - firstLeftPrenIdx - 1);
                    if (firstPara == "\'\'" || firstPara == "\"\"") {
                        int firstRightPrenIdx = line2.find_first_of(')');
                        if (firstCommaIdx != string::npos && firstRightPrenIdx != string::npos)
                        {
                            string secondPara = line2.substr(firstCommaIdx + 1, firstRightPrenIdx - firstCommaIdx - 1);
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
                }
            }
#endif

#ifdef UNLOCKMUSIC
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 13) == "@unlockMusic(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = SetParams(line, { GetParams(line)[0] });
                SingleQuote(line2);
            }
#endif

#ifdef UNLOCKCG
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 10) == "@unlockCg(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = SetParams(line, { GetParams(line)[0] });
                SingleQuote(line2);
            }
#endif

#ifdef BRANCH
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 13) == "@clearBranch(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = ";" + line;
            }
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 14) == "@showBranches(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = line + "\n@r=waitBranch()";
            }
#endif

#ifdef IMAGE
            if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 13) == "+image.group(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                int z = stoi(GetParams(line)[0]);
                z += 10;
                //line2 = SetParams("@image()", { to_string(z), "\'none.png\'" });
                line2 = ";" +line;
            }
            if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 13) == "+image.image(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                auto params = GetParams(line);
                int z = stoi(params[0]);
                z += 10;
                string newParam = "{type=\'image\',name=\'z" + to_string(z) + "\',path=" + params[1] + ",anchor=\'0.23,0.35\',z=" + to_string(z) + ",scale=" + line.substr(line.find_last_of('=') + 1) +'}';
                line2 = SetParams("@img.new()", { newParam });
            }
            if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 11) == "+image.add(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                string temp = line.substr(commandStartIdx2 + 22);
                int z1 = stoi(temp.substr(0, temp.find_first_of(')')));
                temp = temp.substr(temp.find_first_of('('));
                int z2 = stoi(temp.substr(1, temp.find_first_of(')') - 1));
                //z1 += 10;
                z1 = 0;
                z2 += 10;
                //line2 = SetParams("@setChild()", { to_string(z1), to_string(z2) });
                line2 = ";" + line;
            }
            if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 14) == "+image.setPos(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                auto params = GetParams(line);
                int z = stoi(params[0]);
                z += 10;
                float x = stof(params[1]);
                //x += 0.5;
                x *= 0.85;
                float y = stof(params[2]);
                //y += 0.5;
                y = -y;
                line2 = SetParams("@setPosition()", { to_string(z), ftos(x), ftos(y) });
            }
            if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 18) == "+image.setOpacity(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                auto params = GetParams(line);
                int z = stoi(params[0]);
                z += 10;
                line2 = SetParams("@setAlpha()", {to_string(z), params[1]});
            }
#endif

#ifdef GAME
            if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 15) == "+game.showPort=") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = "@game._showPort" + line.substr(line.find_last_of('='));
            }
            else if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 17) == "+game._skipRatio=") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = "@game._skipRatio" + line.substr(line.find_last_of('='));
            }
            else if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 9) == "+game.cc=") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = "@game.cc" + line.substr(line.find_last_of('='));
            }
            else if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 27) == "+game.r=Notification.result") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = ";" + line;
            }
            else if (commandStartIdx2 != string::npos && line.substr(commandStartIdx2, 6) == "+game.") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = "@game" + line.substr(line.find_first_of('.'));
            }
#endif

#ifdef AUTOSAVE
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 10) == "@autoSave(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line2 = ";" + line;
            }
#endif

#ifdef WAIT
            if (firstNotSpace < line.length() && line[firstNotSpace] == '=') {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                string p = line.substr(firstNotSpace + 1);
                int spi = stoi(p);
                if (spi > 1) {
                    p = to_string(spi / 1000.0);
                    line2 = (firstNotSpace == 0 ? "" : line.substr(0, firstNotSpace)) + "@waitTime(" + p + ")";
                }
            }
#endif

#ifdef CAMERA
            if (commandStartIdx != string::npos && line.substr(commandStartIdx, 8) == "@camera(") {
                logfile << LineNumToString(linum) << " CHECKING: " << line << endl;
                nochange = false;
                line.erase(remove_if(line.begin(), line.end(), isSqrPrent), line.end());
                auto params = GetParams(line);
                string newParam;
                int z = stoi(params[0]);
                if (z > 4) z += 10;
                float time = stoi(params[3]);
                time /= 1000;
                char easing;
                if (params[4] == "Easing.Linear") 
                {
                    easing = '=';
                }
                else if (params[4] == "Easing.OutSine")
                {
                    easing = '>';
                }
                if (params[2] == "\'x\'")
                {
                    string endX = params[5].substr(0, params[5].find_first_of('*'));
                    float x = stof(endX);
                    //x += 0.5;
                    x *= 0.85;
                    newParam = "\'m" + ftos(time) + easing + ftos(x) + ",_\'";
                }
                else if (params[2] == "\'y\'")
                {
                    string endY = params[5].substr(0, params[5].find_first_of('*'));
                    float y = stof(endY);
                    //y += 0.5;
                    y = -y;
                    newParam = "\'m" + ftos(time) + easing + "_," + ftos(y) + '\'';
                }
                else if (params[2] == "\'opacity\'")
                {
                    string X = "";
                    if (params[5] == "0") X = " x";
                    newParam = "\'a" + ftos(time) + easing + params[5] + X + '\'';
                }
                line2 = (firstNotSpace == 0 ? "" : line.substr(0, firstNotSpace)) + SetParams("@camera()", {
                    (z == 4 ? "ManualFixNeeded" : to_string(z)),
                    to_string(hasher(params[1])),
                    newParam
                    });
            }
#endif

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
