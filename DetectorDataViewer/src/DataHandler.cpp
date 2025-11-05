#include "DataHandler.h"
#include "implot.h"

#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <string>
#include <locale>
#include <codecvt>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>


#include "tinyfiledialogs.h"

bool DataHandler::HasData()
{
    return !dataSegments.empty();
}

std::filesystem::path DataHandler::SelectFile(const std::filesystem::path& startPath, const std::vector<const char*>& filterPatterns)
{
    //const char* filterPatterns[] = { "*.asc" };
    const char* filePath = tinyfd_openFileDialog(
        "Choose a file",               // Dialog title
        startPath.string().c_str(),    // Default path or file
        1,                             // Number of filters
        filterPatterns.data(),                // Filter patterns (NULL for any file type)
        NULL,                          // Filter description (optional)
        0                              // Allow multiple selection (0 = false)
    );
    if (!filePath)
    {
        return std::filesystem::path();
    }
    return std::filesystem::path(filePath);
}

void DataHandler::LoadData(std::string filePath)
{
    std::ifstream inputFile(filePath, std::ios::binary | std::ios::ate);

    // Check if the file is successfully opened
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    std::streamsize size = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (inputFile.read(buffer.data(), size))
    {
        std::string str(buffer.data());
        std::string line;
        
        int linenumber = 0;

        int start = 0;
        int end = 0;
        while ((end = str.find('\n', start)) != std::string::npos)
        {
            std::string line = str.substr(start, end - start);

            // Process the line as needed
            ProcessLine(line);

            //data.push_back(line);
            //linenumber++;
            //if (linenumber % 100000 == 0) {
            //    std::cout << linenumber << "\n";
            //}

            start = end + 1;
        }
        //std::cout << "\n" << data[0] << "\n";
        //std::cout << data[1] << "\n";
        //std::cout << data[data.size() - 2] << "\n";
        //std::cout << data.size() << "\n";
        //std::cout << "toll\n";

        inputFile.close();
    }
}

void DataHandler::ProcessLine(std::string& line)
{
    size_t firstDigitPos = line.find_first_of("0123456789");

    // line with numbers
    if (firstDigitPos == 0) 
    {
        //printf("number row\n");
        std::istringstream ss(line);
        std::string part;

        std::getline(ss, part, ',');
        currentSegment.times.push_back(currentSegment.startTime + std::stod(part));

        for (int i = 0; std::getline(ss, part, ','); i++)
        {
            float value = std::stod(part);
            currentSegment[i].push_back(value);
        }
        return;
    }
    else if (line.find("started") != std::string::npos)
    {
        //printf("start row\n");
        currentSegment = DataSegment();

        auto firstDigitPos = line.find_first_of("0123456789");

        if (firstDigitPos != std::string::npos) {
            // Extracting date and time starting from the first digit
            std::tm tp = {};
            std::istringstream ss(line.substr(firstDigitPos));
            ss >> std::get_time(&tp, "%d-%m-%Y at %H:%M:%S");

            //std::cout << ss.str() << " " << tp.time_since_epoch().count() << "\n";

            //auto date = std::chrono::floor<std::chrono::days>(tp);
            //std::time_t date_time_t = std::chrono::system_clock::to_time_t(date);
            //std::cout << "Date: " << std::put_time(std::localtime(&date_time_t), "%d-%m-%Y") << std::endl;

            currentSegment.startTime = mktime(&tp);
            currentSegment.startDate = SecondsToDate(currentSegment.startTime);
        }
        return;
    }
    else if (line.find("stopped") != std::string::npos)
    {
        //printf("stop row\n");
        auto firstDigitPos = line.find_first_of("0123456789");

        if (firstDigitPos != std::string::npos) {

            std::tm tp = {};
            std::istringstream ss(line.substr(firstDigitPos));
            ss >> std::get_time(&tp, "%d-%m-%Y at %H:%M:%S");

            currentSegment.endTime = mktime(&tp);
            currentSegment.endDate = SecondsToDate(currentSegment.endTime);
        }

        dataSegments.push_back(currentSegment);
        return;
    }
    // line with labels
    else
    {
        std::istringstream ss(line);
        std::string part;

        for (int i = 0; std::getline(ss, part, ','); )
        {
            if (part.find("Time") != std::string::npos)
                continue;

            //std::cout << part << std::endl;
            currentSegment.dataArrays.emplace_back();
            currentSegment.dataArrays[i].name = part;
            if (part.find("voltage") != std::string::npos || part.find(" U") != std::string::npos)
            {
                currentSegment.dataArrays[i].plot = 1;
            }
            if (part.find("current") != std::string::npos || part.find(" I") != std::string::npos)
            {
                currentSegment.dataArrays[i].plot = 2;
            }
            i++;
        }
        return;
    }
}

std::string DataHandler::SecondsToDate(double seconds)
{
    time_t sec = seconds;
    struct tm* timeinfo = std::gmtime(&sec);

    // Format the date string
    char buffer[50]; // Adjust the size as needed
    std::strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", timeinfo);

    return std::string(buffer);
}

void DataHandler::PlotDataSegmentVoltages(int i)
{
    if (i >= dataSegments.size())
    {
        std::cerr << "Index too large" << std::endl;
        return;
    }
    if (ImPlot::GetPlotLimits().X.Min > dataSegments[i].endTime || ImPlot::GetPlotLimits().X.Max < dataSegments[i].startTime)
    {
        return;
    }
    
    double frameSpan = ImPlot::GetPlotLimits().X.Size();
    float dataInFrameRatio = (dataSegments[i].endTime - dataSegments[i].startTime) / frameSpan;
    dataInFrameRatio = min(1, dataInFrameRatio);
    dataInFrameRatio = max(0.2, dataInFrameRatio);

    // maybe adjust downsample factor
    int downsample = (int)frameSpan / (5000 * dataInFrameRatio + 1) + 1;
    int start = (int)(ImPlot::GetPlotLimits().X.Min - dataSegments[i].startTime);
    start = start < 0 ? 0 : start > dataSegments[i].size() - 1 ? dataSegments[i].size() - 1 : start;
    int end = (int)(ImPlot::GetPlotLimits().X.Max - dataSegments[i].startTime) + 1000;
    end = end < 0 ? 0 : end > dataSegments[i].size() - 1 ? dataSegments[i].size() - 1 : end;
    int size = (end - start) / downsample;
    //std::cout << start << "\n";
   
    for (const DataArray& da : dataSegments[i].dataArrays)
    {
        if (da.plot == 1 && !dataSegments[i].times.empty())
        {
            ImPlot::PlotLine(da.name.c_str(), &dataSegments[i].times.data()[start], &da.data[start], size, 0, 0, sizeof(double) * downsample);
        }
    }
}

void DataHandler::PlotDataSegmentCurrents(int i)
{
    if (i >= dataSegments.size())
    {
        std::cerr << "Index too large" << std::endl;
        return;
    }
    if (ImPlot::GetPlotLimits().X.Min > dataSegments[i].endTime || ImPlot::GetPlotLimits().X.Max < dataSegments[i].startTime)
    {
        return;
    }

    double frameSpan = ImPlot::GetPlotLimits().X.Size();
    float dataInFrameRatio = (dataSegments[i].endTime - dataSegments[i].startTime) / frameSpan;
    dataInFrameRatio = min(1, dataInFrameRatio);
    dataInFrameRatio = max(0.2, dataInFrameRatio);

    // maybe adjust downsample factor
    int downsample = (int)frameSpan / (5000 * dataInFrameRatio + 1) + 1;
    int start = (int)(ImPlot::GetPlotLimits().X.Min - dataSegments[i].startTime);
    start = start < 0 ? 0 : start > dataSegments[i].size() - 1 ? dataSegments[i].size() - 1 : start;
    int end = (int)(ImPlot::GetPlotLimits().X.Max - dataSegments[i].startTime) + 1000;
    end = end < 0 ? 0 : end > dataSegments[i].size() - 1 ? dataSegments[i].size() - 1 : end;
    int size = (end - start) / downsample;

    for (const DataArray& da : dataSegments[i].dataArrays)
    {
        if (da.plot == 2 && !dataSegments[i].times.empty())
        {
            ImPlot::PlotLine(da.name.c_str(), &dataSegments[i].times.data()[start], &da.data[start], size, 0, 0, sizeof(double) * downsample);
        }
    }
}





