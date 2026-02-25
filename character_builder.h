#include "ranked_builder.h"
#include "rfcharacters.h"
#include "cjson/cJSON.h" // Include cJSON library for JSON handling
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#ifdef _WIN32
#define PATH_MAX 4096  // MAX_PATH for classic Win32 paths
#endif

// Get user input to create a new Character
Character get_user_input_character();

// Add a Character to the characters array
Character **add_character(Character **characters, size_t *character_count, Character new_character);

// Print class stats per rank
void print_class_stats(CharacterClass charClass);

// Select a class predef to view stats
void select_and_print_class_stats();

// Clear screen function (platform-independent)
void clear_screen();

// main loop for character builder
int char_builder_main_loop();
