#ifndef STRINGSPLIT_H
#define STRINGSPLIT_H
#include <string>


template <typename Container>
Container& split( Container& result, std::string &s, const typename Container::value_type& delimiters, bool empties = true )
{
  result.clear();
  size_t current;
  size_t next = -1;
  do
  {
    if (empties == false)
    {
      next = s.find_first_not_of( delimiters, next + 1 );
      if (next == Container::value_type::npos) break;
      next -= 1;
    }
    current = next + 1;
    next = s.find_first_of( delimiters, current );
    result.push_back( s.substr( current, next - current ) );
  }
  while (next != Container::value_type::npos);
  return result;
}

//usage:
//string s = "One, two,, four , five,";
//vector <string> fields;
//split( fields, s, "," );

#endif // STRINGSPLIT_H
