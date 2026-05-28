#include <stdio.h>
#include <string.h>
#include "exec.h"

#ifdef __unix__
  #include <strings.h>
#endif

#define BIN_EXT ".bin"

void banner(void)
{
  printf("\n  -------------------------------------\n");
  printf("          BISC-16 Virtual Machine\n");
  printf("  -------------------------------------\n");
  printf("    MIT-Licensed  /  2026 Erdem Araç\n");
  printf("  https://github.com/aerdemarac/BISC-16\n");
  printf("  -------------------------------------\n\n");
  return;
}

void usage(void)
{
  printf("  Usage: ");
  printf("./bisc-vm <romfile-path> \n\n");
  return;
}

void error(enum exec_err err){
  switch (err) {
    case NO_ARGS:
      printf("  ERROR: Please insert a valid file-path !\n\n");
      break;
    case UNABLE_TO_LOCATE:
      printf("  ERROR: Unable to locate rom-file !\n\n");
      break;
    case INCORRECT_FILE_EXTENSION:
      printf("  ERROR: Please Insert a ROM with BIN extension !\n\n");
      break;
    default:
      printf("  ERROR: Make sure file-path is valid and rom file is generated !\n\n");
      break;
  }
  return;
}

int __extension_check(const char* path){
  if(!path || strlen(path) < 6 ){
    return -1;
  }
  const char* ext = path + strlen(path) - 4;
  return strncmp(ext,BIN_EXT,strlen(BIN_EXT)) ;
}
