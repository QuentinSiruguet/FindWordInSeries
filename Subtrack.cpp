#include "Subtrack.h"

Subtrack::Subtrack(std::vector<std::string> cfgdatas)
{
	MkvExtractPath = cfgdatas[0];
	MkvInfoPath = cfgdatas[1];
	FFMPEGPath = cfgdatas[2];
	SubsPath = cfgdatas[3];
	VideosPath = cfgdatas[4];
	PathToExtract = cfgdatas[5];
	define_series_name();
	WordToFind = cfgdatas[6];
	for (int i = 0; i < WordToFind.size(); i++)
		WordToFind[i] = std::tolower(WordToFind[i]);
	FromPreviousLine = cfgdatas[7];
	VideoLength = cfgdatas[8];
}

void Subtrack::define_series_name()
{
	int pos_last_slash = 0;
	for (int i = 0; i < PathToExtract.size(); i++)
	{
		if (PathToExtract[i] == '\\')
			pos_last_slash = i;
	}
	SeriesName = PathToExtract.substr(pos_last_slash, std::string::npos);
}

void Subtrack::extract_subs()
{
	int episode = 1;
	for (auto& s : allPathsToExtract)
	{
		int id = get_engsubs_id(s);
		if (id != -1)
		{
			std::stringstream command;
			std::stringstream name;
			name << SubsPath << "" << SeriesName << "" << get_filename(s.string()) << "-track_" << id << ".srt";
			command << MkvExtractPath << " " << s << " tracks " << id << ":" << "\"" << name.str() << "\"";
			if (!already_extracted_subs(name.str()))
			{
				std::cout << command.str() << std::endl;
				system(command.str().c_str());
			}
			find_word_in_subs(name.str(), episode);
		}
		episode++;
	}

}

bool Subtrack::already_extracted_subs(std::string name)
{
	int pos_last_slash = 0;
	for (int i = 0; i < name.size(); i++)
		if (name[i] == '\\')
			pos_last_slash = i;
	name = name.substr(pos_last_slash + 1, std::string::npos);

	bool result = false;
	std::stringstream ss;
	ss << SubsPath << "\\" << SeriesName << "\\extractedsubs.txt";
	std::ifstream extractedsubs(ss.str());
	std::string actual_line;
	while (std::getline(extractedsubs, actual_line))
	{
		if (actual_line == name)
		{
			result = true;
			break;
		}
	}
	extractedsubs.close();
	if (result) return result;
	else
	{
		std::fstream extractedsubs(ss.str(), std::ios::out | std::ios::app);
		extractedsubs << name << std::endl;
		extractedsubs.close();
	}
	return result;
}

void Subtrack::find_word_in_subs(std::string filename, int episode)
{
	std::ifstream subfile(filename);
	std::string actual_line;

	std::string previous_timing;
	std::string timing;

	int count = 0;
	while (std::getline(subfile, actual_line))
	{
		for (int i = 0; i < actual_line.size(); i++)
		{
			actual_line[i] = std::tolower(actual_line[i]);
		}

		if (actual_line.find(Arrow) != std::string::npos)
		{
			previous_timing = timing;
			timing = actual_line;
		}
		else if (actual_line.find(WordToFind) != std::string::npos)
		{
			count++;
			if (FromPreviousLine == "NO")
				extract_video_from_file(episode, count, timing);
			else
				extract_video_from_file(episode, count, previous_timing);
		}
	}
}

void Subtrack::extract_video_from_file(int episode, int counter, std::string timing)
{
	std::stringstream command;
	std::string timing1 = timing.substr(0, 12);
	timing1[8] = '.';
	timing1[9] = '0';
	timing1[10] = '0';
	timing1[11] = '0';
	//std::string timing2 = timing.substr(17, 12);
	//timing2[8] = '.';
	command << FFMPEGPath << " -hwaccel cuda -i " << allPathsToExtract[episode - 1] << " -ss " << timing1 << " -vcodec libx264 -crf 27 -preset ultrafast -c:a copy -map 0:v -map 0:m:language:eng -map -0:s -t " << VideoLength << " \"" /* << timing2 << " "*/ << VideosPath << "\\" << SeriesName << "\\" << WordToFind << "\\" << get_filename(allPathsToExtract[episode - 1].string()) << "-";
	timing1[2] = 'h';
	timing1[5] = 'm';
	command << timing1.substr(0, 8) << "s.mp4\"";
	std::cout << command.str() << std::endl;
	system(command.str().c_str());
}

std::string Subtrack::get_filename(std::string filename)
{
	int count = 0;
	std::string result;

	for (int i = 0; i < filename.size(); i++)
	{
		if (filename[i] == '\\')
			count = i;
	}

	result = filename.substr(count, filename.size() - count - 4);
	return result;
}

int Subtrack::get_engsubs_id(std::filesystem::path path)
{
	std::stringstream ss;
	ss << this->MkvInfoPath << " -J " << path << " >Ressources/temp/temp.json";
	system(ss.str().c_str());

	std::ifstream JsonFile("Ressources/temp/temp.json", std::ifstream::binary);
	Json::Value JsonValues;

	JsonFile >> JsonValues;

	const Json::Value& tracks = JsonValues["tracks"];

	for (int i = 0; i < tracks.size(); i++)
		if (tracks[i]["type"] == "subtitles" && tracks[i]["properties"]["language"] == "eng")
			return tracks[i]["id"].asInt();
	return -1;
}

std::string Subtrack::get_series_names()
{
	return SeriesName;
}

void Subtrack::set_AllPathsToExtract(std::vector<std::filesystem::path> temp)
{
	for (auto& t : temp)
	{
		allPathsToExtract.push_back(t);
	}
}
