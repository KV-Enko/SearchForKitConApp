#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

struct DocItem
{
	int position;
	int quantity;
	std::vector<std::string> dirs;
};

struct KitItem
{
	std::string dir;
	int quantity;
};

std::vector<DocItem> ReadDocument(const std::string& filename)
{
	if (filename.empty())
	{
		std::wcerr << L"Ошибка: Пустое название файла документа" << std::endl;
		return {};
	}

	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::wcerr << L"Ошибка: Не удалось открыть файл " << filename.c_str() << std::endl;
		return {};
	}

	std::vector<DocItem> docitems;

	// Go through each line of the document
	std::string line;
	bool firstline = true;
	while (std::getline(file, line))
	{
		if (firstline)
		{
			firstline = false;
			continue; // Skip header string
		}

		if (line.empty())
			continue; // Skip empty lines

		std::istringstream iss(line);

		DocItem item;
		if (!(iss >> item.position >> item.quantity))
		{
			std::wcout << L"Неверный формат строки под номером: " << line.c_str() << " в файле " << filename.c_str() << std::endl;
			continue;
		}

		// Parse directories
		std::string dir;
		while (iss >> dir)
		{
			item.dirs.push_back(dir);
		}

		docitems.push_back(item);
	}

	return docitems;
}

std::vector<KitItem> ReadKit(const std::string& filename)
{
	if (filename.empty())
	{
		std::wcerr << L"Ошибка: Пустое название файла наборов." << std::endl;
		return {};
	}

	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::wcerr << L"Ошибка: Не удалось открыть файл " << filename.c_str() << std::endl;
		return {};
	}

	std::vector<KitItem> kititems;

	// Go through each line of the document
	std::string line;
	bool firstline = true;
	while (std::getline(file, line))
	{
		if (firstline)
		{
			firstline = false;
			continue; // Skip header string
		}

		if (line.empty())
			continue; // Skip empty lines

		std::istringstream iss(line);

		KitItem item;
		if (!(iss >> item.dir >> item.quantity))
		{
			std::wcout << L"Неверный формат строки под номером: " << line.c_str() << " в файле " << filename.c_str() << std::endl;
			continue;
		}

		kititems.push_back(item);
	}

	return kititems;
}

bool CheckKitInDocument(const std::vector<DocItem>& docitems, const std::vector<KitItem>& kititems, std::map<int, std::unordered_map<std::string, int>>& result)
{
	// Create a map of required items
	std::unordered_map<std::string, int> required;
	for (const KitItem& kit : kititems)
	{
		required[kit.dir] = kit.quantity;
	}

	// Go through all items in the document
	for (const DocItem& item : docitems)
	{
		// A single item can have a few dirs
		for (const std::string& dir : item.dirs)
		{
			// See if the dir really presents in required
			if (required.find(dir) != required.end() && required[dir] > 0)
			{
				int taken = std::min(item.quantity, required[dir]);
				result[item.position][dir] = taken;
				required[dir] -= taken;

				if (required[dir] == 0)
				{
					required.erase(dir);
				}
			}
		}
	}

	for (const auto& req : required)
	{
		if (req.second > 0)
		{
			return false;
		}
	}

	return true;
}

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WIN64)
	setlocale(LC_ALL, "Russian");
#else
	setlocale(LC_ALL, "ru_RU.UTF-8");
	chdir("..");
#endif

	const char* const DOCUMENT_FILENAME = "document.txt";
	const char* const KIT_FILENAME = "kit.txt";

	std::vector<DocItem> docitems = ReadDocument(DOCUMENT_FILENAME);
	if (docitems.empty())
	{
		std::wcout << L"Не найдены записи в документе " << DOCUMENT_FILENAME << std::endl;
		return 1;
	}

	std::vector<KitItem> kititems = ReadKit(KIT_FILENAME);
	if (kititems.empty())
	{
		std::wcout << L"Не найдены записи в документе " << DOCUMENT_FILENAME << std::endl;
		return 1;
	}

	std::map<int, std::unordered_map<std::string, int>> useddocitems;
	if (CheckKitInDocument(docitems, kititems, useddocitems))
	{
		std::wcout << L"Набор содержится в документе" << std::endl;
		std::wcout << L"Состав набора: " << std::endl;

		for (const auto& item: useddocitems)
		{
			for (const auto& dir : item.second)
			{
				std::wcout << item.first << '\t' << dir.second << '\t' << std::wstring(dir.first.begin(), dir.first.end()) << std::endl;
			}
		}
	}
	else
	{
		std::wcout << L"Набор не входит в документ" << std::endl;
	}

	return 0;
}