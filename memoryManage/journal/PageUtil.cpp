#include "PageUtil.h"
#include "PageHeader.h"

#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <sstream>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include <iostream>

USING_MM_NAMESPACE

string PageUtil::GenPageFileName(const string& jname, short pageNum)
{
	return JOURNAL_PREFIX + "." + jname + "." + std::to_string(pageNum) + "." + JOURNAL_SUFFIX;
}

string PageUtil::GenPageFullPath(const string& dir, const string& jname, short pageNum)
{
	std::stringstream ss;
	ss << dir << "/" << GenPageFileName(jname, pageNum);
	return ss.str();
}

string PageUtil::GetPageFileNamePattern(const string& jname)
{
	return JOURNAL_PREFIX + "\\." + jname + "\\.[0-9]+\\." + JOURNAL_SUFFIX;
}

short PageUtil::ExtractPageNum(const string&filename, const string& jname)
{
	string numStr = filename.substr(JOURNAL_PREFIX.length() + jname.length() + 2, filename.length() - JOURNAL_SUFFIX.length());
	return atoi(numStr.c_str());
}

vector<short> PageUtil::GetPageNums(const string& dir, const string& jname)
{
	string namePattern = GetPageFileNamePattern(jname);
	boost::filesystem::path p(dir);
	boost::regex pattern(namePattern);

	vector<short> res;

	for (auto &file : boost::filesystem::directory_iterator(p))
	{
		string filename = file.path().filename().string();
		if (boost::regex_match(filename.begin(),filename.end(),pattern))
		{
			res.push_back(ExtractPageNum(filename,jname));
		}
	}
	std::sort(res.begin(),res.end());

	return res;
}

short PageUtil::GetPageNumWithTime(const string& dir, const string&jname, long time)
{
	vector<short> pageNums = GetPageNums(dir, jname);

	for (int idx = pageNums.size() - 1; idx >= 0; idx--)
	{
		PageHeader header = GetPageHeader(dir, jname, pageNums[idx]);
		if (header.start_nano < time)
		{
			return pageNums[idx];
		}
	}
	return 1;
}

PageHeader PageUtil::GetPageHeader(const string&dir, const string&jname, short pageNum)
{
	PageHeader header;
	string path = PageUtil::GenPageFullPath(dir, jname, pageNum);
	FILE* pfile = fopen(path.c_str(), "r");
	if (pfile == nullptr)
	{
		perror("cannot open file in PageUtil::GetPageNumWithTime");
	}

	size_t length = fread(&header, 1, sizeof(PageHeader), pfile);
	if (length != sizeof(PageHeader))
	{
		perror("cannot get page header in PageUtil::GetPageNumWithTime");
	}
	fclose(pfile);
	return header;
}

void* PageUtil::LoadPageBuffer(const string& path, int size, bool isWriting, bool quickMode)
{
	int fd = open(path.c_str(), (isWriting) ? (O_RDWR | O_CREAT) : O_RDONLY, (mode_t)0600);

	if (fd < 0)
	{
		std::cout << "fd is " << fd << std::endl;
		if (!isWriting)
			return nullptr;
		string errorMsg = "Cannot create/write the file with path " + path;
		perror(errorMsg.c_str());
		exit(EXIT_FAILURE);
	}

	if (/*!quickMode &&*/ isWriting)
	{
		if (lseek(fd, size - 1, SEEK_SET) == -1)
		{
			close(fd);
			perror("Error calling lseek() to 'stretch' the file");
			exit(EXIT_FAILURE);
		}
		if (write(fd, "", 1) == -1)
		{
			close(fd);
			perror("Error writing last byte of the file");
			exit(EXIT_FAILURE);
		}
	}

	void* buffer = mmap(0, size, (isWriting) ? (PROT_READ | PROT_WRITE) : PROT_READ, MAP_SHARED, fd, 0);

	if (buffer == MAP_FAILED)
	{
		close(fd);
		perror("Error mapping file to buffer");
		exit(EXIT_FAILURE);
	}

	if (!quickMode && madvise(buffer, size, MADV_RANDOM) != 0 && mlock(buffer, size) != 0)
	{
		munmap(buffer, size);
		close(fd);
		perror("Error in madvise or mlock");
		exit(EXIT_FAILURE);
	}

	close(fd);
	return buffer;
}

void PageUtil::ReleasePageBuffer(void* buffer, int size, bool quickMode)
{
	//unlock and unmap
	if (!quickMode&&munlock(buffer, size) != 0)
	{
		perror("ERROR in munlock");
		exit(EXIT_FAILURE);
	}

	if (munmap(buffer, size) != 0)
	{
		perror("ERROR in munmap");
		exit(EXIT_FAILURE);
	}
}

bool PageUtil::FileExists(const string& filename)
{
	int fd=open(filename.c_str(), O_RDONLY, (mode_t)0600);
	if (fd >= 0)
	{
		close(fd);
		return true;
	}
	return false;
}
