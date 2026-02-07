#include <string>

/*
    If not # found, then find returns std::string::npos wich is just max size for size_t
 */
void RemoveComments(std::string &fullLine)
{
    size_t  pos;

    pos = fullLine.find("#");
    if (pos != std::string::npos)
        fullLine.erase(pos);
}

void    trimWhitepaces(std::string &fullLine)
{
    if (fullLine.empty())
        return;
    size_t start;
    size_t end;
    start = fullLine.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
    {
        fullLine.clear();
        return;
    }
    end = fullLine.find_last_not_of(" \t\r\n");
    fullLine = fullLine.substr(start, (end - start) + 1);
}
