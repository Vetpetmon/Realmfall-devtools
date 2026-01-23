#include "ranked_builder.h"
#include "rfcharacters.h"
#include "cjson/cJSON.h" // Include cJSON library for JSON handling
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Get user input to create a new Character
Character get_user_input_character();

// Add a Character to the characters array
Character **add_character(Character **characters, size_t *character_count, Character new_character);

int main() {
    // Array of characters to build
    Character **characters;
    size_t character_count = 0;
    // malloc initial array
    characters = malloc(sizeof(Character *) * 0); // Start with 0, will realloc
    if (!characters) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // loop and ask user if they want to create a new character, generate files, or exit
    while (1) {
        printf("\nCharacter Builder Menu:\n");
        printf("1. Create a new character\n");
        printf("2. Generate character files\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        int choice;
        scanf("%d", &choice);
        // Clear input buffer
        while (getchar() != '\n');
        if (choice == 1) {
            Character newCharacter = get_user_input_character();
            characters = add_character(characters, &character_count, newCharacter);
            // Free temporary character fields
            free(newCharacter.name);
            free(newCharacter.displayName);
            free(newCharacter.textColor);
            free(newCharacter.secondaryColor);
        } else if (choice == 2) {
            // Generate character files for all characters in the array
            for (size_t i = 0; i < character_count; i++) {
                // Placehollder
            }
            printf("Character files generated for %zu characters.\n", character_count);
        } else if (choice == 3) {
            // Exit
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
    

    return 0;
}

// Get user input to create a new Character
Character get_user_input_character() {
    Character newCharacter;
    // Initialize to NULL
    newCharacter.name = NULL;
    newCharacter.displayName = NULL;
    newCharacter.textColor = NULL;
    newCharacter.secondaryColor = NULL;
    newCharacter.ranks = 0;
    // Get user input for each field
    
}

// Add a Character to the characters array
Character **add_character(Character **characters, size_t *character_count, Character new_character) {
    

    // Reallocate the characters array to hold one more character
    Character **temp = realloc(characters, sizeof(Character *) * (*character_count + 1));
    if (!temp) {
        fprintf(stderr, "Memory reallocation failed\n");
        return characters; // Return original array on failure
    }
    characters = temp;
    // Allocate memory for the new character and copy data
    characters[*character_count] = malloc(sizeof(Character));
    if (!characters[*character_count]) {
        fprintf(stderr, "Memory allocation for new character failed\n");
        return characters; // Return original array on failure
    }
    // allocate for name and displayName and colors
    characters[*character_count]->name = strdup(new_character.name);
    characters[*character_count]->displayName = strdup(new_character.displayName);
    characters[*character_count]->textColor = strdup(new_character.textColor);
    characters[*character_count]->secondaryColor = strdup(new_character.secondaryColor);
    characters[*character_count]->ranks = new_character.ranks;
    characters[*character_count]->charClass = new_character.charClass;
    // Increment character count
    (*character_count)++;
    return characters;
}