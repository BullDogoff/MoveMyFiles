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

//		Парсим из текста числа
//		Любые символы, кроме цифр, отбрасываем
//		Любую последовательность цифр считаем числом, пока не упрёмся в numeric_limits<>::max()

bool parceFile(fs::path path_, std::vector<uint32_t>& numbers_)
{
	numbers_.erase(numbers_.begin(), numbers_.end());						//		Подготовка массива чисел

	char buf_;																//		Сюда читаем очередной байт из файла
	uint32_t number_;														//		Сюда записываем очередное число

	if (fs::status(path_).type() == fs::file_type::not_found)				//		Проверка на существование
	{
		std::cout << "Can't find 'numbers.txt', program terminated" << std::endl;
		return false;
	}

	std::ifstream file(path_);
	if (!file.is_open())													//		Проверка на открытие
		return false;

	if (file.is_open())
	{
		size_t length_ = 0;													//		Найдём длину файла
		file.seekg(0, std::ios::end);
		length_ = file.tellg();
		file.seekg(0, std::ios::beg);

		number_ = 0;														//		Очистим буферы
		buf_ = 0;

		for (size_t it = 0; it < length_; it++)
		{
			file.read(&buf_, std::streamsize(1));							//		Читаем очередной символ

			if (std::isdigit(buf_))											//		Если символ - цифра
			{
				if (number_ < std::numeric_limits<uint32_t>::max() / 10)	//		Проверка на целочисленное переполнение
				{
					switch (buf_)											//		Добавляем очередной младший разряд
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
				else
				{
					number_ = std::numeric_limits<uint32_t>::max();			//		/ВРЕМЕННО!/ При целочисленном переполнении присваиваем максимальное значение
				}
			}
			else															//		Если символ не цифра
			{
				if (number_ != 0)											//		Если в буфере что-то есть
				{
					numbers_.push_back(number_);							//		Записываем буфер в массив
					number_ = 0;											//		Обнуляем буфер
				}
			}
		}
		if (number_ != 0)													//		Заглушка на случай, если последний байт в файле - цифра
		{
			numbers_.push_back(number_);
			number_ = 0;
		}

		for (auto& it : numbers_)
		{
			std::cout << it << "\n";
		}
	}
}

//		Парсим из текста числа
//		Любые символы, кроме цифр, отбрасываем
//		Любую последовательность цифр считаем числом, записываем в строку

bool parceFile(fs::path path_, std::vector<std::string>& strings_)
{
	std::vector<char> buffer_(0);

	//		Prepare output vector
	strings_.erase(strings_.begin(), strings_.end());

	//		Check if txt file exists
	if (fs::status(path_).type() == fs::file_type::not_found)
	{
		std::cout << "\nCan't find " << path_ << " file, parcing terminated" << std::endl;
		return false;
	}
		
	std::ifstream file_(path_);
	//		Check if txt file opened
	if (!file_.is_open())
	{
		std::cout <<"\nCan't open " << path_ << " file, parcing terminated" << std::endl;
		return false;
	}

	//		Count length of txt file
	size_t length_ = 0;
	file_.seekg(0, std::ios::end);
	length_ = file_.tellg();
	file_.seekg(0, std::ios::beg);

	//		Read txt file to char vector
	buffer_.resize(length_ + 1);
	file_.read(buffer_.data(), length_);

#ifdef DEBUG

	std::cout << "\nContents of '" << fs::relative(path_).string() << "':\n\n" << buffer_.data() << std::endl;
	std::cout << "\nTotal " << length_ << " characters in '" << fs::relative(path_).string() << "' file" << std::endl;

#endif // DEBUG

	//		Parcing char vector into string vector
	std::string buf_;
	for (auto& it : buffer_)
	{
		//		If symbol is digit
		if (std::isdigit(it))
			buf_.push_back(it);
		//		If symbol is not digit
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

//		Парсим из имени файла первую последовательность цифр
//		Возвращаем строку с номером

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

//		Ищем в директории назначения файлы, содержащие в названии числа из numbers
//		Комипуем их в поддиректорию '\processed'
//		Удаляем их из исходной директории

bool moveFiles(fs::path path_, std::vector<std::string>& strings_)
{
	//		Path for directory for processed files
	fs::path processedPath_ = fs::path(path_ / OUTPUT_DIR);

	//		Check if directory exists, then delete all
	if (fs::status(processedPath_).type() == fs::file_type::not_found)
		//		Create directory
		fs::create_directory(processedPath_);

	size_t count_ = 0;

#ifdef DEBUG

	std::cout << "\nCheck filenames if there are matches with 'strings_' std::vector:\n" << std::endl;

#endif // DEBUG


	for (auto& n : strings_)
	{
		for (auto& p : fs::directory_iterator(path_))
		{
			if (parceFilename(fs::relative(p).string()).compare(n) == 0)
			{
#ifdef DEBUG

				std::cout << "Found match: " << fs::relative(p).string() << " and " << n << std::endl;

#endif // DEBUG
				//fs::copy_file(p, processedPath_ / fs::relative(p));
				//fs::remove(p);
				count_ += 1;
			}
		}
	}

	if (count_ > 0)
		std::cout << "Succesfully moved " << count_ << " files\n" << std::endl;
	else
		std::cout << "No matches found\n" << std::endl;

	return true;
}
