#pragma once

#include <any>
#include <sstream>
#include <string>
#include <unordered_map>
#include <windows.h>



class CIni
{
public:
	//! для определения типа
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
		//!  название родительской секции
		std::string parent_section;

		//!   название секции
		std::string section_path;

		//! вектор строк составных частей пути секции
		std::vector<std::string> section_path_parts;



		//! мапа "ключ - значение"
		std::unordered_map<std::string, std::pair<VALUE_TYPE, std::any>> key_value_map;

		//! мапа секций
		std::unordered_map<std::string, Section> sections_map;

		//! конструктор секции
		Section(const std::string& section_path);

		//! деструктор секции
		~Section();


		//! содержит ли секция пару с даным ключом ?
		bool ContainsKey(const std::string& key_name) const;

		//! содержит ли секция такую подсекцию ?
		bool ContainsSubSection(const std::string& section_name) const;

		//! есть ли у секции подсекции ?
		bool ContainsSubSections() const;

		//! добавить подсекцию в секцию
		CIni::Section* AddSection(const std::string& section_name);


		//! функция возвращает искомое значение: 
		//! указатель на значение и тип значения
		const std::pair<VALUE_TYPE, void*>&
			ReadValue(const std::string& key_name) const;


		//! записать секции в ini файл
		bool WriteToIni(const std::string& m_full_file_path) const;

		//! функция определяет, содержат ли вектора одинаковые части путей
		static bool HasEqualPathParts(const std::vector<std::string>& path_parts_1,
									const std::vector<std::string>& path_parts_2);

		//! функция прохождения по структуре секции: 
		//! по мапе ключ-значение и по мапе подсекций
		void Print() const;

		//! найти подсекцию. const перегрузка
		const CIni::Section*
			FindSection(const std::vector<std::string>& section_path_parts) const;

		//! найти подсекцию. не const перегрузка
		CIni::Section*
			FindSection(const std::vector<std::string>& section_path_parts);

		//! функция для поиска секции
		Section*
			FindSection(const std::string section_path);

		//! const функция для поиска секции
		const Section*
			FindSection(const std::string section_path) const;

		// перегруженные функции записи значения в мапу "ключ-значение"
		// true если записи под таким ключом не было ; 
		// false если уже была запись под данным ключом. Перезапись

		//! запись bool значения по заданному ключу
		void WriteValue(const std::string& key_name,
						bool value);
		//! запись char значения по заданному ключу
		void WriteValue(const std::string& key_name,
						char value);
		//! запись unsigned char значения по заданному ключу
		void WriteValue(const std::string& key_name,
						unsigned char value);
		//! запись int значения по заданному ключу
		void WriteValue(const std::string& key_name,
						int value);
		//! запись unsigned int значения по заданному ключу
		void WriteValue(const std::string& key_name,
						unsigned int value);
		//! запись long значения по заданному ключу
		void WriteValue(const std::string& key_name,
						const long& value);
		//! запись unsigned long значения по заданному ключу
		void WriteValue(const std::string& key_name,
						const unsigned long& value);
		//! запись long long значения по заданному ключу
		void WriteValue(const std::string& key_name,
						const long long& value);
		//! запись unsigned long long значения по заданному ключу
		void WriteValue(const std::string& key_name,
						const unsigned long long& value);
		//! запись double значения по заданному ключу
		void WriteValue(const std::string& key_name,
						const double& value);
		//! запись std::string значения по заданному ключу
		void WriteValue(const std::string& key_name,
						const std::string& value);
		//! запись стркоки в виде const char* по заданному ключу
		//! использовать  static_cast<const char*> при передаче в функцию
		void WriteValue(const std::string& key_name,
						const char* value);

		// перегруженные функции чтения значения в переменную

		//! чтение bool значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   bool& value) const;
		//! чтение int значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   char& value) const;
		//! чтение unsigned int значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   unsigned char& value) const;
		//! чтение int значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   int& value) const;
		//! чтение unsigned int значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   unsigned int& value) const;
		//! чтение long значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   long& value) const;
		//! чтение long значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   unsigned long& value) const;
		//! чтение long long значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   long long& value) const;
		//! чтение unsigned long long значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   unsigned long long& value) const;
		//! чтение double значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   double& value) const;
		//! чтение std::string значения по заданному ключу
		bool ReadValue(const std::string& key_name,
					   std::string& value) const;
	};

	//! полный путь до ini файла
	std::string m_file_full_path;

	//! мапа секций
	std::unordered_map<std::string, Section> m_sections;

	//! сменилось ли хоть одно значение в паре ключ-значение ?
	bool is_changed = false;


	// вспомогательые перегруженные функции записи в мапу значения
	// 
	//! добавить значение в мапу
	void AddPairToSection(Section* p_section, 
						  const std::string& key, 
						  const long long& number);

	//! добавить значение в мапу
	void AddPairToSection(Section* p_section,
						  const std::string& key,
						  const unsigned long long& number);


	//! вспомогательная рекурсивная функция для поиска секции
	Section*
		FindSection(std::unordered_map<std::string, Section>& some_map,
					const std::vector<std::string>& section_path_parts);

	//! функция для поиска секции
	Section*
		FindSection(const std::vector<std::string>& section_path_parts);

	//! вспомогательная рекурсивная функция для поиска секции
	const Section*
		FindSection(const std::unordered_map<std::string, Section>& some_map,
					const std::vector<std::string>& section_path_parts) const;

	//! функция для поиска секции
	const Section*
		FindSection(const std::vector<std::string>& section_path_parts) const;

	//! функция построения структуры мапы секций
	void InitSectionsNames();
	//! функция добавления в мапу пар ключ-значение
	void InitKeyValuePairs();
	//! рекурсивная функция добавления в мапу пар ключ-значение
	void InitKeyValuePairs(Section* p_section);
public:
	CIni() {}
	CIni(const std::string& str);
	~CIni() {}

	//! проверка: пустая ли мапа секций
	bool IsEmpty() const;

	//! есть ли файл в наличии
	bool FileExists() const;

	//! инициализация мапы. функция используется, 
	//! когда при отработке конструктора не был установлен файл
	void InitCIni();

	//!  есть ли у файла атрибут ReadOnly ?
	bool IsReadOnly() const;

	

	//! функция для поиска секции
	Section*
		FindSection(const std::string section_path);

	//! const функция для поиска секции
	const Section*
		FindSection(const std::string section_path) const;

	//! добавим секцию с таким именем, если ее не было
	void AddSection(const std::string& section_path);


	//! есть ли в ini файле такая секция ?
	bool SectionExists(const std::string& section) const;


	/// \brief   есть ли в ini файле данные под таким ключом ?
	/// \details   просматривает только указанную секцию
	bool KeyExists(const std::string& section_path,
					const std::string& key) const;


	//!  функция возврата мапы с подсекциями
	std::unordered_map<std::string, Section>&
		GetSectionMap(const std::string section_path);

	//!  функция возврата мапы с подсекциями
	const std::unordered_map<std::string, Section>&
		GetSectionMap(const std::string section_path) const;

	//!  функция возврата содержимого секции
	std::unordered_map<std::string, std::pair<VALUE_TYPE, std::any>>&
		GetKeyValueSectionMap(const std::string section_path);

	//!  функция возврата содержимого секции
	const std::unordered_map<std::string, std::pair<VALUE_TYPE, std::any>>&
		GetKeyValueSectionMap(const std::string section_path) const;


	//! сколько секций в данном ini файле?
	unsigned int SectionsCount() const;

	//! функция возвращает количество записей в указанной секции
	unsigned int KeysCount(const std::string& section_path) const;


	//! установить путь до файла
	void SetFullFilePath(const std::string& str);

	//! запись всей структуры в ini файл
	bool WriteToIni() const;



	// перегруженные функции записи в ini файл

	//! записать секцию и ключ-значение в ini файл. минуя запись в мапу
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

	//! записать секцию и ключ-значение  в ini файл. минуя запись в мапу
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					char value) const;

	//! записать секцию и ключ-значение  в ini файл. минуя запись в мапу
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					const char* value) const;

	//! записать секцию и ключ-значение  в ini файл. минуя запись в мапу
	//bool SaveValue(const std::string& section_name,
	//				const std::string& key_name,
	//				const CString& value) const;

	//! записать секцию и ключ-значение  в ini файл. минуя запись в мапу
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					const std::string& value) const;


	//! записать секцию и ключ-значение в ini файл. минуя запись в мапу
	bool SaveValue(const std::string& section_name,
					const std::string& key_name,
					bool value) const;

	// перегруженные функции записи значения в ini файл по заданному пути.

	//! записать секцию и ключ-значение в ini файл. минуя запись в мапу
	template <typename T>
	bool SaveValue(const std::string& section_key_name,
					T value) const
	{
		// разделяем строку на путь до секции и ключ

		auto separator = section_key_name.rfind('\\');

		std::string section_name(section_key_name, 0, separator);
		std::string key_name(section_key_name, // берем часть этой строки
							separator + 1, // позиция начала
							section_key_name.size() - section_name.size() - 1); // количество символов


		std::string str = std::to_string(value);

		return WritePrivateProfileString(
			section_name.c_str(),
			key_name.c_str(),
			str.c_str(),
			m_file_full_path.c_str());
	}

	//! записать секцию и ключ-значение  в ini файл. минуя запись в мапу
	bool SaveValue(const std::string& section_key_name,
					char value) const;

	//! записать секцию и ключ-значение  в ini файл. минуя запись в мапу
	bool SaveValue(const std::string& section_key_name,
					const char* value) const;

	//! записать секцию и ключ-значение  в ini файл. минуя запись в мапу
	//bool SaveValue(const std::string& section_key_name,
	//				const CString& value) const;

	//! записать секцию и ключ-значение  в ini файл. минуя запись в мапу
	bool SaveValue(const std::string& section_key_name,
					const std::string& value) const;


	//! записать секцию и ключ-значение в ini файл. минуя запись в мапу
	bool SaveValue(const std::string& section_key_name,
					bool value) const;


	//! функция прохождения по всей структуре класса: 
	//! по всем парам ключ-значение и по всем секциям и подсекциям
	//! используется в целях отладки
	void Print() const;



	// перегруженные функции записи значения по заданному ключу в указанную секцию
	// bool - получилось записать или нет:
	// false - нет секции или у файла атрибут ReadOnly

	//! запись bool значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					bool value);
	//! запись char значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					char value);
	//! запись unsigned char значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					unsigned char value);
	//! запись int значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					int value);
	//! запись unsigned int значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					unsigned int value);
	//! запись long значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const long& value);
	//! запись unsigned long значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const unsigned long& value);
	//! запись long long значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const long long& value);
	//! запись unsigned long long значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const unsigned long long& value);
	//! запись double значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const double& value);
	//! запись std::string значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const std::string& value);
	//! запись стркоки в виде const char* по заданному ключу
	//! использовать  static_cast<const char*> при передаче в функцию
	bool WriteValue(const std::string& section_name,
					const std::string& key_name,
					const char* value);


	// перегруженные функции записи значения в CIni
	// где первый аргумент - путь до секции совмещенный с ключом
	
	//! запись bool значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					bool value);
	//! запись char значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					char value);
	//! запись unsigned char значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					unsigned char value);
	//! запись int значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					int value);
	//! запись unsigned int значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					unsigned int value);
	//! запись long значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					const long& value);
	//! запись unsigned long значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					const unsigned long& value);
	//! запись long long значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					const long long& value);
	//! запись unsigned long long значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					const unsigned long long& value);
	//! запись double значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					const double& value);
	//! запись std::string значения по заданному ключу в указанную секцию
	bool WriteValue(const std::string& section_key_name,
					const std::string& value);
	//! запись стркоки в виде const char* по заданному ключу
	//! использовать  static_cast<const char*> при передаче в функцию
	bool WriteValue(const std::string& section_key_name,
					const char* value);

	// перегруженные функции чтения значения в переменную

	//! чтение bool значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					bool& value) const;
	//! чтение char значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					char& value) const;

	//! чтение unsigned char значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					unsigned char& value) const;

	//! чтение int значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					int& value) const;

	//! чтение unsigned int значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					unsigned int& value) const;

	//! чтение long значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					long& value) const;

	//! чтение unsigned long значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					unsigned long& value) const;

	//! чтение long long значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					long long& value) const;

	//! чтение unsigned long long значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					unsigned long long& value) const;

	//! чтение double значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					double& value) const;

	//! чтение std::string значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_name,
					const std::string& key_name,
					std::string& value) const;

	// перегруженные функции чтения значения в переменную
	// где первый аргумент - путь до секции совмещенный с ключом

	//! чтение bool значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					bool& value) const;

	//! чтение char значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					char& value) const;

	//! чтение unsigned char значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					unsigned char& value) const;

	//! чтение int значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					int& value) const;

	//! чтение unsigned int значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					unsigned int& value) const;

	//! чтение long значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					long& value) const;

	//! чтение unsigned long значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					unsigned long& value) const;

	//! чтение long long значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					long long& value) const;

	//! чтение unsigned long long значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					unsigned long long& value) const;

	//! чтение double значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					double& value) const;

	//! чтение std::string значения по заданному ключу из указанной секции
	bool ReadValue(const std::string& section_key_name,
					std::string& value) const;
};


