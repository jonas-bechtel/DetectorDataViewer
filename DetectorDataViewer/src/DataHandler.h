#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "implot.h"

struct DataArray
{
    std::vector<double> data;
    std::string name;
    int plot = 0;
};

struct DataSegment
{
	double startTime;
	double endTime;

	std::string startDate;
	std::string endDate;

	std::vector<double> times;
    std::vector<DataArray> dataArrays;

	int size() { return times.size(); }

	std::vector<double>& operator[](int i)
	{
        return dataArrays[i].data;
	}
};

class DataHandler
{
public:
	bool HasData();
	void ClearData() { dataSegments.clear(); }
	std::vector<DataSegment>& GetData() { return dataSegments; }
	DataSegment& GetData(int i) { return dataSegments[i]; }
	int Size() { return dataSegments.size(); }
	double GetSmallestTime() { return dataSegments[0].startTime; }
	double GetLargestTime() { return dataSegments.back().endTime; }

	std::filesystem::path SelectFile(const std::filesystem::path& startPath, const std::vector<const char*>& filterPatterns);
	
	void LoadData(std::string filePath);

	void PlotDataSegmentVoltages(int i);
	void PlotDataSegmentCurrents(int i);

	void ShowWindow();

private:
	void ProcessLine(std::string& line);
	std::string SecondsToDate(double seconds);

private:
	std::vector<DataSegment> dataSegments;
	DataSegment currentSegment;

	ImPlotRect plotLims;
};

