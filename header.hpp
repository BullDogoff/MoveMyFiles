//----------------------------------------------------
//		MoveMyFiles v. 1.0b
//		© BullDogoff 2021
//		HEADER_HPP
//----------------------------------------------------

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>

#pragma once

#define OUTPUT_DIR "output"
#define DEBUG

namespace fs = std::filesystem;

bool parceFile(fs::path, std::vector<uint32_t>&);
bool parceFile(fs::path, std::vector<std::string>&);
std::string parceFilename(std::string);
bool moveFiles(fs::path, std::vector<std::string>&);

//	Parcing numbers from txt file
//	Excluding all characters but digits
//	Any numeric sequence consider as number, while below numeric_limits<>::max()

bool parceFile(fs::path path_, std::vector<uint32_t>& numbers_)
{
	char buf_;
	uint32_t number_;

	//	Prepare output vector
	numbers_.erase(numbers_.begin(), numbers_.end());

	//	Check if file exists
	if (fs::status(path_).type() == fs::file_type::not_found)
	{
		std::cout << "Can't find '" << fs::relative(path_).string() <<"', parcing terminated" << std::endl;
		return false;
	}

	std::ifstream file(path_);
	//	Check if file opene
	if (!file.is_open())
	{
		std::cout << "Can't open '" << fs::relative(path_).string() << "', parcing terminated" << std::endl;
		return false;
	}

	//	Find file length
	size_t length_ = 0;
	file.seekg(0, std::ios::end);
	length_ = file.tellg();
	file.seekg(0, std::ios::beg);

	//	Clear buffers
	number_ = 0;
	buf_ = 0;
	
	for (size_t i = 0; i < length_; i++)
	{
		//	Reading next character
		file.read(&buf_, std::streamsize(1));

		if (std::isdigit(buf_))
		{
			// Integer overflow check
			if (number_ < std::numeric_limits<uint32_t>::max() / 10 - 10)
			{
				switch (buf_)
				{
				case '1':
					number_ = number_ * 10 + 1;
					break;
				case '2':
					number_ = number_ * 10 + 2;
					break;
				case '3':
					number_ = number_ * 10 + 3;
					break;
				case '4':
					number_ = number_ * 10 + 4;
					break;
				case '5':
					number_ = number_ * 10 + 5;
					break;
				case '6':
					number_ = number_ * 10 + 6;
					break;
				case '7':
					number_ = number_ * 10 + 7;
					break;
				case '8':
					number_ = number_ * 10 + 8;
					break;
				case '9':
					number_ = number_ * 10 + 9;
					break;
				case '0':
					number_ = number_ * 10 + 0;
					break;
				default:
					break;
				}
			}
			//	/TEMPORARY!/ On integer overflow
			else
			{
				number_ = std::numeric_limits<uint32_t>::max();
			}
		}
		else
		{
			if (number_ != 0)
			{
				numbers_.push_back(number_);
				number_ = 0;
			}
		}
	}
	//	If last character is digit
	if (number_ != 0)
	{
		numbers_.push_back(number_);
		number_ = 0;
	}

#ifdef DEBUG

	for (auto& it : numbers_)
	{
		std::cout << it << "\n";
	}

#endif // DEBUG

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
		std::cout <<"\nCan't open " << fs::relative(path_).string() << " file, parcing terminated" << std::endl;
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

	std::cout << "\nContents of '" << fs::relative(path_).string() << "':\n\n" << buffer_.data() << std::endl;
	std::cout << "\nTotal " << length_ << " characters in '" << fs::relative(path_).string() << "' file" << std::endl;

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

	std::cout << "\nContents of 'strings_' std::vector:\n\n";
	for (auto& it : strings_)
		std::cout << it.data() << std::endl;
	std::cout << "\nTotal " << strings_.size() << " numbers in 'strings_' std::vector" << std::endl;

#endif // DEBUG

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
	//	Path for directory for processed files
	fs::path processedPath_ = fs::path(path_ / OUTPUT_DIR);

	//	Check if directory exists
	if (fs::status(processedPath_).type() == fs::file_type::not_found)
		fs::create_directory(processedPath_);

	size_t count_ = 0;

#ifdef DEBUG

	std::cout << "\nCheck filenames if there are matches with 'strings_' std::vector:\n" << std::endl;

#endif // DEBUG

	//	Check filenames if there are matches with strings_ vector (surprisingly >_>)
	for (auto& n : strings_)
	{
		for (auto& p : fs::directory_iterator(path_))
		{
			//	Using .compare() method to find exact match
			if (parceFilename(fs::relative(p).string()).compare(n) == 0)
			{
				count_ += 1;

				std::cout << "Found match: " << fs::relative(p).string() << " and " << n << "			";
				
				//	Check if there is enough free space
				if (p.file_size() < fs::space(path_.root_directory()).available)
				{
					fs::copy_file(p, processedPath_ / fs::relative(p));
					fs::remove(p);
					std::cout << "MOVED" << std::endl;
				}
				else
				{
					std::cout << "\n\nNot enough space at '" << path_.root_directory().string() << "', copying terminated\n" << std::endl;
					if (count_ > 0)
						std::cout << "Total moved " << count_ << " files\n" << std::endl;
					return false;
				}
			}
		}
	}

	if (count_ > 0)
		std::cout << "Total moved " << count_ << " files\n" << std::endl;
	else
		std::cout << "No matches found\n" << std::endl;

	return true;
}
