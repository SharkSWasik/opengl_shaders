#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using std::cerr;
using std::endl;
using std::string;
using std::ostringstream;
using std::ifstream;

string load_file(const char* filename)
{
    ostringstream content;
    ifstream input_file(filename);

    if (!input_file.is_open())
    {
        cerr << "Could not open file" << endl;
        exit(EXIT_FAILURE);
    }

    content << input_file.rdbuf();
    return content.str();
}
