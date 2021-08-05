//----------------------------------------------------
//		MoveMyFiles v. 1.1b
//		© BullDogoff 2021
//		MAIN_CPP
//----------------------------------------------------

#include "header.hpp"

int main()
{
	std::error_code ec_;
	std::cout << "MoveMyFiles application version 1.1b, copyright BullDogoff, 2021" << std::endl;

#ifdef DEBUG
	
	std::cout << "\nDEBUG\nProgram is running in DEBUG mode, copying files instead of moving" << std::endl;

#endif // DEBUG


	fs::path path_(fs::current_path());
	std::map<std::string, fs::directory_entry> fileMap_;
	std::vector<std::string> strings_;

	if (!enumerateDirectoryEntries(path_, fileMap_))
	{
		std::cout << "Enumerator error, program terminated" << std::endl;
		return -1;
	}
	
	if (!parceFile(fileMap_.at("txt"), strings_))
	{
		std::cout << "Parcer error, program terminated" << std::endl;
		return -2;
	}

	if (!moveFiles(path_, strings_))
	{
		std::cout << "Mover error, program terminated" << std::endl;
		return -3;
	}

#ifdef DEBUG

	std::cout << "\n//DEBUG" << std::endl;

#endif // DEBUG


	system("pause");

	return 0;
}
