#5
PARSER: When an error that isn't a syntax error occurs during the parsing of a JSON file, We should either delete the entire object, or ignore the error and continue after the next '}' if specified, rather than returning at the instance of such error

#6
PARSER: Handle trailing braces