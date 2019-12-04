#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iomanip>

auto hello()
{
    std::cout << "The broker app. Read JSON file and processes it\n";
    std::cout << "Enter input filename: ";

    std::string input;
    std::getline(std::cin, input);

    std::cout << "Enter output filename: ";

    std::string output;
    std::getline(std::cin, output);

    return std::make_pair(input, output);
}

int main()
{
    auto fileInfo = hello();

    // Prepare input file
    std::ifstream input(fileInfo.first);
    if (input.fail()) {
        std::cout << "File error\n";
        return 1;
    }

    // Serialize
    nlohmann::json json;
    input >> json;

    // Проверка
    if (!json.is_array()) {
        std::cout << "Input JSON is not an array\n";
        return 1;
    }
    if (json.size() != 3) {
        std::cout << "Input JSON array size is not 3\n";
        return 1;
    }
    if (!(json[0].size() == json[1].size() && json[1].size() == json[2].size())) {
        std::cout << "Input JSON nested array sizes are not equal\n";
        return 1;
    }

    // Processing
    nlohmann::json outputJson;
    for (size_t i = 0; i < json[0].size(); i++) {
        outputJson.push_back(
            nlohmann::json{
                {"ticker", json[0][i]},
                {"id", json[1][i]},
                {"description", json[2][i]},
            }
        );
    }

    // Подготовка выходного файла
    std::ofstream output(fileInfo.second);
    if (output.fail()) {
        std::cout << "File error\n";
        return 1;
    }

    // Вывод
    output << std::setw(4) << outputJson << std::endl;
}