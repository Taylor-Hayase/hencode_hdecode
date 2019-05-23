# hencode_hdecode
C implementation of huffman encoding and decoding

Compile with 'make all'

This program will create 3 executables

Htable:
  Usage: ./htable <filename>
  This program will output a table of all the character from a given file and their corresponding huffman codes
  This program is used be both hencode and hdecode.
  
Hencode:
  Usage: ./hencode infile [outfile]
  This program will take in a file and encode its contents using a huffman encoding process.
  If an output file is given the resulting ecoded file will be written to the file.
  Otherwise the encoded file will be put to stdout.
  
Hdecode:
  Usage: ./hdecode [(infile | -) [outfile]]
  This program can either take in a file, or input from stdin and then decode the given file using huffman decoding.
  if the '-' flag is given, then the program will read from sdtin, otherwise it will read from the provided file.
  
