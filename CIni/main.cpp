#include "CIni.h"
#include "Funcs.h"
#include <iostream>
#include <string>

int main()
{
	// 1) a variant. ������� ini. ��������������
	std::cout << "Enter a full path of directory where you want to add ini file: ";
	std::string full_path;
	std::getline(std::cin, full_path); // enter like "D:\Kotova_projects\CIni\CIni"
	CIni ini;
	ini.SetFullFilePath(full_path + "\\test.ini");

	// ���
	// 1) b variant. ����� ������ ������� ���� �� �����
	//CIni ini("E:\\GIT\\CIni\\test.ini");


	
	// ����� �������� �� ���� ����������
	if (!ini.IsEmpty())
	{
		std::cout << "Sections Count in new.ini : " 
				  << ini.SectionsCount() 
				  << std::endl << std::endl;

		ini.Print();
	}

	std::cout << std::endl;

	// 2) ������ � ������ CIni ����� ������ � ������
	WriteValue(ini);

	// 3) ������ � ini �����
	ini.WriteToIni();

	// 4) �������� CIni �������
	CheckSection(ini);

	// 5) ���������� ����� ������ � ������� � ���������� ����� � ����
	SavePassingMap(ini);


	// 6)  � ����� �� ��������� ��������� � ������ ���� ����-��������
	ChangeValue(ini);

	return 0;
}
