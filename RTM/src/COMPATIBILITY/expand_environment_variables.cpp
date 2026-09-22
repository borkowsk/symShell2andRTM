/// @file
/// @brief Implementacja funkcji, która rozwija w stringu zmienne systemowe oraz '~' jako równoważnik $HOME
/// @date 2026-09-22 (modified)
/// @details
///     Składnia ${VAR}: Użycie nawiasów klamrowych zapobiega chciwemu dopasowywaniu nazwy zmiennej,
///     co przydaje się, gdy bezpośrednio po zmiennej następują inne znaki (np. ${VAR}_file.txt).
///
///     Brakujące zmienne: Jeśli system nie znajdzie danej zmiennej środowiskowej, kod w tym wariancie
///     pozostawia nienaruszone $VAR (zamiast zastępować je pustym ciągiem), co ułatwia diagnozowanie błędów.
///
/// @note Kod zgodny w 100% z C++11 bez żadnych dodatkowych bibliotek

#include <iostream>
#include <string>
#include <cstdlib>
#include <regex>

/// @brief Rozwija w stringu zmienne systemowe i '~' jako równoważnik $HOME
std::string expand_environment_variables(const std::string& input_path)
{
    if (input_path.empty())
    {
        return "";
    }

    std::string path_str = input_path;

    // 1. Obsługa tyldy (~)
    if (path_str[0] == '~')
    {
        const char* home = std::getenv("HOME");
        if (home)
        {
            path_str.replace(0, 1, home);
        }
    }

    // 2. Dopasowanie $VAR oraz ${VAR}
    // Grupa 1 dopasowuje nazwę w ${VAR}, Grupa 2 w $VAR
    static const std::regex env_var_regex(R"(\$\{([A-Za-z0-9_]+)\}|\$([A-Za-z0-9_]+))");

    std::string result;
    auto it = std::sregex_iterator(path_str.begin(), path_str.end(), env_var_regex);
    auto end = std::sregex_iterator();

    size_t last_pos = 0;

    for (; it != end; ++it)
    {
        const std::smatch& match = *it;
        result.append(path_str, last_pos, match.position() - last_pos);

        std::string var_name = match[1].matched ? match[1].str() : match[2].str();

        const char* var_value = std::getenv(var_name.c_str());
        if (var_value)
        {
            result.append(var_value);
        } else {
            result.append(match.str());
        }

        last_pos = match.position() + match.length();
    }

    result.append(path_str, last_pos, std::string::npos);

    return result;
}

