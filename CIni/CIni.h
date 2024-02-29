#pragma once

#include <any>
#include <sstream>
#include <string>
#include <unordered_map>
#include <windows.h>



class CIni
{
public:
	//! ��� ����������� ����
	enum class VALUE_TYPE
	{
		BOOL,
		CHAR,
		UCHAR,
		INT,
		UINT,
		LONG,
		ULONG,
		LONGLONG,
		ULONGLONG,
		DOUBLE,
		STRING,
		NO
	};


private:

	struct Section
	{
		//!  �������� ������������ ������
		std::string parent_section;

		//!   �������� ������
		std::string section_path;

		//! ������ ����� ��������� ������ ���� ������
		std::vector<std::string> section_path_parts;



		//! ���� "���� - ��������"
		std::unordered_map<std::string, std::pair<VALUE_TYPE, std::any>> key_value_map;

		//! ���� ������
		std::unordered_map<std::string, Section> sections_map;

		//! ����������� ������
		Section(const std::string& section_path);

		//! ���������� ������
		~Section();


		//! �������� �� ������ ���� � ����� ������ ?
		bool ContainsKey(const std::string& key_name) const;

		//! �������� �� ������ ����� ��������� ?
		bool ContainsSubSection(const std::string& section_name) const;

		//! ���� �� � ������ ��������� ?
		bool ContainsSubSections() const;

		//! �������� ��������� � ������
		CIni::Section* AddSection(const std::string& section_name);


		//! ������� ���������� ������� ��������: 
		//! ��������� �� �������� � ��� ��������
		const std::pair<VALUE_TYPE, void*>&
			ReadValue(const std::string& key_name) const;


		//! �������� ������ � ini ����
		bool WriteToIni(const std::string& m_full_file_path) const;

		//! ������� ����������, �������� �� ������� ���������� ����� �����
		static bool HasEqualPathParts(const std::vector<std::string>& path_parts_1,
									const std::vector<std::string>& path_parts_2);

		//! ������� ����������� �� ��������� ������: 
		//! �� ���� ����-�������� � �� ���� ���������
		void Print() const;

		//! ����� ���������. const ����������
		const CIni::Section*
			FindSection(const std::vector<std::string>& section_path_parts) const;

		//! ����� ���������. �� const ����������
		CIni::Section*
			FindSection(const std::vector<std::string>& section_path_parts);

		//! ������� ��� ������ ������
		Section*
			FindSection(const std::string section_path);

		//! const ������� ��� ������ ������
		const Section*
			FindSection(const std::string section_path) const;

		// ������������� ������� ������ �������� � ���� "����-��������"
		// true ���� ������ ��� ����� ������ �� ���� ; 
		// false ���� ��� ���� ������ ��� ������ ������. ����������

		//! ������ bool �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						bool value);
		//! ������ char �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						char value);
		//! ������ unsigned char �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						unsigned char value);
		//! ������ int �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						int value);
		//! ������ unsigned int �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						unsigned int value);
		//! ������ long �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						const long& value);
		//! ������ unsigned long �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						const unsigned long& value);
		//! ������ long long �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						const long long& value);
		//! ������ unsigned long long �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						const unsigned long long& value);
		//! ������ double �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						const double& value);
		//! ������ std::string �������� �� ��������� �����
		void WriteValue(const std::string& key_name,
						const std::string& value);
		//! ������ ������� � ���� const char* �� ��������� �����
		//! ������������  static_cast<const char*> ��� �������� � �������
		void WriteValue(const std::string& key_name,
						const char* value);

		// ������������� ������� ������ �������� � ����������

		//! ������ bool �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   bool& value) const;
		//! ������ int �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   char& value) const;
		//! ������ unsigned int �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   unsigned char& value) const;
		//! ������ int �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   int& value) const;
		//! ������ unsigned int �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   unsigned int& value) const;
		//! ������ long �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   long& value) const;
		//! ������ long �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   unsigned long& value) const;
		//! ������ long long �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   long long& value) const;
		//! ������ unsigned long long �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   unsigned long long& value) const;
		//! ������ double �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   double& value) const;
		//! ������ std::string �������� �� ��������� �����
		bool ReadValue(const std::string& key_name,
					   std::string& value) const;
	};

	//! ������ ���� �� ini �����
	std::string m_file_full_path;

	//! ���� ������
	std::unordered_map<std::string, Section> m_sections;

	//! ��������� �� ���� ���� �������� � ���� ����-�������� ?
	bool is_changed = false;


	// �������������� ������������� ������� ������ � ���� ��������
	// 
	//! �������� �������� � ����
	void AddPairToSection(Section* p_section, 
						  const std::string& key, 
						  const long long& number);

	//! �������� �������� � ����
	void AddPairToSection(Section* p_section,
						  const std::string& key,
						  const unsigned long long& number);


	//! ��������������� ����������� ������� ��� ������ ������
	Section*
		FindSection(std::unordered_map<std::string, Section>& some_map,
					const std::vector<std::string>& section_path_parts);

	//! ������� ��� ������ ������
	Section*
		FindSection(const std::vector<std::string>& section_path_parts);

	//! ��������������� ����������� ������� ��� ������ ������
	const Section*
		FindSection(const std::unordered_map<std::string, Section>& some_map,
					const std::vector<std::string>& section_path_parts) const;

	//! ������� ��� ������ ������
	const Section*
		FindSection(const std::vector<std::string>& section_path_parts) const;

	//! ������� ���������� ��������� ���� ������
	void InitSectionsNames();
	//! ������� ���������� � ���� ��� ����-��������
	void InitKeyValuePairs();
	//! ����������� ������� ���������� � ���� ��� ����-��������
	void InitKeyValuePairs(Section* p_section);
public:
	CIni() {}
	CIni(const std::string& str);
	~CIni() {}

	//! ��������: ������ �� ���� ������
	bool IsEmpty() const;

	//! ���� �� ���� � �������
	bool FileExists() const;

	//! ������������� ����. ������� ������������, 
	//! ����� ��� ��������� ������������ �� ��� ���������� ����
	void InitCIni();

	//!  ���� �� � ����� ������� ReadOnly ?
	bool IsReadOnly() const;

	

	//! ������� ��� ������ ������
	Section*
		FindSection(const std::string section_path);

	//! const ������� ��� ������ ������
	const Section*
		FindSection(const std::string section_path) const;

	//! ������� ������ � ����� ������, ���� �� �� ����
	void AddSection(const std::string& section_path);


	//! ���� �� � ini ����� ����� ������ ?
	bool SectionExists(const std::string& section) const;


	/// \brief   ���� �� � ini ����� ������ ��� ����� ������ ?
	/// \details   ������������� ������ ��������� ������
	bool KeyExists(const std::string& section_path,
					const std::string& key) const;


	//!  ������� �������� ���� � �����������
	std::unordered_map<std::string, Section>&
		GetSectionMap(const std::string section_path);

	//!  ������� �������� ���� � �����������
	const std::unordered_map<std::string, Section>&
		GetSectionMap(const std::string section_path) const;

	//!  ������� �������� ����������� ������
	std::unordered_map<std::string, std::pair<VALUE_TYPE, std::any>>&
		GetKeyValueSectionMap(const std::string section_path);

	//!  ������� �������� ����������� ������
	const std::unordered_map<std::string, std::pair<VALUE_TYPE, std::any>>&
		GetKeyValueSectionMap(const std::string section_path) const;


	//! ������� ������ � ������ ini �����?
	unsigned int SectionsCount() const;

	//! ������� ���������� ���������� ������� � ��������� ������
	unsigned int KeysCount(const std::string& section_path) const;


	//! ���������� ���� �� �����
	void SetFullFilePath(const std::string& str);

	//! ������ ���� ��������� � ini ����
	bool WriteToIni() const;



	// ������������� ������� ������ � ini ����

	//! �������� ������ � ����-�������� � ini ����. ����� ������ � ����
	template <typename T>
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					T value) const
	{
		std::string str = std::to_string(value);

		return WritePrivateProfileString(
			section_name.c_str(),
			key_name.c_str(),
			str.c_str(),
			m_file_full_path.c_str());
	}

	//! �������� ������ � ����-��������  � ini ����. ����� ������ � ����
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					char value) const;

	//! �������� ������ � ����-��������  � ini ����. ����� ������ � ����
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					const char* value) const;

	//! �������� ������ � ����-��������  � ini ����. ����� ������ � ����
	//bool SaveValue(const std::string& section_name,
	//				const std::string& key_name,
	//				const CString& value) const;

	//! �������� ������ � ����-��������  � ini ����. ����� ������ � ����
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					const std::string& value) const;


	//! �������� ������ � ����-�������� � ini ����. ����� ������ � ����
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					bool value) const;

	// ������������� ������� ������ �������� � ini ���� �� ��������� ����.

	//! �������� ������ � ����-�������� � ini ����. ����� ������ � ����
	template <typename T>
	bool SaveValue(const std::string& section_key_name,
					T value) const
	{
		// ��������� ������ �� ���� �� ������ � ����

		auto separator = section_key_name.rfind('\\');

		std::string section_name(section_key_name, 0, separator);
		std::string key_name(section_key_name, // ����� ����� ���� ������
							separator + 1, // ������� ������
							section_key_name.size() - section_name.size() - 1); // ���������� ��������


		std::string str = std::to_string(value);

		return WritePrivateProfileString(
			section_name.c_str(),
			key_name.c_str(),
			str.c_str(),
			m_file_full_path.c_str());
	}

	//! �������� ������ � ����-��������  � ini ����. ����� ������ � ����
	bool SaveValue(const std::string& section_key_name,
					char value) const;

	//! �������� ������ � ����-��������  � ini ����. ����� ������ � ����
	bool SaveValue(const std::string& section_key_name,
					const char* value) const;

	//! �������� ������ � ����-��������  � ini ����. ����� ������ � ����
	//bool SaveValue(const std::string& section_key_name,
	//				const CString& value) const;

	//! �������� ������ � ����-��������  � ini ����. ����� ������ � ����
	bool SaveValue(const std::string& section_key_name,
					const std::string& value) const;


	//! �������� ������ � ����-�������� � ini ����. ����� ������ � ����
	bool SaveValue(const std::string& section_key_name,
					bool value) const;


	//! ������� ����������� �� ���� ��������� ������: 
	//! �� ���� ����� ����-�������� � �� ���� ������� � ����������
	//! ������������ � ����� �������
	void Print() const;



	// ������������� ������� ������ �������� �� ��������� ����� � ��������� ������
	// bool - ���������� �������� ��� ���:
	// false - ��� ������ ��� � ����� ������� ReadOnly

	//! ������ bool �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					bool value);
	//! ������ char �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					char value);
	//! ������ unsigned char �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					unsigned char value);
	//! ������ int �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					int value);
	//! ������ unsigned int �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					unsigned int value);
	//! ������ long �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const long& value);
	//! ������ unsigned long �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const unsigned long& value);
	//! ������ long long �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const long long& value);
	//! ������ unsigned long long �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const unsigned long long& value);
	//! ������ double �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const double& value);
	//! ������ std::string �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const std::string& value);
	//! ������ ������� � ���� const char* �� ��������� �����
	//! ������������  static_cast<const char*> ��� �������� � �������
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const char* value);


	// ������������� ������� ������ �������� � CIni
	// ��� ������ �������� - ���� �� ������ ����������� � ������
	
	//! ������ bool �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					bool value);
	//! ������ char �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					char value);
	//! ������ unsigned char �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					unsigned char value);
	//! ������ int �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					int value);
	//! ������ unsigned int �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					unsigned int value);
	//! ������ long �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					const long& value);
	//! ������ unsigned long �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					const unsigned long& value);
	//! ������ long long �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					const long long& value);
	//! ������ unsigned long long �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					const unsigned long long& value);
	//! ������ double �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					const double& value);
	//! ������ std::string �������� �� ��������� ����� � ��������� ������
	bool WriteValue(const std::string& section_key_name,
					const std::string& value);
	//! ������ ������� � ���� const char* �� ��������� �����
	//! ������������  static_cast<const char*> ��� �������� � �������
	bool WriteValue(const std::string& section_key_name,
					const char* value);

	// ������������� ������� ������ �������� � ����������

	//! ������ bool �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					bool& value) const;
	//! ������ char �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					char& value) const;

	//! ������ unsigned char �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					unsigned char& value) const;

	//! ������ int �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					int& value) const;

	//! ������ unsigned int �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					unsigned int& value) const;

	//! ������ long �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					long& value) const;

	//! ������ unsigned long �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					unsigned long& value) const;

	//! ������ long long �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					long long& value) const;

	//! ������ unsigned long long �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					unsigned long long& value) const;

	//! ������ double �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					double& value) const;

	//! ������ std::string �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					std::string& value) const;

	// ������������� ������� ������ �������� � ����������
	// ��� ������ �������� - ���� �� ������ ����������� � ������

	//! ������ bool �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					bool& value) const;

	//! ������ char �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					char& value) const;

	//! ������ unsigned char �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					unsigned char& value) const;

	//! ������ int �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					int& value) const;

	//! ������ unsigned int �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					unsigned int& value) const;

	//! ������ long �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					long& value) const;

	//! ������ unsigned long �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					unsigned long& value) const;

	//! ������ long long �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					long long& value) const;

	//! ������ unsigned long long �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					unsigned long long& value) const;

	//! ������ double �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					double& value) const;

	//! ������ std::string �������� �� ��������� ����� �� ��������� ������
	bool ReadValue(const std::string& section_key_name,
					std::string& value) const;
};


