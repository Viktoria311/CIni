#include "CIni.h"
#include "Funcs.h"
#include <iostream>
#include <string>

int main()
{
	// 1) a variant. создаем ini. Инициализируем
	std::cout << "Enter a full path of directory where you want to add ini file: ";
	std::string full_path;
	std::getline(std::cin, full_path); // enter like "D:\Kotova_projects\CIni\CIni"
	CIni ini;
	ini.SetFullFilePath(full_path + "\\test.ini");

	// ИЛИ
	// 1) b variant. можно ввести готовый путь до файла
	//CIni ini("E:\\GIT\\CIni\\test.ini");


	
	// можно пройтись по мапе отладчиком
	if (!ini.IsEmpty())
	{
		std::cout << "Sections Count in new.ini : " 
				  << ini.SectionsCount() 
				  << std::endl << std::endl;

		ini.Print();
	}

	std::cout << std::endl;

	// 2) запись в классы CIni новых секций с полями
	WriteValue(ini);

	// 3) запись в ini файлы
	ini.WriteToIni();

	// 4) проверка CIni классов
	CheckSection(ini);

	// 5) записываем новую секцию с ключами и значениями сразу в файл
	SavePassingMap(ini);


	// 6)  у одной из подсекций добавляем и меняем пару ключ-значение
	ChangeValue(ini);

	return 0;
}
