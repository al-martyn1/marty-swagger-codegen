#pragma once


static const bool bFirst = true;

inline
void printSectionHeader ( const char *title, int szFound = -1, bool bFirst = false )
{
    using umba::lout;
    using namespace umba::omanip;

    if (!bFirst)
        lout << "\n\n#----------------------------------------------------------------------------\n";

    lout << "### "<<title;

    if (szFound>=0)
        lout << " (Total: " << szFound << ")";

    lout << ":\n\n";
}


inline
void printStringVector( const std::vector<std::string> &v )
{
    using umba::lout;
    using namespace umba::omanip;

    for( unsigned i=0; i!=v.size(); ++i)
    {
        if (i)
            lout << ", ";
        lout << v[i];
    }
}

inline
void printNameAndDeps( const std::string &name, const std::vector<std::string> &deps, int nameWidth )
{
    using umba::lout;
    using namespace umba::omanip;

    if (!deps.size())
    {
        lout << name; // << "\n";
    }
    else
    {
        lout << left << width(nameWidth) << name << " - ";
        printStringVector(deps);
    }
}


