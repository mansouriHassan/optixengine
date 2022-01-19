
#pragma once

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>

enum ParserTokenType
{
  PTT_UNKNOWN, // Unknown, normally indicates an error.
  PTT_ID,      // Keywords, identifiers (not a number).
  PTT_VAL,     // Immediate floating point value.
  PTT_STRING,  // Filenames and any other identifiers in quotation marks.
  PTT_EOL,     // End of line.
  PTT_EOF      // End of file.
};


// System and scene file parsing information.
class ConfigParser
{
public:
  ConfigParser();
  //~ConfigParser();
  
  bool load(std::string const& filename);

  ParserTokenType getNextToken(std::string& token);

  size_t                 getSize() const;
  std::string::size_type getIndex() const;
  unsigned int           getLine() const;
    
private:
  std::string            m_source; // System or scene description file contents.
  std::string::size_type m_index;  // Parser's current character index into m_source.
  unsigned int           m_line;   // Current source code line, one-based for error messages.
};

#endif // CONFIG_PARSER_H
