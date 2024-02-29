#include "CIni.h"
#include <algorithm>
#include <any>
#include <cmath>
#include <io.h>
#include <iostream>
#include <regex>
#include <windows.h>



/////////////////////////////////////////////////////////////////////
// Функции класса Section

CIni::Section::Section(const std::string& section_path)
		: section_path(section_path)
{
	// из полного пути секции вычленяем path родительской секции

	std::istringstream f(section_path); // распарсим путь текущей секции
	std::string s;

	while (getline(f, s, '\\'))
		section_path_parts.push_back(s);


	int section_path_parts_size = section_path_parts.size();
	for (int i = 0; i < section_path_parts_size - 1; ++i)
	{
		if (i != 0)
			parent_section += '\\';

		parent_section += section_path_parts[i];
	}

}

CIni::Section::~Section()
{

}

bool
CIni::Section::ContainsKey(const std::string& key_name) const
{
	for (auto iter = key_value_map.cbegin();
		iter != key_value_map.cend(); ++iter)
	{

		if (iter->first == key_name)
			return true;
	}

	return false;
}

bool 
CIni::Section::ContainsSubSection(const std::string& section_name) const
{
	std::string full_section_name(section_path + "\\" + section_name);

	for (auto iter = sections_map.cbegin();
		iter != sections_map.cend(); ++iter)
	{

		if (iter->first == full_section_name)
			return true;
	}

	return false;
}

bool
CIni::Section::ContainsSubSections() const
{
	if (sections_map.empty())
		return false;
	return true;
}

CIni::Section*
CIni::Section::AddSection(const std::string& section_name)
{
	// конструируем полный путь до новой секции
	std::string full_section_path = section_path + '\\' + section_name;

	for (auto iter = sections_map.begin();
		iter != sections_map.end(); ++iter)
	{
		// если секция уже существует, возвращаем ее 
		if (iter->first == full_section_path)
			return &(iter->second);

	}

	// если такой секции нет, дошли до этого момента. создаем секцию
	sections_map.insert({ full_section_path, Section(full_section_path) });

	return FindSection(section_name);
}

bool
CIni::Section::WriteToIni(const std::string& m_full_file_path) const
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly(m_full_file_path))
	//	return false;
	//else
	{
		bool all_keys_result = true;

		for (auto iter = key_value_map.cbegin();
			iter != key_value_map.cend(); ++iter)
		{
			std::string str;

			switch (iter->second.first)
			{
			case CIni::VALUE_TYPE::BOOL:
			{
				bool bool_val = std::any_cast<bool>(iter->second.second);

				if (bool_val == true)
					str = "true";
				else
					str = "false";

				break;
			}
			case CIni::VALUE_TYPE::CHAR:
			{
				str = std::any_cast<char>(iter->second.second);
				break;
			}
			case CIni::VALUE_TYPE::UCHAR:
			{
				str = std::to_string(std::any_cast<unsigned char>(iter->second.second));
				break;
			}
			case CIni::VALUE_TYPE::INT:
			{
				str = std::to_string(std::any_cast<int>(iter->second.second));
				break;
			}
			case CIni::VALUE_TYPE::LONG:
			{
				str = std::to_string(std::any_cast<long>(iter->second.second));
				break;
			}
			case CIni::VALUE_TYPE::ULONG:
			{
				str = std::to_string(std::any_cast<unsigned long>(iter->second.second));
				break;
			}
			case CIni::VALUE_TYPE::DOUBLE:
			{
				str = std::to_string(std::any_cast<double>(iter->second.second));
				break;
			}
			case CIni::VALUE_TYPE::STRING:
			{
				str = std::any_cast<std::string>(iter->second.second);
				break;
			}
			}

			BOOL result = WritePrivateProfileString(
				section_path.c_str(),
				iter->first.c_str(),
				str.c_str(),
				m_full_file_path.c_str());

			if (all_keys_result != false)
				all_keys_result = result;
		}

		bool all_sections_result = true;

		for (auto iter = sections_map.cbegin();
			iter != sections_map.cend(); ++iter)
		{

			bool result = iter->second.WriteToIni(m_full_file_path);

			if (all_sections_result != false)
				all_sections_result = result;

		}

		return (all_sections_result && all_keys_result);
	}
}

bool
CIni::Section::HasEqualPathParts(const std::vector<std::string>& path_parts_1,
								const std::vector<std::string>& path_parts_2)
{
	int iteration_count = (path_parts_1.size() <= path_parts_2.size()) ?
							path_parts_1.size() : path_parts_2.size();

	for (int i = 0; i < iteration_count; ++i)
	{
		if (path_parts_1[i] != path_parts_2[i])
			return false;
	}
	return true;
}

const CIni::Section*
CIni::Section::FindSection(const std::vector<std::string>& section_path_parts) const
{
	// мы находимся в секции
	this->section_path;

	// если в данной секции нет подсекций
	if (sections_map.empty())
		return nullptr;
	else
	{
		// для сравнения размеров путей берем первую секцию в мапе. У другох секций размер пути будет одинаков
		int difference = section_path_parts.size() - sections_map.cbegin()->second.section_path_parts.size();

		// если размер у векторов, содержащих пути, совпадает
		if (difference == 0)
		{
			for (auto iter = sections_map.cbegin(); iter != sections_map.cend(); ++iter)
			{
				// либо найдем нужную секцию
				if (iter->second.section_path_parts == section_path_parts)
				{
					return &(iter->second);
				}
			}

			// либо искомой секции нет
			return nullptr;
		}
		else
		{
			for (auto iter = sections_map.cbegin(); iter != sections_map.cend(); ++iter)
			{
				// если с учетом разницы совпадают вектора
				if (HasEqualPathParts(iter->second.section_path_parts, section_path_parts))
				{
					return iter->second.FindSection(section_path_parts);
				}
			}

			// либо искомой секции нет
			return nullptr;
		}
	}
	
	// либо искомой секции нет
	return nullptr;
}

CIni::Section*
CIni::Section::FindSection(const std::vector<std::string>& section_path_parts)
{
	auto vec = this->section_path_parts;

	// для сравнения размеров путей берем первую секцию в мапе. У другох секций размер пути будет одинаков
	int difference = section_path_parts.size() - this->section_path_parts.size();

	// если размер у векторов, содержащих пути, совпадает
	if (difference == 1)
	{
		for (auto iter = sections_map.begin(); iter != sections_map.end(); ++iter)
		{
			// либо найдем нужную секцию
			if (iter->second.section_path_parts == section_path_parts)
			{
				return &(iter->second);
			}
		}

		// либо искомой секции нет
		return nullptr;
	}
	else
	{
		for (auto iter = sections_map.begin(); iter != sections_map.end(); ++iter)
		{
			// если с учетом разницы совпадают вектора
			if (HasEqualPathParts(iter->second.section_path_parts, section_path_parts))
			{
				return iter->second.FindSection(section_path_parts);
			}
		}

		// либо искомой секции нет
		return nullptr;
	}

	// либо искомой секции нет
	return nullptr;
}

CIni::Section*
CIni::Section::FindSection(const std::string section_path)
{
	// создадим вектор со всеми строками пути
	std::vector<std::string> subsection_path_parts(section_path_parts);

	subsection_path_parts.push_back(section_path);

	return FindSection(subsection_path_parts);
}

const CIni::Section*
CIni::Section::FindSection(const std::string section_path) const
{
	// создадим вектор со всеми строками пути
	std::vector<std::string> subsection_path_parts(section_path_parts);

	subsection_path_parts.push_back(section_path);

	return FindSection(subsection_path_parts);
}

void
CIni::Section::WriteValue(const std::string& key_name,
						bool value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::BOOL, value };
}

void
CIni::Section::WriteValue(const std::string& key_name,
						char value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::CHAR, value };
}

void
CIni::Section::WriteValue(const std::string& key_name,
						unsigned char value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::UCHAR, value };
}

void
CIni::Section::WriteValue(const std::string& key_name,
						int value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::INT, value };
}

void
CIni::Section::WriteValue(const std::string& key_name,
						unsigned int value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::UINT, value };

}

void
CIni::Section::WriteValue(const std::string& key_name,
						const long& value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::LONG, value };
}

void
CIni::Section::WriteValue(const std::string& key_name,
						const unsigned long& value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::ULONG, value };

}

void
CIni::Section::WriteValue(const std::string& key_name,
						const long long& value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::LONGLONG, value };
}

void
CIni::Section::WriteValue(const std::string& key_name,
						const unsigned long long& value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::ULONGLONG, value };
}

void
CIni::Section::WriteValue(const std::string& key_name,
						const double& value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::DOUBLE, value };
}

void
CIni::Section::WriteValue(const std::string& key_name,
						const std::string& value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}

	key_value_map[key_name] = { VALUE_TYPE::STRING, value };

}
void
CIni::Section::WriteValue(const std::string& key_name,
						const char* value)
{
	auto iter = key_value_map.find(key_name);

	if (iter != key_value_map.end())
	{
		// удаляем
		key_value_map.erase(key_name);
	}
	std::string str(value);
	key_value_map[key_name] = { VALUE_TYPE::STRING, str };

}

bool
CIni::Section::ReadValue(const std::string& key_name,
						bool& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			auto result = std::any_cast<char>(key_value_map.at(key_name).second);

			if (result == 0)
			{
				value = false;
				return true;
			}
			else if (result == 1)
			{
				value = true;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			auto result = std::any_cast<unsigned char>(key_value_map.at(key_name).second);

			if (result == 0)
			{
				value = false;
				return true;
			}
			else if (result == 1)
			{
				value = true;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::INT:
		{
			auto result = std::any_cast<int>(key_value_map.at(key_name).second);

			if (result == 0)
			{
				value = false;
				return true;
			}
			else if (result == 1)
			{
				value = true;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			auto result = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			if (result == 0)
			{
				value = false;
				return true;
			}
			else if (result == 1)
			{
				value = true;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			auto result = std::any_cast<long>(key_value_map.at(key_name).second);

			if (result == 0)
			{
				value = false;
				return true;
			}
			else if (result == 1)
			{
				value = true;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			auto result = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			if (result == 0)
			{
				value = false;
				return true;
			}
			else if (result == 1)
			{
				value = true;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (result == 0)
			{
				value = false;
				return true;
			}
			else if (result == 1)
			{
				value = true;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (result == 0)
			{
				value = false;
				return true;
			}
			else if (result == 1)
			{
				value = true;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			// если  double == 1              double с нулевой дробной частью 
			if ((std::trunc(result) == 1 && result - std::trunc(result) < 0.00001) ||
				// если  double == 0
				(std::trunc(result) == 0 &&
					((result > 0  /* если  double > 0 */ && result - std::trunc(result) < 0.00001) ||
					(result < 0  /* если  double < 0 */ && result - std::trunc(result) > -0.00001))))
			{
				value = static_cast<bool>(result);
				return true;
			}

			// потеря точности
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	// либо искомой секции нет
	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						char& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			value = std::any_cast<char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			auto result = std::any_cast<unsigned char>(key_value_map.at(key_name).second);

			if (result > CHAR_MAX)
				return false;

			value = result;
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			auto result = std::any_cast<int>(key_value_map.at(key_name).second);

			if (result >= CHAR_MIN &&
				result <= CHAR_MAX)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			auto result = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			if (result <= static_cast<unsigned int>(CHAR_MAX))
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			auto result = std::any_cast<long>(key_value_map.at(key_name).second);

			if (result >= CHAR_MIN &&
				result <= CHAR_MAX)
			{
				value = static_cast<char>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			auto result = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			if (result <= static_cast<unsigned long>(CHAR_MAX))
			{
				value = static_cast<char>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (result >= CHAR_MIN &&
				result <= CHAR_MAX)
			{
				value = static_cast<char>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (result <= CHAR_MAX)
			{
				value = static_cast<char>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			if ((std::trunc(result) >= 0 &&   // если  double  положительный
				result - std::trunc(result) < 0.00001 &&  // если  double с нулевой дробной частью 
				std::trunc(result) <= CHAR_MAX) ||  // если целая часть double умещается в char

				// если  double с нулевой дробной частью и double отрицательный
				(std::trunc(result) <= 0 &&  // если  double отрицательный
					result - std::trunc(result) > -0.00001 &&   // если  double с нулевой дробной частью 
					std::trunc(result) >= CHAR_MIN))  // если целая часть double умещается в char
			{
				value = static_cast<int>(result);
				return true;
			}

			// потеря точности
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						unsigned char& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			auto result = std::any_cast<char>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}
			return false;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			value = std::any_cast<unsigned char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			auto result = std::any_cast<int>(key_value_map.at(key_name).second);

			if (result >= 0 && result <= UCHAR_MAX)
			{
				value = result;
				return true;
			}
			return false;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			auto result = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			if (UCHAR_MAX < result)
				return false;

			value = result;
			return true;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			auto result = std::any_cast<long>(key_value_map.at(key_name).second);

			if (UCHAR_MAX < result || result < 0)
				return false;

			value = static_cast<unsigned char>(result);
			return true;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			auto result = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			if (UCHAR_MAX < result)
				return false;

			value = static_cast<unsigned char>(result);
			return true;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (UCHAR_MAX < result || result < 0)
				return false;

			value = static_cast<unsigned char>(result);
			return true;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (UCHAR_MAX < result)
				return false;

			value = static_cast<unsigned>(result);
			return true;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			if (std::trunc(result) >= 0 && // если это неотрицательный  double
				result - std::trunc(result) < 0.00001 &&  // если  double с нулевой дробной частью
				std::trunc(result) <= UCHAR_MAX) // если  double умещается в unsigned char
			{
				value = static_cast<unsigned char>(result);
				return true;
			}

			// потеря точности
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	// либо искомой секции нет
	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						int& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			value = std::any_cast<char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			value = std::any_cast<unsigned char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			value = std::any_cast<int>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			auto result = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			if (static_cast<unsigned int>(INT_MAX) < result)
				return false;

			value = result;
			return true;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			auto result = std::any_cast<long>(key_value_map.at(key_name).second);

			if (INT_MAX < result ||
				INT_MIN > result)
				return false;

			value = result;
			return true;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			auto result = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			if (static_cast<unsigned long>(INT_MAX) < result)
				return false;

			value = result;
			return true;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (INT_MAX < result ||
				INT_MIN > result)
				return false;

			value = static_cast<int>(result);
			return true;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (INT_MAX < result)
				return false;

			value = static_cast<int>(result);
			return true;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			if ((std::trunc(result) >= 0 &&   // если  double  положительный
				result - std::trunc(result) < 0.00001 &&  // если  double с нулевой дробной частью 
				std::trunc(result) <= INT_MAX) ||  // если целая часть double умещается в int

				// если  double с нулевой дробной частью и double отрицательный
				(std::trunc(result) <= 0 &&  // если  double отрицательный
				result - std::trunc(result) > -0.00001 &&   // если  double с нулевой дробной частью 
				std::trunc(result) >= INT_MIN))  // если целая часть double умещается в int
			{
				value = static_cast<int>(result);
				return true;
			}

			// потеря точности
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						unsigned int& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			auto result = std::any_cast<char>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			value = std::any_cast<unsigned char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			auto result = std::any_cast<int>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			value = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			auto result = std::any_cast<long>(key_value_map.at(key_name).second);

			if (result >= 0 &&
				result <= static_cast<long>(UINT_MAX))
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			auto result = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			if (UINT_MAX >= result)
			{
				value = result;
				return true;
			}
			return false;

			value = result;

		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (result >= 0 &&
				result <= UINT_MAX)
			{
				value = static_cast<unsigned int>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (UINT_MAX >= result)
			{
				value = static_cast<unsigned int>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			if (std::trunc(result) >= 0 && // если это неотрицательный  double
				result - std::trunc(result) < 0.00001 &&  // если  double с нулевой дробной частью
				std::trunc(result) <= UINT_MAX) // если  double умещается в unsigned int
			{
				value = static_cast<unsigned int>(result);
				return true;
			}

			// потеря точности
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						long& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			value = std::any_cast<char>(key_value_map.at(key_name).second);

			return true;

		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			value = std::any_cast<unsigned char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			value = std::any_cast<int>(key_value_map.at(key_name).second);

			return true;

		}
		case CIni::VALUE_TYPE::UINT:
		{
			value = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			value = std::any_cast<long>(key_value_map.at(key_name).second);

			return true;

		}
		case CIni::VALUE_TYPE::ULONG:
		{
			auto result = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			if (static_cast<unsigned long>(LONG_MAX) >= result)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (result <= LONG_MAX &&
				result >= LONG_MIN)
			{
				value = static_cast<long>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (LONG_MAX >= result)
			{
				value = static_cast<long>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			// если  double с нулевой дробной частью и double положительный
			if ((std::trunc(result) >= 0 &&
				result - std::trunc(result) < 0.00001 &&
				std::trunc(result) <= LONG_MAX) ||

				// если  double с нулевой дробной частью и double отрицательный
				(std::trunc(result) <= 0 &&  // если  double отрицательный
				result - std::trunc(result) > -0.00001 &&   // если  double с нулевой дробной частью 
				std::trunc(result) >= LONG_MIN))  // если целая часть double умещается в long
			{
				value = static_cast<long>(result);
				return true;
			}

			// потеря точности
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	// либо искомой секции нет
	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						unsigned long& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			auto result = std::any_cast<char>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			value = std::any_cast<unsigned char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			auto result = std::any_cast<int>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			value = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			auto result = std::any_cast<long>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			value = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (result >= 0 &&
				result <= ULONG_MAX)
			{
				value = static_cast<unsigned long>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (ULONG_MAX >= result)
			{
				value = static_cast<unsigned long>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			if (std::trunc(result) >= 0 && // если это неотрицательный  double
				result - std::trunc(result) < 0.00001 &&  // если  double с нулевой дробной частью
				std::trunc(result) <= ULONG_MAX) // если  double умещается в unsigned long
			{
				value = static_cast<unsigned long>(result);
				return true;
			}

			// потеря точности
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	// либо искомой секции нет
	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						long long& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			value = std::any_cast<char>(key_value_map.at(key_name).second);

			return true;

		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			value = std::any_cast<unsigned char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			value = std::any_cast<int>(key_value_map.at(key_name).second);

			return true;

		}
		case CIni::VALUE_TYPE::UINT:
		{
			value = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			value = std::any_cast<long>(key_value_map.at(key_name).second);

			return true;

		}
		case CIni::VALUE_TYPE::ULONG:
		{
			value = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			return true;

		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			value = std::any_cast<long long>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (static_cast<unsigned long long>(LLONG_MAX) >= result)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			// если  double с нулевой дробной частью и double положительный
			if ((std::trunc(result) >= 0 && result - std::trunc(result) < 0.00001) ||
				// если  double с нулевой дробной частью и double отрицательный
				(std::trunc(result) <= 0 && result - std::trunc(result) > -0.00001))
			{
				value = static_cast<long long>(result);
				return true;
			}

			// потеря точности
			return false;
		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	// либо искомой секции нет
	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						unsigned long long& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			auto result = std::any_cast<char>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			value = std::any_cast<unsigned char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			auto result = std::any_cast<int>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			value = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			auto result = std::any_cast<long>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			value = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (result >= 0)
			{
				value = result;
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			value = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			auto result = std::any_cast<double>(key_value_map.at(key_name).second);

			if (std::trunc(result) >= 0 && // если это неотрицательный  double
				result - std::trunc(result) < 0.00001) // если  double с нулевой дробной частью
			{
				value = static_cast<unsigned long long>(result);
				return true;
			}

			// потеря точности
			return false;
		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	// либо искомой секции нет
	return false;
}

bool
CIni::Section::ReadValue(const std::string& key_name,
						double& value) const
{
	if (key_value_map.find(key_name) != key_value_map.end())
	{
		switch (key_value_map.at(key_name).first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			value = std::any_cast<bool>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			value = std::any_cast<char>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			value = std::any_cast<unsigned char>(key_value_map.at(key_name).second);
			return true;
		}
		case CIni::VALUE_TYPE::INT:
		{
			value = std::any_cast<int>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			value = std::any_cast<unsigned int>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			value = std::any_cast<long>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			value = std::any_cast<unsigned long>(key_value_map.at(key_name).second);

			return true;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			auto result = std::any_cast<long long>(key_value_map.at(key_name).second);

			if (result <= DBL_MAX &&
				result >= DBL_MIN)
			{
				value = static_cast<double>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			auto result = std::any_cast<unsigned long long>(key_value_map.at(key_name).second);

			if (result <= DBL_MAX)
			{
				value = static_cast<double>(result);
				return true;
			}

			return false;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			value = std::any_cast<double>(key_value_map.at(key_name).second);
			return true;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// преобразование невозможно
			return false;
		}
		}
	}

	// либо искомой секции нет
	return false;
}


bool
CIni::Section::ReadValue(const std::string& key_name,
						 std::string& value) const
{
	switch (key_value_map.at(key_name).first)
	{
	case CIni::VALUE_TYPE::BOOL:
	{
		value = std::to_string(std::any_cast<bool>(key_value_map.at(key_name).second));
		return true;
	}
	case CIni::VALUE_TYPE::CHAR:
	{
		value = std::any_cast<char>(key_value_map.at(key_name).second);

		return true;
	}
	case CIni::VALUE_TYPE::UCHAR:
	{
		value = std::to_string(std::any_cast<unsigned char>(key_value_map.at(key_name).second));
		return true;
	}
	case CIni::VALUE_TYPE::INT:
	{
		value = std::to_string(std::any_cast<int>(key_value_map.at(key_name).second));

		return true;
	}
	case CIni::VALUE_TYPE::UINT:
	{
		value = std::to_string(std::any_cast<unsigned int>(key_value_map.at(key_name).second));

		return true;
	}
	case CIni::VALUE_TYPE::LONG:
	{
		value = std::to_string(std::any_cast<long>(key_value_map.at(key_name).second));

		return true;
	}
	case CIni::VALUE_TYPE::ULONG:
	{
		value = std::to_string(std::any_cast<unsigned long>(key_value_map.at(key_name).second));

		return true;
	}
	case CIni::VALUE_TYPE::LONGLONG:
	{
		value = std::to_string(std::any_cast<long long>(key_value_map.at(key_name).second));

		return true;
	}
	case CIni::VALUE_TYPE::ULONGLONG:
	{
		value = std::to_string(std::any_cast<unsigned long long>(key_value_map.at(key_name).second));

		return true;
	}
	case CIni::VALUE_TYPE::DOUBLE:
	{
		value = std::to_string(std::any_cast<double>(key_value_map.at(key_name).second));
		return true;

	}
	case CIni::VALUE_TYPE::STRING:
	{
		value = std::any_cast<std::string>(key_value_map.at(key_name).second);
		return true;
	}
	}
}


////////////////////////////////////////////////////////////////
// Функции класа CIni

CIni::CIni(const std::string& str) : m_file_full_path(str)
{
	InitSectionsNames();
	InitKeyValuePairs();
}

bool 
CIni::IsEmpty() const
{
	return m_sections.empty();
}

bool
CIni::FileExists() const
{
	if (m_file_full_path != "" &&
		::GetFileAttributes(m_file_full_path.c_str()) != DWORD(-1) &&
		_access(m_file_full_path.c_str(), 0) == 0) // - 00 Проверка только на существование
	{
		// Файл существует
		return true;
	}
	else
	{
		// Файл НЕ существует
		return false;
	};
}

void
CIni::InitCIni()
{
	InitSectionsNames();
	InitKeyValuePairs();
}

bool
CIni::IsReadOnly() const
{
	DWORD dwAttrs = GetFileAttributes(m_file_full_path.c_str());

	if (dwAttrs & FILE_ATTRIBUTE_READONLY)
	{
		return true;
	}

	return false;
}

void
CIni::InitSectionsNames()
{
	// сначала записываем в массив ИМЕНА всех секций
	std::vector<std::string> sections_names;


	unsigned int result = 0;
	unsigned int sections_buffer_size = 1000; // размер буфера для символов
	char* symbols = new char[sections_buffer_size];


	do
	{
		if (result == sections_buffer_size - 2)
		{
			// если массив не равен nullptr, 
			// значит пытались прочесть все имена секций, 
			// и не хватило размера буфера
			delete[] symbols;
			sections_buffer_size *= 2;

			symbols = new char[sections_buffer_size];
		}


		result = GetPrivateProfileSectionNames(
			symbols, // [out] LPTSTR  lpszReturnBuffer  // буфер, куда пишем названия секций
			sections_buffer_size, // [in]  DWORD     // nSize,  размер буфера
			m_file_full_path.c_str() // [in]  LPCTSTR lpFileName     // файл ini
		);
		/*
		Возвращаемое значение указывает количество символов, скопированных в указанный буфер,
		не включая завершающий символ NULL .
		Если буфер недостаточно велик для хранения всех имен разделов,
		связанных с указанным файлом инициализации,
		возвращаемое значение равно размеру, заданному параметром nSize минус два.
		*/



	} while (result == sections_buffer_size - 2);
	// буфер недостаточно велик для хранения всех имен разделов
	// может надо в цикле do-while записывать имена секций в буфер , пока все имена секций не уместились
	// и нужно не константу, а локальную переменную для размера буфера,
	//	чтоб она увеличивалась в 2 раза.


	std::string section;

	// движемся по всем символам
	for (int symbol_index = 0;
		symbol_index < static_cast<int>(result);
		++symbol_index)
	{

		if (symbols[symbol_index] != NULL)
		{
			section += symbols[symbol_index];
		}
		else // if (symbols[i] == NULL)
		{
			sections_names.insert(sections_names.end(), section);
			section.clear();
		}

	}
	delete[] symbols;

	// ОТ НАЧАЛА МАССИВА С НАЗВАНИЯМИ СЕКЦИЙ

	// есть строки всех  секций

	// ДВИГАЕМСЯ ПО НАЗВАНИЯМ СЕКЦИЙ 
	int section_names_count = sections_names.size();

	for (int section_index = 0;
		section_index < section_names_count;
		++section_index)
	{

		AddSection(sections_names[section_index]);
	}
}

void
CIni::InitKeyValuePairs()
{
	for (auto section_iter = m_sections.begin();
		section_iter != m_sections.end();
		++section_iter)
	{

		section_iter; // ЭТО ПАРА  СТРОКА "полный путь секции" - СЕКЦИЯ.
		section_iter->second; // ЭТО СЕКЦИЯ.   ПО СЕКЦИИ НАДО ПРОХОДИТЬСЯ ЦИКЛОМ
		section_iter->first; //  путь до секции

		InitKeyValuePairs(&(section_iter->second));
	}
}

void
CIni::InitKeyValuePairs(Section* p_section)
{
	unsigned int result = 0;
	unsigned int keys_buffer_size = 1000; // размер буфера для символов
	char* symbols = new char[keys_buffer_size];

	do
	{
		if (result == keys_buffer_size - 2)
		{
			// если массив не равен nullptr, 
			// значит пытались прочесть все имена секций, 
			// и не хватило размера буфера
			delete[] symbols;
			keys_buffer_size *= 2;

			symbols = new char[keys_buffer_size];
		}

		result = GetPrivateProfileString(
			p_section->section_path.c_str(), // [in]  LPCTSTR lpAppName,
			NULL, //[in]  LPCTSTR lpKeyName,
			NULL, // [in]  LPCTSTR lpDefault,
			symbols, // [out] LPTSTR  lpReturnedString,  // буфер, куда пишем все значения от пар "ключ-значение" указанной секции
			keys_buffer_size, //  [in]  DWORD   nSize,  // nSize,  размер буфера
			m_file_full_path.c_str() // [in]  LPCTSTR lpFileName     // файл ini
		);
		/*
		Если параметр [in] lpKeyName имеет значение NULL,
		все имена ключей в разделе, указанном параметром lpAppName,
		копируются в буфер, указанный параметром lpReturnedString
		*/

		/*
		Если lpKeyName имеет значение NULL ,
		а буфер назначения слишком мал для хранения всех строк,
		последняя строка усекается и за ней следуют два символа NULL .
		В этом случае возвращаемое значение равно nSize минус два.
		*/


	} while (result == keys_buffer_size - 2);
	// буфер недостаточно велик для хранения всех имен разделов
	// может надо в цикле do-while записывать имена секций в буфер , пока все имена секций не уместились
	// и нужно не константу, а локальную переменную для размера буфера,
	//	чтоб она увеличивалась в 2 раза.


	// ИМЕЕМ БУФЕР СИМВОЛОВ СО ВСЕМИ КЛЮЧАМИ ДАННОЙ СЕКЦИИ
	std::string key;

	// движемся по всем символам буфера
	for (int symbol_index = 0;
		symbol_index < static_cast<int>(result);
		++symbol_index)
	{
		// ЕСЛИ НЕ ДОШЛИ ДО КОНЦА СТРОКИ КЛЮЧА
		if (symbols[symbol_index] != NULL)
		{
			key += symbols[symbol_index];
		}
		// ПОЛНОСТЬЮ СФОРМИРОВАЛИ СТРОКУ КЛЮЧА
		else // if (symbols[i] == NULL)
		{

			int value_result = 0;
			int value_buffer_size = 200; // размер буфера для символов
			char* value_symbols = new char[value_buffer_size];

			do
			{
				if (result == value_buffer_size - 2)
				{
					// если массив не равен nullptr, 
					// значит пытались прочесть все имена секций, 
					// и не хватило размера буфера
					delete[] symbols;
					value_buffer_size *= 2;

					value_symbols = new char[value_buffer_size];
				}

				value_result = GetPrivateProfileString(
					p_section->section_path.c_str(), // [in]  LPCTSTR lpAppName,
					key.c_str(), //[in]  LPCTSTR lpKeyName,
					NULL, // [in]  LPCTSTR lpDefault,
					value_symbols, // [out] LPTSTR  lpReturnedString,  // буфер, куда пишем все значения от пар "ключ-значение" указанной секции
					value_buffer_size, //  [in]  DWORD   nSize,  // nSize,  размер буфера
					m_file_full_path.c_str() // [in]  LPCTSTR lpFileName     // файл ini
				);

			} while (value_result == value_buffer_size - 2);

			// имеем строку со значением
			std::string value_str(value_symbols);

			//if (value_result == 1 &&
			//	value_str[0] == '1')
			//{
			//	p_section->key_value_map.insert({ key, {VALUE_TYPE::BOOL, true} });
			//}
			//else if (value_result == 1 &&
			//	value_str[0] == '0')
			//{
			//	p_section->key_value_map.insert({ key, {VALUE_TYPE::BOOL, false} });
			//}
			//else
			{
				std::regex double_int_rx(R"(^([+-]?(?:[[:d:]]+\.?|[[:d:]]*\.[[:d:]]+))(?:[Ee][+-]?[[:d:]]+)?$)");
				std::regex int_rx("-?[[:digit:]]+");

				// является ли строка целым значением ?
				if (std::regex_match(value_str, int_rx))
				{
					// число отрицательное
					if (value_symbols[0] == '-')
					{
						// самое большое знаковое число
						long long number = std::stoll(std::string(value_symbols));

						AddPairToSection(p_section, key, number);

					}
					// неотрицательное число
					else
					{
						// самое большое беззнаковое число
						unsigned long long number = std::stoull(std::string(value_symbols));

						AddPairToSection(p_section, key, number);

					}
				}
				// является ли строка числом с плавав. точкой ?
				else if (std::regex_match(value_str, double_int_rx))
				{
					p_section->key_value_map.insert({ key, {VALUE_TYPE::DOUBLE, std::stod(value_str) } });
				}
				
				else
				{
					// если строка состоит только из одного символа
					if (value_str.size() == 1)
						p_section->key_value_map.insert({ key, {VALUE_TYPE::CHAR, value_str[0] } });
					else if (value_str == "true")
						p_section->key_value_map.insert({ key, {VALUE_TYPE::BOOL, true } });
					else if (value_str == "false")
						p_section->key_value_map.insert({ key, {VALUE_TYPE::BOOL, false } });
					else // это просто строка
						p_section->key_value_map.insert({ key, {VALUE_TYPE::STRING, value_str } });
				}

			}

			delete[] value_symbols;

			key.clear();
		}
	}
	delete[] symbols;

	// заполняем подсекции
	for (auto iter = p_section->sections_map.begin();
		iter != p_section->sections_map.end(); ++iter)
	{
		InitKeyValuePairs(&(iter->second));
	}
}

void
CIni::AddPairToSection(Section* p_section,
						const std::string& key,
						const long long& number)
{
	/* 
	if (number <= SCHAR_MAX &&
		number >= SCHAR_MIN)
	{
		p_section->key_value_map.insert({ key, { VALUE_TYPE::CHAR, static_cast<char>(number)  } });
	}
	else */if (number <= INT_MAX &&
		number >= INT_MIN)
	{
		p_section->key_value_map.insert({ key, { VALUE_TYPE::INT, static_cast<int>(number) } });
	}
	else if (number <= LONG_MAX &&
		number >= LONG_MIN)
	{
		p_section->key_value_map.insert({ key, { VALUE_TYPE::LONG, static_cast<long>(number) } });
	}
	else //if (number <= LLONG_MAX &&
		//number >= LLONG_MIN)
	{
		p_section->key_value_map.insert({ key, { VALUE_TYPE::LONGLONG, number } });
	}
}

void
CIni::AddPairToSection(Section* p_section,
						const std::string& key,
						const unsigned long long& number)
{
	if (number <= UCHAR_MAX)
	{
		p_section->key_value_map.insert({ key, { VALUE_TYPE::UCHAR, static_cast<unsigned char>(number) } });
	}
	else if (number <= UINT_MAX)
	{
		p_section->key_value_map.insert({ key, { VALUE_TYPE::UINT, static_cast<unsigned int>(number)  } });
	}
	else if (number <= ULONG_MAX)
	{
		p_section->key_value_map.insert({ key, { VALUE_TYPE::ULONG, static_cast<unsigned long>(number) } });
	}
	else //if (number <= ULLONG_MAX)
	{
		p_section->key_value_map.insert({ key, { VALUE_TYPE::ULONGLONG, number } });
	}
}

CIni::Section*
CIni::FindSection(const std::string section_path)
{
	// распарсим путь текущей секции
	std::vector<std::string> section_path_parts;

	std::istringstream f(section_path);
	std::string s;

	while (getline(f, s, '\\'))
		section_path_parts.push_back(s);

	return FindSection(section_path_parts);
}

const CIni::Section*
CIni::FindSection(const std::string section_path) const
{
	// распарсим путь текущей секции
	std::vector<std::string> section_path_parts;

	std::istringstream f(section_path);
	std::string s;

	while (getline(f, s, '\\'))
		section_path_parts.push_back(s);

	return FindSection(section_path_parts);
}

void
CIni::AddSection(const std::string& section_path)
{
	// если :unordered_map с секциями еще пуст
	if (m_sections.empty())
	{
		m_sections.insert({ section_path, Section(section_path) });

	}
	else
	{
		// это НЕ подсекция. это секция верхнего уровня
		if (section_path.find('\\') == std::string::npos)
		{
			m_sections.insert({ section_path, Section(section_path) });
		}
		// это подсекция
		else
		{
			// сконструировать строку пути родительской секции
			auto pos = section_path.find_last_of('\\');
			std::string parent_section_path(section_path, 0, pos);

			// найти родительскую секцию и добавить в нее 
			Section* parent_section = FindSection(parent_section_path);

			// 3) добавить в родительскую секцию новую подсекцию
			if (parent_section == nullptr)
			{
				AddSection(parent_section_path); //return false;
				parent_section = FindSection(parent_section_path);
			}

			parent_section->sections_map.insert({ section_path, Section(section_path) });

		}
	}
}

bool
CIni::SectionExists(const std::string& section) const
{
	if (section.find('\\') == std::string::npos)
	{
		for (auto iter = m_sections.cbegin();
			iter != m_sections.cend(); ++iter)
		{

			if (iter->first == section)
				return true;
		}

		return false;
	}
	else
	{
		const Section* p_section = FindSection(section);

		if (p_section != nullptr)
			return true;
		else
			return false;
	}

}

bool
CIni::KeyExists(const std::string& section_path,
				const std::string& key) const
{
	// если секция НЕ является подсекцией
	if (section_path.find('\\') == std::string::npos)
	{
		for (auto iter = m_sections.cbegin();
			iter != m_sections.cend(); ++iter)
		{

			if (iter->first == section_path)
			{

				for (auto key_value_iter = iter->second.key_value_map.cbegin();
					key_value_iter != iter->second.key_value_map.cend();
					++key_value_iter)
				{

					if (key_value_iter->first == key)
						return true;
				}

			}
		}

		return false;
	}
	// если это ПОДсекция
	else
	{
		const Section* p_section = FindSection(section_path);

		if (p_section != nullptr)
		{
			for (auto iter = p_section->key_value_map.cbegin();
				iter != p_section->key_value_map.cend(); ++iter)
			{

				if (iter->first == key)
					return true;
			}
		}

		return false;
	}
}

std::unordered_map<std::string, CIni::Section>&
CIni::GetSectionMap(const std::string section_path)
{
	Section* p_section = FindSection(section_path);

	return p_section->sections_map;
}

const std::unordered_map<std::string, CIni::Section>&
CIni::GetSectionMap(const std::string section_path) const
{
	const Section* p_section = FindSection(section_path);

	return p_section->sections_map;
}

std::unordered_map<std::string, std::pair<CIni::VALUE_TYPE, std::any>>&
CIni::GetKeyValueSectionMap(const std::string section_path)
{
	Section* p_section = FindSection(section_path);

	return p_section->key_value_map;
}

const std::unordered_map<std::string, std::pair<CIni::VALUE_TYPE, std::any>>&
CIni::GetKeyValueSectionMap(const std::string section_path) const
{
	const Section* p_section = FindSection(section_path);

	return p_section->key_value_map;
}

CIni::Section*
CIni::FindSection(const std::vector<std::string>& section_path_parts)
{
	// если ищем верхнего уровня секцию
	if (section_path_parts.size() == static_cast<unsigned int>(1))
	{
		auto iter = m_sections.find(section_path_parts[0]);
		if (iter != m_sections.end())
			return &(iter->second);


		return nullptr;
	}
	else // if (section_path_parts.size() != 1)
	{

		for (auto iter = m_sections.begin();
			iter != m_sections.end(); ++iter)
		{

			if (iter->first == section_path_parts[0])
				return iter->second.FindSection(section_path_parts);
		}

		return nullptr;
	}
}

const CIni::Section*
CIni::FindSection(const std::vector<std::string>& section_path_parts) const
{
	// если ищем верхнего уровня секцию
	if (section_path_parts.size() == static_cast<unsigned int>(1))
	{
		auto iter = m_sections.find(section_path_parts[0]);
		if (iter != m_sections.end())
			return &(iter->second);


		return nullptr;
	}
	else // if (section_path_parts.size() != 1)
	{

		for (auto iter = m_sections.cbegin();
			iter != m_sections.cend(); ++iter)
		{

			if (iter->first == section_path_parts[0])
				return iter->second.FindSection(section_path_parts);
		}

		return nullptr;
	}
}

unsigned int
CIni::SectionsCount() const
{
	return m_sections.size();
}

unsigned int
CIni::KeysCount(const std::string& section_path) const
{
	// если символ '\\' не найден с строке пути
	if (section_path.find('\\') == std::string::npos)
	{
		for (auto iter = m_sections.cbegin(); iter != m_sections.cend(); ++iter)
			if (iter->first == section_path)
				return iter->second.key_value_map.size();
	}
	else
	{
		const Section* p_section = FindSection(section_path);

		return p_section->key_value_map.size();
	}
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				bool value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				char value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				unsigned char value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				int value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}
		
	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				unsigned int value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				const long& value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				const unsigned long& value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return false;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				const long long& value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				const unsigned long long& value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				const double& value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				const std::string& value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_name,
				const std::string& key_name,
				const char* value)
{
	// не записываем, если у файла атрибут ReadOnly
	//if (IsReadOnly())
	//	return false;

	Section* p_section = FindSection(section_name);

	if (p_section == nullptr)
	{
		AddSection(section_name); //return false;
		p_section = FindSection(section_name);
	}

	p_section->WriteValue(key_name, value);
	return true;
}

bool
CIni::WriteValue(const std::string& section_key_name,
				bool value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				char value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				unsigned char value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				int value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				unsigned int value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const long& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const unsigned long& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const long long& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const unsigned long long& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const double& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const std::string& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

bool 
CIni::WriteValue(const std::string& section_key_name,
				const char* value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return WriteValue(section_name, key_name, value);
}

void
CIni::SetFullFilePath(const std::string& str)
{
	m_file_full_path = str;
}

bool
CIni::WriteToIni() const
{
	//if (!IsReadOnly())
	{
		bool result;
		bool all_result = true;

		for (auto iter = m_sections.cbegin();
			iter != m_sections.cend(); ++iter)
		{

			result = iter->second.WriteToIni(m_file_full_path);

			// если хоть одна пара в какой-либо секции не записалась
			// возвращаем false
			if (result == false)
				all_result = false;
		}

		return all_result;
	}
	//else
	//	return false;
}

bool
CIni::SaveValue(const std::string& section_name,
				const std::string& key_name,
				const std::string& value) const
{
	return WritePrivateProfileString(
		section_name.c_str(),
		key_name.c_str(),
		value.c_str(),
		m_file_full_path.c_str());
}

bool 
CIni::SaveValue(const std::string& section_name,
				const std::string& key_name,
				char value) const
{
	return WritePrivateProfileString(
		section_name.c_str(),
		key_name.c_str(),
		&value,
		m_file_full_path.c_str());
}

bool
CIni::SaveValue(const std::string& section_name,
				const std::string& key_name,
				const char* value) const
{
	return WritePrivateProfileString(
		section_name.c_str(),
		key_name.c_str(),
		value,
		m_file_full_path.c_str());
}

bool
CIni::SaveValue(const std::string& section_name,
				const std::string& key_name,
				bool value) const
{
	std::string str;

	if (value == true)
		str = "true";
	else
		str = "false";

	return WritePrivateProfileString(
		section_name.c_str(),
		key_name.c_str(),
		str.c_str(),
		m_file_full_path.c_str());
}

bool
CIni::SaveValue(const std::string& section_key_name,
				char value) const
{
	// разделяем строку на путь до секции и ключ

	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
						separator + 1, // позиция начала
						section_key_name.size() - section_name.size() - 1); // количество символов

	return SaveValue(section_name, key_name, value);
}

bool
CIni::SaveValue(const std::string& section_key_name,
				const char* value) const
{
	// разделяем строку на путь до секции и ключ

	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return SaveValue(section_name, key_name, value);
}

bool
CIni::SaveValue(const std::string& section_key_name,
				const std::string& value) const
{
	// разделяем строку на путь до секции и ключ

	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return SaveValue(section_name, key_name, value);
}

bool
CIni::SaveValue(const std::string& section_key_name,
				bool value) const
{
	// разделяем строку на путь до секции и ключ

	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return SaveValue(section_name, key_name, value);
}

void CIni::Section::Print() const
{
	for (auto iter = key_value_map.cbegin();
		iter != key_value_map.cend(); ++iter)
	{
		iter->first; // КЛЮЧ
		iter->second.first; // ТИП

		switch (iter->second.first)
		{
		case CIni::VALUE_TYPE::BOOL:
		{
			bool value = std::any_cast<bool>(iter->second.second);
			std::cout << iter->first << " = "
				<< (value ? "true" : "false")
				<< " (type BOOL)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::CHAR:
		{
			char value = std::any_cast<char>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type CHAR)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::UCHAR:
		{
			unsigned char value = std::any_cast<unsigned char>(iter->second.second);
			std::cout << iter->first << " = "
				<< static_cast<int>(value)
				<< " (type UCHAR)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::INT:
		{
			int value = std::any_cast<int>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type INT)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::UINT:
		{
			unsigned int value = std::any_cast<unsigned int>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type UINT)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::LONG:
		{
			long value = std::any_cast<long>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type LONG)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::ULONG:
		{
			unsigned long value = std::any_cast<unsigned long>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type ULONG)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::LONGLONG:
		{
			long long value = std::any_cast<long long>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type LONGLONG)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::ULONGLONG:
		{
			unsigned long long value = std::any_cast<unsigned long long>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type ULONGLONG)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::DOUBLE:
		{
			double value = std::any_cast<double>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type DOUBLE)" << std::endl;
			break;
		}
		case CIni::VALUE_TYPE::STRING:
		{
			std::string value = std::any_cast<std::string>(iter->second.second);
			std::cout << iter->first << " = "
				<< value
				<< " (type STRING)" << std::endl;
			break;
		}
		}
	}

	for (auto iter = sections_map.cbegin();
		iter != sections_map.cend(); ++iter)
	{
		std::cout << '['
			<< iter->first  // ИМЯ  СЕКЦИИ
			<< ']'
			<< std::endl;

		iter->second.Print();
	}
}

void CIni::Print() const
{
	for (auto iter = m_sections.cbegin();
		iter != m_sections.cend(); ++iter)
	{

		std::cout << '['
				  << iter->first  // ИМЯ  СЕКЦИИ
				  << ']'
				  << std::endl;

		iter->second.Print();
	}
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	bool& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	char& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	unsigned char& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	int& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	unsigned int& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	unsigned long& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	long& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	long long& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
	return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	unsigned long long& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	double& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}

bool
CIni::ReadValue(const std::string& section_name,
	const std::string& key_name,
	std::string& value) const
{
	const Section* p_section = FindSection(section_name);

	if (p_section != nullptr)
		return p_section->ReadValue(key_name, value);
	else
		return false;
}


// перегруженные функции чтения значения в переменную
// где первый аргумент - путь до секции совмещенный с ключом
bool
CIni::ReadValue(const std::string& section_key_name,
	bool& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	char& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	unsigned char& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	int& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	unsigned int& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	long& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	unsigned long& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	long long& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	unsigned long long& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	double& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	std::string& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // берем часть этой строки
		separator + 1, // позиция начала
		section_key_name.size() - section_name.size() - 1); // количество символов

	return ReadValue(section_name, key_name, value);
}

/*
CIni::Section::Section(const std::string& section_path,
						std::string& file) :
						section_path(section_path),
						ref_file_full_path(file)
{
	// из полного пути секции вычленяем path родительской секции

	std::istringstream f(section_path); // распарсим путь текущей секции
	std::string s;

	while (getline(f, s, '\\'))
		section_path_parts.push_back(s);


	int section_path_parts_size = section_path_parts.size();
	for (int i = 0; i < section_path_parts_size - 1; ++i)
	{
		if (i != 0)
			parent_section += '\\';

		parent_section += section_path_parts[i];
	}

}


// вызывать вместо AddSection() в InitSectionsNames()
CIni::Section*
CIni::CreateSection(const std::string& section_path)
{
	// если :unordered_map с секциями еще пуст
	if (m_sections.empty())
	{
		m_sections.insert({ section_path, Section(section_path, m_file_full_path) });

	}
	else
	{
		// это НЕ подсекция. это секция верхнего уровня
		if (section_path.find('\\') == std::string::npos)
		{
			m_sections.insert({ section_path, Section(section_path, m_file_full_path) });
		}
		// это подсекция
		else
		{
			// 1) сконструировать строку пути родительской секции
			auto pos = section_path.find_last_of('\\');
			std::string parent_section_path(section_path, 0, pos);

			// 2) найти родительскую секцию и добавить в нее
			Section* parent_section = FindSection(parent_section_path);


			// 3) если родительской мапы нет в структуре,
			// добавляем сначала родительские секции, потов в них добавляем данную подсекцию
			if (parent_section == nullptr)
			{
				// распарсим путь текущей секции
				std::vector<std::string> section_path_parts;

				std::istringstream f(section_path);
				std::string s;

				while (getline(f, s, '\\'))
					section_path_parts.push_back(s);

				std::string parent_section_path;

				// добавляем сначала недостающие родительские секции и подсекции
				for (int i = 0; i < static_cast<int>(section_path_parts.size()); ++i)
				{
					if (parent_section_path.empty())
						parent_section_path = section_path_parts[0];
					else
						parent_section_path += "\\" + section_path_parts[i];

					if (!SectionExists(parent_section_path))
						CreateSection(parent_section_path);
				}

				// потом добавляем в существующую родительскую секцию данную подсекцию
				parent_section = FindSection(parent_section_path);
			}

			// 4) добавить в родительскую секцию новую подсекцию
			parent_section->sections_map.insert({ section_path, Section(section_path, m_file_full_path) });
		}
	}

	auto p_section = FindSection(section_path);
	p_section->ref_file_full_path = m_file_full_path;

	return p_section;

	return FindSection(section_path);
}
*/