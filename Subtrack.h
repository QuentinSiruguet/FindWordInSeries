#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <thread>
#include <iterator>
#include <algorithm>
#include <fstream>

#include <json/json.h>

class Subtrack
{
private:
	std::vector<std::filesystem::path> allPathsToExtract;

	std::string MkvExtractPath;
	std::string MkvInfoPath;
	std::string FFMPEGPath;
	std::string SubsPath;
	std::string VideosPath;
	std::string PathToExtract;
	std::string SeriesName;
	std::string WordToFind;
	std::string Arrow = "-->";
	std::string FromPreviousLine;
	std::string VideoLength;

public:
	Subtrack(std::vector<std::string> cfgdatas);
	void define_series_name();

	void extract_subs();
	bool already_extracted_subs(std::string name);
	void find_word_in_subs(std::string filename, int episode);
	void extract_video_from_file(int episode, int counter, std::string timing);

	std::string get_filename(std::string filename);
	int get_engsubs_id(std::filesystem::path path);
	std::string get_series_names();

	void set_AllPathsToExtract(std::vector<std::filesystem::path> temp);
};

