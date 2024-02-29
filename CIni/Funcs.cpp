#include "Funcs.h"
#include <iostream>


void 
WriteValue(CIni& ini)
{
	// запись в мапу, использу€ функцию со строкой, где секци€ и ключ раздельно
	ini.WriteValue("MainDlg", "bool", true);
	ini.WriteValue("MainDlg", "char", 'f');
	ini.WriteValue("MainDlg", "unsigned_char", 15);
	ini.WriteValue("grid", "int", 94000);
	ini.WriteValue("grid\\collumns", "double", 4.2);
	ini.WriteValue("grid\\collumns", "string", std::string("lalala"));

	// запись в мапу, использу€ функцию со строкой, где секци€ и ключ слитно
	ini.WriteValue("SectionKeyTogether\\bool", true);
	ini.WriteValue("SectionKeyTogether\\char", 'f');
	ini.WriteValue("SectionKeyTogether\\unsigned_char", 15);
	ini.WriteValue("SectionKeyTogether\\int", 7800);
	ini.WriteValue("SectionKeyTogether\\double", 4.2);
	ini.WriteValue("SectionKeyTogether\\string", std::string("lalala"));
}

void 
CheckSection(const CIni& ini)
{
	// дл€ проверки выбрана секци€, в которой есть подсекци€
	auto section_grid = ini.FindSection("grid");

	if (section_grid != nullptr)
	{
		std::cout << std::endl;
		section_grid->Print();
	}
}

void 
SavePassingMap(const CIni& ini)
{
	// секци€ и ключ отдельно
	ini.SaveValue("PassingMap", "bool", true);
	ini.SaveValue("PassingMap", "char", 'f');
	ini.SaveValue("PassingMap", "unsigned_char", 15);
	ini.SaveValue("PassingMap", "int", 15000);
	ini.SaveValue("PassingMap", "double", 4.2);
	ini.SaveValue("PassingMap", "string", std::string("lalala"));

	// секци€ и ключ вместе
	ini.SaveValue("PassingMap\\other_bool", false);
	ini.SaveValue("PassingMap\\other_char",'f');
	ini.SaveValue("PassingMap\\other_unsigned_char", 30);
	ini.SaveValue("PassingMap\\other_int", -15000);
	ini.SaveValue("PassingMap\\other_double", 4.2);
	ini.SaveValue("PassingMap\\other_string", std::string("zzz..."));
}

void 
ChangeValue(CIni& ini)
{
	auto* m_ptrsection = ini.FindSection("grid");

	if (m_ptrsection != nullptr)
	{
		std::cout << std::endl 
				  << "Section [grid] ContainsSubSection  \"collumns\" ? " 
				  << (m_ptrsection->ContainsSubSection("collumns") ? "true" : "false") 
				  << std::endl;


		auto* m_ptr_chibi_section = m_ptrsection->FindSection("collumns");

		if (m_ptr_chibi_section != nullptr)
		{
			std::cout << std::endl 
					  << "In [grid\\collumns] were : "
					  << std::endl;

			m_ptr_chibi_section->Print();

			// новое значение
			m_ptr_chibi_section->WriteValue("new_key", 8.5);

			std::cout << std::endl
				<< "After adding \"new_key\" = 8.5" << std::endl 
				<< "In [grid\\collumns] are : "
				<< std::endl;

			m_ptr_chibi_section->Print();

			// мен€ем значение по ключу
			std::cout << std::endl
					  << "Change value from key \"ney_key\" "
					  << std::endl;

			m_ptr_chibi_section->WriteValue("new_key", static_cast<const char*>("ooooops"));
			m_ptr_chibi_section->Print();

			// мен€ем обратно
			std::cout << std::endl
					  << "Change BACK value from key \"ney_key\" "
					  << std::endl;

			m_ptr_chibi_section->WriteValue("new_key", 8.5);
			m_ptr_chibi_section->Print();

		}
		
	}

	// use CIni::Section::WriteValue(2 args)
	m_ptrsection = ini.FindSection("MainDlg");

	if (m_ptrsection != nullptr)
	{
		std::cout << std::endl
			<< "In [MainDlg] were : "
			<< std::endl;
		m_ptrsection->Print();


		m_ptrsection->WriteValue("new_key", static_cast<const char*>("ooooops"));

		std::cout << std::endl
			<< "After adding \"new_key\" = \"ooooops\" " << std::endl
			<< "In [MainDlg] are : "
			<< std::endl;
		m_ptrsection->Print();
	}

	// use CIni::WriteValue(3 args)
	m_ptrsection = ini.FindSection("PassingMap");

	if (m_ptrsection != nullptr)
	{
		std::cout << std::endl
			<< "In [PassingMap] were : "
			<< std::endl;
		m_ptrsection->Print();

		ini.WriteValue("PassingMap", "new_key", static_cast<const char*>("ooooops"));
		

		std::cout << std::endl
			<< "After adding \"new_key\" = \"ooooops\" " << std::endl
			<< "In [PassingMap] are : "
			<< std::endl;
		m_ptrsection->Print();
	}

	// use CIni::WriteValue(2 args)
	m_ptrsection = ini.FindSection("SectionKeyTogether");

	if (m_ptrsection != nullptr)
	{
		std::cout << std::endl
			<< "In [SectionKeyTogether] were : "
			<< std::endl;
		m_ptrsection->Print();

		ini.WriteValue("SectionKeyTogether\\new_key", static_cast<const char*>("ooooops"));


		std::cout << std::endl
			<< "After adding \"new_key\" = \"ooooops\" " << std::endl
			<< "In [SectionKeyTogether] are : "
			<< std::endl;
		m_ptrsection->Print();
	}
	
	
}