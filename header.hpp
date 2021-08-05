//----------------------------------------------------
//		MoveMyFiles v. 1.1b
//		© BullDogoff 2021
//		HEADER_HPP
//----------------------------------------------------

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>

#pragma once

#ifndef MOVE_MY_FILES
#define MOVE_MY_FILES

#define OUTPUT_DIR "output"
//#define DEBUG

namespace fs = std::filesystem;

bool enumerateDirectoryEntries(const fs::path, std::map<std::string, fs::directory_entry>&);
bool parceFile(fs::path, std::vector<std::string>&);
bool digitlessFilename(const fs::path);
std::string parceFilename(std::string);
bool moveFiles(fs::path, std::vector<std::string>&);


//	Enumerating directory entryes
//	Excluding all but regular files, put them into map<entry, filename>
//	Find first .txt file, mark as numbers source

bool enumerateDirectoryEntries(const fs::path path_, std::map<std::string, fs::directory_entry>& entryMap_)
{
	//	Check every directory entry
	for (auto& p : fs::directory_iterator(path_))
	{
		//	If regular file
		if (fs::status(p.path()).type() == fs::file_type::regular)
		{
			// If text file
			if (p.path().extension() == ".txt" && digitlessFilename(p.path()) && !entryMap_.contains("txt"))
			{
				//	Mark it as numbers source
				entryMap_.insert(std::pair<std::string, fs::directory_entry>("txt", p));
				std::cout << "\nFound '" << fs::relative(p.path()).string() << "' file, marked as numbers source" << std::endl;
			}
			else
				//	Else add it to map of all files in directory
				entryMap_.insert(std::pair<std::string, fs::directory_entry>(fs::relative(p.path()).string(), p));
		}
	}

	if (entryMap_.empty())
	{
		std::cout << "No regular files found, enumerating failed" << std::endl;
		return false;
	}

#ifdef DEBUG

	std::cout << "\nDEBUG\nContents of entryMap_:\n";
	for (auto& it : entryMap_)
	{
		std::cout << it.first << " ---> " << fs::relative(it.second) << "\n";
	}
	std::cout << "\n//DEBUG\n" << std::endl;

#endif // DEBUG


	return true;
}


//	Parcing numbers from txt file
//	Excluding all characters but digit
//	Any numeric sequence consider as number, write into string vector

bool parceFile(fs::path path_, std::vector<std::string>& strings_)
{
	std::string buf_;
	std::vector<char> buffer_(0);

	//	Prepare output vector
	strings_.erase(strings_.begin(), strings_.end());

	//	Check if txt file exists
	if (fs::status(path_).type() == fs::file_type::not_found)
	{
		std::cout << "\nCan't find " << fs::relative(path_).string() << " file, parcing terminated" << std::endl;
		return false;
	}

	std::ifstream file_(path_);
	//	Check if txt file opened
	if (!file_.is_open())
	{
		std::cout << "\nCan't open " << fs::relative(path_).string() << " file, parcing terminated" << std::endl;
		return false;
	}

	//	Count length of txt file
	size_t length_ = 0;
	file_.seekg(0, std::ios::end);
	length_ = file_.tellg();
	file_.seekg(0, std::ios::beg);

	//	Read txt file to char vector
	buffer_.resize(length_ + 1);
	file_.read(buffer_.data(), length_);

#ifdef DEBUG

	std::cout << "\nDEBUG\nContents of '" << fs::relative(path_).string() << "':\n\n" << buffer_.data() << std::endl;
	std::cout << "\nTotal " << length_ << " characters in '" << fs::relative(path_).string() << "' file" << std::endl;
	std::cout << "\n//DEBUG\n" << std::endl;

#endif // DEBUG

	//	Parcing char vector into string vector
	for (auto& it : buffer_)
	{
		//	If symbol is digit
		if (std::isdigit(it))
			buf_.push_back(it);
		//	If symbol is not digit
		else
		{
			if (buf_ != "")
			{
				strings_.push_back(buf_);
				buf_.erase();
			}
		}
	}
	strings_.shrink_to_fit();

#ifdef DEBUG

	std::cout << "\nDEBUG\nContents of 'strings_' std::vector:\n\n";
	for (auto& it : strings_)
		std::cout << it.data() << std::endl;
	std::cout << "\nTotal " << strings_.size() << " numbers in 'strings_' std::vector" << std::endl;
	std::cout << "\n//DEBUG\n" << std::endl;

#endif // DEBUG

	return true;
}


//	Parcing filename
//	Return true if there is no decimal digits in it

bool digitlessFilename(const fs::path path_)
{
	for (auto& it : fs::relative(path_).string())
	{
		if (std::isdigit(it))
		{
			return false;
		}
	}

	return true;
}


//	Parcing first numeric sequence from filename

std::string parceFilename(std::string filename_)
{
	std::string parcedFilename_;

	bool startFlag = false;
	bool endFlag = false;

	for (auto& it : filename_)
	{
		if (!startFlag && !endFlag && isdigit(it))
			startFlag = true;

		if (startFlag && !endFlag && isdigit(it))
			parcedFilename_.push_back(it);

		if (startFlag && !endFlag && !isdigit(it))
			endFlag = true;
	}

	return parcedFilename_;
}


//	Search for files, containing sought numbers in filename
//	Copy them into subdirectory
//	Delete them from original directory

bool moveFiles(fs::path path_, std::vector<std::string>& strings_)
{
	std::error_code ec;

	//	Path for directory for processed files
	fs::path processedPath_ = fs::path(path_ / OUTPUT_DIR);

	//	Check if directory exists
	if (fs::status(processedPath_).type() == fs::file_type::not_found)
		fs::create_directory(processedPath_);

	size_t total_ = 0;
	size_t count_ = 0;

	std::cout << "\nCheck filenames if there are matches with 'strings_' std::vector:\n";

	//	Check filenames if there are matches with strings_ vector (surprisingly >_>)
	for (auto& n : strings_)
	{
		for (auto& p : fs::directory_iterator(path_))
		{
			//	Using .compare() method to find exact match
			if (parceFilename(fs::relative(p).string()).compare(n) == 0)
			{
				
				total_ += 1;
				std::cout << "Found match: " << fs::relative(p).string() << " and " << n << " ---> ";
				
				//	Check if there is enough free space
				if (p.file_size() < fs::space(path_.root_directory()).available)
				{
					if (!fs::copy_file(p, processedPath_ / fs::relative(p), fs::copy_options::skip_existing, ec))
					{
						std::cout << "\nUnable to move file '" << fs::relative(p).string() << "', error code " << ec.value() << std::endl;
					}
					else
					{

#ifndef DEBUG
					
						fs::remove(p);

#endif //DEBUG

						std::cout << "MOVED" << std::endl;
						count_ += 1;
					}
				}
				else
				{
					std::cout << "\n\nNot enough space at '" << path_.root_directory().string() << "', copying terminated\n" << std::endl;
				}
			}
		}
	}

	if (total_ > 0)
		std::cout << "Total moved " << count_ << " of " << total_ << " files\n" << std::endl;
	else
		std::cout << "No matches found\n" << std::endl;

	return true;
}


#endif // !MOVE_MY_FILES
