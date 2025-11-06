#ifndef WBRAG_LIBS_FILE_UTIL_SRC_FILE_UTIL_H
#define WBRAG_LIBS_FILE_UTIL_SRC_FILE_UTIL_H

#include <stdint.h>

class FileUtil {
 public:
  static void ReadBinFile(char* file_path, const int element_type, const int num_elements, void* dst);
  static void ReadTextFile(char* file_path, const int num_elements, void* dst);
  static void ReadDigitFile(char* file_path, const int num_elements, void* dst);
  static void ReadOctFile(char* file_path, const int num_elements, uint8_t* dst);
  static void ReadHexFile(char* file_path, const int num_elements, uint16_t* dst);
};

#endif