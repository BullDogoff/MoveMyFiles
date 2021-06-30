//----------------------------------------------------
//		MoveMyFiles v. 1.0b
//		© BullDogoff 2021
//		MAIN_CPP
//----------------------------------------------------

#include "header.hpp"

int main()
{
	std::error_code ec_;
	std::cout << "MoveMyFiles application version 1.0b, copyright BullDogoff, 2021" << std::endl;

#ifdef DEBUG
	
	std::cout << "Program is running in DEBUG mode" << std::endl;

#endif // DEBUG


	fs::path path_(fs::current_path());
	std::vector<uint32_t> numbers_;
	std::vector<std::string> strings_;

	if (!parceFile(path_ / "numbers.txt", strings_))
	{
		std::cout << "Parcer error, program terminated" << std::endl;
		return -1;
	}

	if (!moveFiles(path_, strings_))
	{
		std::cout << "Mover error, program terminated" << std::endl;
		return -1;
	}

	system("pause");

	return 0;
}
