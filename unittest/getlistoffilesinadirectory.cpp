#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string.h>
#include <iostream>

using namespace std;

/*function... might want it in some class?*/
int getdir (string dir, vector<string> &files)
{
	DIR *dp;
	struct dirent *dirp;
	if((dp = opendir(dir.c_str())) == NULL)
	{
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		files.push_back(string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}

int getListOfExistedModels (vector<string> &files, vector<int> &existedModelIds)
{
	string preFix = "agentReport_";
	string postFix = ".csv";
	int modelId;
	int totalModels = 0;

	char prestr[20], poststr[20];

	for (unsigned int i = 0;i < files.size();i++)
	{
		if(!strncmp(files[i].c_str(), preFix.c_str(), preFix.size()) // Starts with
				&&
				!strncmp(files[i].c_str() + (files[i].size() - postFix.size()), postFix.c_str(), postFix.size())) // Ends with
		{
			int ret = sscanf(files[i].c_str(), "%*[^_]%*c%d%*c%*s",&modelId);
			existedModelIds.push_back(modelId);
			totalModels++;
			cout << files[i] << " " << modelId << endl;
		}
	}
	return totalModels;
}

int main()
{
	string dir = string("log/20180520_123411");
	vector<string> files = vector<string>();

	getdir(dir,files);

	for (unsigned int i = 0;i < files.size();i++)
	{
		cout << files[i] << endl;
	}

	cout << endl;

	vector<int> existedModelIds;

	getListOfExistedModels(files, existedModelIds);


	cout << endl;
	for (unsigned int i = 0;i < existedModelIds.size();i++)
	{
		cout << existedModelIds[i] << endl;
	}

	return 0;
}
