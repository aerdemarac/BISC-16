#define argc_check(argc) if(argc < 2){\
        error(NO_ARGS);\
        return 1;\
    }\

#define file_check(rom_file) if(!rom_file){\
        error(UNABLE_TO_LOCATE);\
        return 1;\
    }\

#define extension_check(ROM_PATH,rom_file) if(__extension_check(ROM_PATH)){\
        error(INCORRECT_FILE_EXTENSION);\
        fclose(rom_file);\
        return 1;\
    }\

enum exec_err{
  NO_ARGS,
  UNABLE_TO_LOCATE,
  INCORRECT_FILE_EXTENSION,
};

/* Execution Flow Methods */ 
void banner(void);
void usage(void);
void error(enum exec_err err);
int __extension_check(const char* path);
