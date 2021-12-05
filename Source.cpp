#include "Subtrack.h"

std::vector<std::string> cfgdatas;
std::vector<std::filesystem::path> allPathsToExtract;

void get_config()
{
	std::ifstream readcfg("Ressources/config.cfg");

	for (int i = 0; i < 9; i++)
	{
		std::string actual_line;
		std::getline(readcfg, actual_line);
		cfgdatas.push_back(actual_line);
	}
	readcfg.close();
}

void create_folders(std::string seriesname)
{
	std::stringstream ss;
	ss << cfgdatas[4] << "\\" << seriesname;

	if (!std::filesystem::exists(ss.str()))
		std::filesystem::create_directories(ss.str());

	cfgdatas[6][0] = std::toupper(cfgdatas[6][0]);

	ss << "\\" << cfgdatas[6];

	if (!std::filesystem::exists(ss.str()))
		std::filesystem::create_directories(ss.str());

}

void get_all_paths_to_extract()
{

	if (std::filesystem::is_directory(cfgdatas[5]))
		for (const auto& entry : std::filesystem::directory_iterator(cfgdatas[5]))
			if (std::filesystem::is_directory(entry.path()))
				for (const auto& entrys : std::filesystem::directory_iterator(entry.path()))
					allPathsToExtract.push_back(entrys.path());
			else
				allPathsToExtract.push_back(entry.path());
	else
		allPathsToExtract.push_back(cfgdatas[5]);
}

int main() {


	get_config();
	Subtrack subs(cfgdatas);
	create_folders(subs.get_series_names());

	get_all_paths_to_extract();

	subs.set_AllPathsToExtract(allPathsToExtract);
	subs.extract_subs();


	return 1;
}