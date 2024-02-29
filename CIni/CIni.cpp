#include "CIni.h"
#include <algorithm>
#include <any>
#include <cmath>
#include <io.h>
#include <iostream>
#include <regex>
#include <windows.h>



/////////////////////////////////////////////////////////////////////
// ������� ������ Section

CIni::Section::Section(const std::string& section_path)
		: section_path(section_path)
{
	// �� ������� ���� ������ ��������� path ������������ ������

	std::istringstream f(section_path); // ��������� ���� ������� ������
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
	// ������������ ������ ���� �� ����� ������
	std::string full_section_path = section_path + '\\' + section_name;

	for (auto iter = sections_map.begin();
		iter != sections_map.end(); ++iter)
	{
		// ���� ������ ��� ����������, ���������� �� 
		if (iter->first == full_section_path)
			return &(iter->second);

	}

	// ���� ����� ������ ���, ����� �� ����� �������. ������� ������
	sections_map.insert({ full_section_path, Section(full_section_path) });

	return FindSection(section_name);
}

bool
CIni::Section::WriteToIni(const std::string& m_full_file_path) const
{
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ��������� � ������
	this->section_path;

	// ���� � ������ ������ ��� ���������
	if (sections_map.empty())
		return nullptr;
	else
	{
		// ��� ��������� �������� ����� ����� ������ ������ � ����. � ������ ������ ������ ���� ����� ��������
		int difference = section_path_parts.size() - sections_map.cbegin()->second.section_path_parts.size();

		// ���� ������ � ��������, ���������� ����, ���������
		if (difference == 0)
		{
			for (auto iter = sections_map.cbegin(); iter != sections_map.cend(); ++iter)
			{
				// ���� ������ ������ ������
				if (iter->second.section_path_parts == section_path_parts)
				{
					return &(iter->second);
				}
			}

			// ���� ������� ������ ���
			return nullptr;
		}
		else
		{
			for (auto iter = sections_map.cbegin(); iter != sections_map.cend(); ++iter)
			{
				// ���� � ������ ������� ��������� �������
				if (HasEqualPathParts(iter->second.section_path_parts, section_path_parts))
				{
					return iter->second.FindSection(section_path_parts);
				}
			}

			// ���� ������� ������ ���
			return nullptr;
		}
	}
	
	// ���� ������� ������ ���
	return nullptr;
}

CIni::Section*
CIni::Section::FindSection(const std::vector<std::string>& section_path_parts)
{
	auto vec = this->section_path_parts;

	// ��� ��������� �������� ����� ����� ������ ������ � ����. � ������ ������ ������ ���� ����� ��������
	int difference = section_path_parts.size() - this->section_path_parts.size();

	// ���� ������ � ��������, ���������� ����, ���������
	if (difference == 1)
	{
		for (auto iter = sections_map.begin(); iter != sections_map.end(); ++iter)
		{
			// ���� ������ ������ ������
			if (iter->second.section_path_parts == section_path_parts)
			{
				return &(iter->second);
			}
		}

		// ���� ������� ������ ���
		return nullptr;
	}
	else
	{
		for (auto iter = sections_map.begin(); iter != sections_map.end(); ++iter)
		{
			// ���� � ������ ������� ��������� �������
			if (HasEqualPathParts(iter->second.section_path_parts, section_path_parts))
			{
				return iter->second.FindSection(section_path_parts);
			}
		}

		// ���� ������� ������ ���
		return nullptr;
	}

	// ���� ������� ������ ���
	return nullptr;
}

CIni::Section*
CIni::Section::FindSection(const std::string section_path)
{
	// �������� ������ �� ����� �������� ����
	std::vector<std::string> subsection_path_parts(section_path_parts);

	subsection_path_parts.push_back(section_path);

	return FindSection(subsection_path_parts);
}

const CIni::Section*
CIni::Section::FindSection(const std::string section_path) const
{
	// �������� ������ �� ����� �������� ����
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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
		// �������
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

			// ����  double == 1              double � ������� ������� ������ 
			if ((std::trunc(result) == 1 && result - std::trunc(result) < 0.00001) ||
				// ����  double == 0
				(std::trunc(result) == 0 &&
					((result > 0  /* ����  double > 0 */ && result - std::trunc(result) < 0.00001) ||
					(result < 0  /* ����  double < 0 */ && result - std::trunc(result) > -0.00001))))
			{
				value = static_cast<bool>(result);
				return true;
			}

			// ������ ��������
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
			return false;
		}
		}
	}

	// ���� ������� ������ ���
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

			if ((std::trunc(result) >= 0 &&   // ����  double  �������������
				result - std::trunc(result) < 0.00001 &&  // ����  double � ������� ������� ������ 
				std::trunc(result) <= CHAR_MAX) ||  // ���� ����� ����� double ��������� � char

				// ����  double � ������� ������� ������ � double �������������
				(std::trunc(result) <= 0 &&  // ����  double �������������
					result - std::trunc(result) > -0.00001 &&   // ����  double � ������� ������� ������ 
					std::trunc(result) >= CHAR_MIN))  // ���� ����� ����� double ��������� � char
			{
				value = static_cast<int>(result);
				return true;
			}

			// ������ ��������
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
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

			if (std::trunc(result) >= 0 && // ���� ��� ���������������  double
				result - std::trunc(result) < 0.00001 &&  // ����  double � ������� ������� ������
				std::trunc(result) <= UCHAR_MAX) // ����  double ��������� � unsigned char
			{
				value = static_cast<unsigned char>(result);
				return true;
			}

			// ������ ��������
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
			return false;
		}
		}
	}

	// ���� ������� ������ ���
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

			if ((std::trunc(result) >= 0 &&   // ����  double  �������������
				result - std::trunc(result) < 0.00001 &&  // ����  double � ������� ������� ������ 
				std::trunc(result) <= INT_MAX) ||  // ���� ����� ����� double ��������� � int

				// ����  double � ������� ������� ������ � double �������������
				(std::trunc(result) <= 0 &&  // ����  double �������������
				result - std::trunc(result) > -0.00001 &&   // ����  double � ������� ������� ������ 
				std::trunc(result) >= INT_MIN))  // ���� ����� ����� double ��������� � int
			{
				value = static_cast<int>(result);
				return true;
			}

			// ������ ��������
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
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

			if (std::trunc(result) >= 0 && // ���� ��� ���������������  double
				result - std::trunc(result) < 0.00001 &&  // ����  double � ������� ������� ������
				std::trunc(result) <= UINT_MAX) // ����  double ��������� � unsigned int
			{
				value = static_cast<unsigned int>(result);
				return true;
			}

			// ������ ��������
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
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

			// ����  double � ������� ������� ������ � double �������������
			if ((std::trunc(result) >= 0 &&
				result - std::trunc(result) < 0.00001 &&
				std::trunc(result) <= LONG_MAX) ||

				// ����  double � ������� ������� ������ � double �������������
				(std::trunc(result) <= 0 &&  // ����  double �������������
				result - std::trunc(result) > -0.00001 &&   // ����  double � ������� ������� ������ 
				std::trunc(result) >= LONG_MIN))  // ���� ����� ����� double ��������� � long
			{
				value = static_cast<long>(result);
				return true;
			}

			// ������ ��������
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
			return false;
		}
		}
	}

	// ���� ������� ������ ���
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

			if (std::trunc(result) >= 0 && // ���� ��� ���������������  double
				result - std::trunc(result) < 0.00001 &&  // ����  double � ������� ������� ������
				std::trunc(result) <= ULONG_MAX) // ����  double ��������� � unsigned long
			{
				value = static_cast<unsigned long>(result);
				return true;
			}

			// ������ ��������
			return false;

		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
			return false;
		}
		}
	}

	// ���� ������� ������ ���
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

			// ����  double � ������� ������� ������ � double �������������
			if ((std::trunc(result) >= 0 && result - std::trunc(result) < 0.00001) ||
				// ����  double � ������� ������� ������ � double �������������
				(std::trunc(result) <= 0 && result - std::trunc(result) > -0.00001))
			{
				value = static_cast<long long>(result);
				return true;
			}

			// ������ ��������
			return false;
		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
			return false;
		}
		}
	}

	// ���� ������� ������ ���
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

			if (std::trunc(result) >= 0 && // ���� ��� ���������������  double
				result - std::trunc(result) < 0.00001) // ����  double � ������� ������� ������
			{
				value = static_cast<unsigned long long>(result);
				return true;
			}

			// ������ ��������
			return false;
		}
		case CIni::VALUE_TYPE::STRING:
		{
			// �������������� ����������
			return false;
		}
		}
	}

	// ���� ������� ������ ���
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
			// �������������� ����������
			return false;
		}
		}
	}

	// ���� ������� ������ ���
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
// ������� ����� CIni

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
		_access(m_file_full_path.c_str(), 0) == 0) // - 00 �������� ������ �� �������������
	{
		// ���� ����������
		return true;
	}
	else
	{
		// ���� �� ����������
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
	// ������� ���������� � ������ ����� ���� ������
	std::vector<std::string> sections_names;


	unsigned int result = 0;
	unsigned int sections_buffer_size = 1000; // ������ ������ ��� ��������
	char* symbols = new char[sections_buffer_size];


	do
	{
		if (result == sections_buffer_size - 2)
		{
			// ���� ������ �� ����� nullptr, 
			// ������ �������� �������� ��� ����� ������, 
			// � �� ������� ������� ������
			delete[] symbols;
			sections_buffer_size *= 2;

			symbols = new char[sections_buffer_size];
		}


		result = GetPrivateProfileSectionNames(
			symbols, // [out] LPTSTR  lpszReturnBuffer  // �����, ���� ����� �������� ������
			sections_buffer_size, // [in]  DWORD     // nSize,  ������ ������
			m_file_full_path.c_str() // [in]  LPCTSTR lpFileName     // ���� ini
		);
		/*
		������������ �������� ��������� ���������� ��������, ������������� � ��������� �����,
		�� ������� ����������� ������ NULL .
		���� ����� ������������ ����� ��� �������� ���� ���� ��������,
		��������� � ��������� ������ �������������,
		������������ �������� ����� �������, ��������� ���������� nSize ����� ���.
		*/



	} while (result == sections_buffer_size - 2);
	// ����� ������������ ����� ��� �������� ���� ���� ��������
	// ����� ���� � ����� do-while ���������� ����� ������ � ����� , ���� ��� ����� ������ �� ����������
	// � ����� �� ���������, � ��������� ���������� ��� ������� ������,
	//	���� ��� ������������� � 2 ����.


	std::string section;

	// �������� �� ���� ��������
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

	// �� ������ ������� � ���������� ������

	// ���� ������ ����  ������

	// ��������� �� ��������� ������ 
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

		section_iter; // ��� ����  ������ "������ ���� ������" - ������.
		section_iter->second; // ��� ������.   �� ������ ���� ����������� ������
		section_iter->first; //  ���� �� ������

		InitKeyValuePairs(&(section_iter->second));
	}
}

void
CIni::InitKeyValuePairs(Section* p_section)
{
	unsigned int result = 0;
	unsigned int keys_buffer_size = 1000; // ������ ������ ��� ��������
	char* symbols = new char[keys_buffer_size];

	do
	{
		if (result == keys_buffer_size - 2)
		{
			// ���� ������ �� ����� nullptr, 
			// ������ �������� �������� ��� ����� ������, 
			// � �� ������� ������� ������
			delete[] symbols;
			keys_buffer_size *= 2;

			symbols = new char[keys_buffer_size];
		}

		result = GetPrivateProfileString(
			p_section->section_path.c_str(), // [in]  LPCTSTR lpAppName,
			NULL, //[in]  LPCTSTR lpKeyName,
			NULL, // [in]  LPCTSTR lpDefault,
			symbols, // [out] LPTSTR  lpReturnedString,  // �����, ���� ����� ��� �������� �� ��� "����-��������" ��������� ������
			keys_buffer_size, //  [in]  DWORD   nSize,  // nSize,  ������ ������
			m_file_full_path.c_str() // [in]  LPCTSTR lpFileName     // ���� ini
		);
		/*
		���� �������� [in] lpKeyName ����� �������� NULL,
		��� ����� ������ � �������, ��������� ���������� lpAppName,
		���������� � �����, ��������� ���������� lpReturnedString
		*/

		/*
		���� lpKeyName ����� �������� NULL ,
		� ����� ���������� ������� ��� ��� �������� ���� �����,
		��������� ������ ��������� � �� ��� ������� ��� ������� NULL .
		� ���� ������ ������������ �������� ����� nSize ����� ���.
		*/


	} while (result == keys_buffer_size - 2);
	// ����� ������������ ����� ��� �������� ���� ���� ��������
	// ����� ���� � ����� do-while ���������� ����� ������ � ����� , ���� ��� ����� ������ �� ����������
	// � ����� �� ���������, � ��������� ���������� ��� ������� ������,
	//	���� ��� ������������� � 2 ����.


	// ����� ����� �������� �� ����� ������� ������ ������
	std::string key;

	// �������� �� ���� �������� ������
	for (int symbol_index = 0;
		symbol_index < static_cast<int>(result);
		++symbol_index)
	{
		// ���� �� ����� �� ����� ������ �����
		if (symbols[symbol_index] != NULL)
		{
			key += symbols[symbol_index];
		}
		// ��������� ������������ ������ �����
		else // if (symbols[i] == NULL)
		{

			int value_result = 0;
			int value_buffer_size = 200; // ������ ������ ��� ��������
			char* value_symbols = new char[value_buffer_size];

			do
			{
				if (result == value_buffer_size - 2)
				{
					// ���� ������ �� ����� nullptr, 
					// ������ �������� �������� ��� ����� ������, 
					// � �� ������� ������� ������
					delete[] symbols;
					value_buffer_size *= 2;

					value_symbols = new char[value_buffer_size];
				}

				value_result = GetPrivateProfileString(
					p_section->section_path.c_str(), // [in]  LPCTSTR lpAppName,
					key.c_str(), //[in]  LPCTSTR lpKeyName,
					NULL, // [in]  LPCTSTR lpDefault,
					value_symbols, // [out] LPTSTR  lpReturnedString,  // �����, ���� ����� ��� �������� �� ��� "����-��������" ��������� ������
					value_buffer_size, //  [in]  DWORD   nSize,  // nSize,  ������ ������
					m_file_full_path.c_str() // [in]  LPCTSTR lpFileName     // ���� ini
				);

			} while (value_result == value_buffer_size - 2);

			// ����� ������ �� ���������
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

				// �������� �� ������ ����� ��������� ?
				if (std::regex_match(value_str, int_rx))
				{
					// ����� �������������
					if (value_symbols[0] == '-')
					{
						// ����� ������� �������� �����
						long long number = std::stoll(std::string(value_symbols));

						AddPairToSection(p_section, key, number);

					}
					// ��������������� �����
					else
					{
						// ����� ������� ����������� �����
						unsigned long long number = std::stoull(std::string(value_symbols));

						AddPairToSection(p_section, key, number);

					}
				}
				// �������� �� ������ ������ � ������. ������ ?
				else if (std::regex_match(value_str, double_int_rx))
				{
					p_section->key_value_map.insert({ key, {VALUE_TYPE::DOUBLE, std::stod(value_str) } });
				}
				
				else
				{
					// ���� ������ ������� ������ �� ������ �������
					if (value_str.size() == 1)
						p_section->key_value_map.insert({ key, {VALUE_TYPE::CHAR, value_str[0] } });
					else if (value_str == "true")
						p_section->key_value_map.insert({ key, {VALUE_TYPE::BOOL, true } });
					else if (value_str == "false")
						p_section->key_value_map.insert({ key, {VALUE_TYPE::BOOL, false } });
					else // ��� ������ ������
						p_section->key_value_map.insert({ key, {VALUE_TYPE::STRING, value_str } });
				}

			}

			delete[] value_symbols;

			key.clear();
		}
	}
	delete[] symbols;

	// ��������� ���������
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
	// ��������� ���� ������� ������
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
	// ��������� ���� ������� ������
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
	// ���� :unordered_map � �������� ��� ����
	if (m_sections.empty())
	{
		m_sections.insert({ section_path, Section(section_path) });

	}
	else
	{
		// ��� �� ���������. ��� ������ �������� ������
		if (section_path.find('\\') == std::string::npos)
		{
			m_sections.insert({ section_path, Section(section_path) });
		}
		// ��� ���������
		else
		{
			// ��������������� ������ ���� ������������ ������
			auto pos = section_path.find_last_of('\\');
			std::string parent_section_path(section_path, 0, pos);

			// ����� ������������ ������ � �������� � ��� 
			Section* parent_section = FindSection(parent_section_path);

			// 3) �������� � ������������ ������ ����� ���������
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
	// ���� ������ �� �������� ����������
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
	// ���� ��� ���������
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
	// ���� ���� �������� ������ ������
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
	// ���� ���� �������� ������ ������
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
	// ���� ������ '\\' �� ������ � ������ ����
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	// �� ����������, ���� � ����� ������� ReadOnly
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
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				char value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				unsigned char value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				int value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				unsigned int value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const long& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const unsigned long& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const long long& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const unsigned long long& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const double& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool
CIni::WriteValue(const std::string& section_key_name,
				const std::string& value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return WriteValue(section_name, key_name, value);
}

bool 
CIni::WriteValue(const std::string& section_key_name,
				const char* value)
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

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

			// ���� ���� ���� ���� � �����-���� ������ �� ����������
			// ���������� false
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
	// ��������� ������ �� ���� �� ������ � ����

	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
						separator + 1, // ������� ������
						section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return SaveValue(section_name, key_name, value);
}

bool
CIni::SaveValue(const std::string& section_key_name,
				const char* value) const
{
	// ��������� ������ �� ���� �� ������ � ����

	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return SaveValue(section_name, key_name, value);
}

bool
CIni::SaveValue(const std::string& section_key_name,
				const std::string& value) const
{
	// ��������� ������ �� ���� �� ������ � ����

	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return SaveValue(section_name, key_name, value);
}

bool
CIni::SaveValue(const std::string& section_key_name,
				bool value) const
{
	// ��������� ������ �� ���� �� ������ � ����

	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return SaveValue(section_name, key_name, value);
}

void CIni::Section::Print() const
{
	for (auto iter = key_value_map.cbegin();
		iter != key_value_map.cend(); ++iter)
	{
		iter->first; // ����
		iter->second.first; // ���

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
			<< iter->first  // ���  ������
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
				  << iter->first  // ���  ������
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


// ������������� ������� ������ �������� � ����������
// ��� ������ �������� - ���� �� ������ ����������� � ������
bool
CIni::ReadValue(const std::string& section_key_name,
	bool& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	char& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	unsigned char& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	int& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	unsigned int& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	long& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	unsigned long& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	long long& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	unsigned long long& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	double& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

bool
CIni::ReadValue(const std::string& section_key_name,
	std::string& value) const
{
	auto separator = section_key_name.rfind('\\');

	std::string section_name(section_key_name, 0, separator);
	std::string key_name(section_key_name, // ����� ����� ���� ������
		separator + 1, // ������� ������
		section_key_name.size() - section_name.size() - 1); // ���������� ��������

	return ReadValue(section_name, key_name, value);
}

/*
CIni::Section::Section(const std::string& section_path,
						std::string& file) :
						section_path(section_path),
						ref_file_full_path(file)
{
	// �� ������� ���� ������ ��������� path ������������ ������

	std::istringstream f(section_path); // ��������� ���� ������� ������
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


// �������� ������ AddSection() � InitSectionsNames()
CIni::Section*
CIni::CreateSection(const std::string& section_path)
{
	// ���� :unordered_map � �������� ��� ����
	if (m_sections.empty())
	{
		m_sections.insert({ section_path, Section(section_path, m_file_full_path) });

	}
	else
	{
		// ��� �� ���������. ��� ������ �������� ������
		if (section_path.find('\\') == std::string::npos)
		{
			m_sections.insert({ section_path, Section(section_path, m_file_full_path) });
		}
		// ��� ���������
		else
		{
			// 1) ��������������� ������ ���� ������������ ������
			auto pos = section_path.find_last_of('\\');
			std::string parent_section_path(section_path, 0, pos);

			// 2) ����� ������������ ������ � �������� � ���
			Section* parent_section = FindSection(parent_section_path);


			// 3) ���� ������������ ���� ��� � ���������,
			// ��������� ������� ������������ ������, ����� � ��� ��������� ������ ���������
			if (parent_section == nullptr)
			{
				// ��������� ���� ������� ������
				std::vector<std::string> section_path_parts;

				std::istringstream f(section_path);
				std::string s;

				while (getline(f, s, '\\'))
					section_path_parts.push_back(s);

				std::string parent_section_path;

				// ��������� ������� ����������� ������������ ������ � ���������
				for (int i = 0; i < static_cast<int>(section_path_parts.size()); ++i)
				{
					if (parent_section_path.empty())
						parent_section_path = section_path_parts[0];
					else
						parent_section_path += "\\" + section_path_parts[i];

					if (!SectionExists(parent_section_path))
						CreateSection(parent_section_path);
				}

				// ����� ��������� � ������������ ������������ ������ ������ ���������
				parent_section = FindSection(parent_section_path);
			}

			// 4) �������� � ������������ ������ ����� ���������
			parent_section->sections_map.insert({ section_path, Section(section_path, m_file_full_path) });
		}
	}

	auto p_section = FindSection(section_path);
	p_section->ref_file_full_path = m_file_full_path;

	return p_section;

	return FindSection(section_path);
}
*/