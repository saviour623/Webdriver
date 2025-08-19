#1 INIT
<p> Object should have a pointer to it's parent. This would eliminate the need to recursively tranverse through objects with objects or the use of stack to do the tranersal <br>
--------ELIMINATE: we could eliminate __self for __parent <br>
--------TEST: This false inheritance is initiated when adding new object to the base object;
</p>
#5
PARSER: When an error that isn't a syntax error occurs during the parsing of a JSON file, We should either delete the entire object, or ignore the error and continue after the next '}' if specified, rather than returning at the instance of such error

#6
PARSER: Handle trailing braces