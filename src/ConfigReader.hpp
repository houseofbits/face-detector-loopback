#include <stdio.h>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;

class ConfigReader
{
protected:
    map<string, string> values;

public:
    void read(string filename)
    {
        ifstream input(filename);

        for (string line; getline(input, line);)
        {
            vector<string> keyValuePair = split(line, ':');
            if (keyValuePair.size() == 2)
            {
                trim(keyValuePair[0]);
                trim(keyValuePair[1]);
                if (!keyValuePair[0].empty() && !keyValuePair[1].empty())
                {
                    values[keyValuePair[0]] = keyValuePair[1];
                }
            }
        }
    }
    int getIntValue(string key, int defaultValue = 0)
    {
        if (valueExists(key))
        {
            return stoi(values[key]);
        }

        cout << key << " setting not found, using default: " << defaultValue << endl;

        return defaultValue;
    }
    float getFloatValue(string key, float defaultValue = 0)
    {
        if (valueExists(key))
        {
            return stof(values[key]);
        }

        cout << key << " setting not found, using default: " << defaultValue << endl;

        return defaultValue;
    }
    bool getBoolValue(string key, bool defaultValue = false)
    {
        if (valueExists(key))
        {
            return (bool)stoi(values[key]);
        }

        cout << key << " setting not found, using default: " << defaultValue << endl;

        return defaultValue;
    }
    string getStringValue(string key, string defaultValue = "")
    {
        if (valueExists(key))
        {
            return values[key];
        }

        cout << key << " setting not found, using default: " << defaultValue << endl;

        return defaultValue;
    }
    cv::Size2f getSizeValue(string key, cv::Size2f defaultValue = {0, 0})
    {
        if (valueExists(key))
        {
            vector<string> parts = split(values[key], ',');
            if (parts.size() == 2)
            {
                cv::Size2f size;
                trim(parts[0]);
                trim(parts[1]);
                size.width = stof(parts[0]);
                size.height = stof(parts[1]);

                return size;
            }
        }

        cout << key << " setting not found, using default" << endl;

        return defaultValue;
    }
    cv::Size2i getIntSizeValue(string key, cv::Size2i defaultValue = {0, 0})
    {
        cv::Size2f sizef = getSizeValue(key, cv::Size2f(defaultValue.width, defaultValue.height));

        return cv::Size2i((int)sizef.width, (int)sizef.height);
    }

private:
    bool valueExists(string key)
    {
        return values.find(key) != values.end();
    }

    vector<string> split(string str, char seperator)
    {
        vector<string> output;

        string::size_type prev_pos = 0, pos = 0;

        while ((pos = str.find(seperator, pos)) != string::npos)
        {
            string substring(str.substr(prev_pos, pos - prev_pos));

            output.push_back(substring);

            prev_pos = ++pos;
        }

        output.push_back(str.substr(prev_pos, pos - prev_pos));

        return output;
    }

    void ltrim(string &s)
    {
        s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch)
                                   { return !isspace(ch); }));
    }

    void rtrim(string &s)
    {
        s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                        { return !isspace(ch); })
                    .base(),
                s.end());
    }

    void trim(string &s)
    {
        ltrim(s);
        rtrim(s);
    }
};