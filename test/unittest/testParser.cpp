#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <string>
#include <vector>
#include <iostream>
#include "../src/include/Parser.h"
#include "../src/include/Scanner.h"
#include "../src/include/keywords.h"
#include <exception>




std::vector<std::string> splitStreamToLines(std::ostringstream &stream) {
  // The output stream prints the result with newline characters
  // We don't want to write expected results with those, so this is
  // for convenience
  std::string tmp; 
  std::stringstream ss(stream.str());
  std::vector<std::string> lines;

  while(getline(ss, tmp, '\n')){
      lines.push_back(tmp);

  }
  return lines;
}



TEST_CASE("1. Print integer") {
    // Not yet implemented
    CHECK(1 == 1);
    /*
    std::vector<std::string> tokens;
    tokens.push_back()

    Parser prsr = Parser(tokens, getKeywords());
    std::ostringstream output_stream;
    exec_query(query, &output_stream);
    CHECK(splitStreamToLines(output_stream) == expected);  
    */
}
