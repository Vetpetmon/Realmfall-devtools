#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include "rfcharacters.h"
#include "ranked_builder.h"
#include "character_builder.h"


//enum storing menu choices to sub-programs
enum MenuChoice_CharBuilder {
    CREATE_CHARACTER = 1,
    CREATE_ARTICLE_TEMPLATE = 2,
    CREATE_ARMOR_PROPERTIES = 3,
    CREATE_MODSET_DATA = 4,
    EXIT = 5,

};

void print_logo();


// main loop for character builder
int main() {

    print_logo();
    // input loop for menu

    while(1) {
        // Display menu
        printf("\nCharacter Builder Menu:\n");
        printf("1. Create characters\n");
        printf("2. Create article templates\n");
        printf("3. Create armor properties\n");
        printf("4. Create modset data\n");
        printf("5. Exit\n");

        int choice;
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case CREATE_CHARACTER:
                clear_screen();
                char_builder_main_loop();
                break;
            case CREATE_ARTICLE_TEMPLATE:
                // Call article template creation function
                printf("Article template creation not implemented yet.\n");
                // Await user input before clearing screen
                printf("Press Enter to continue...");
                scanf("%*c");
                while (getchar() != '\n');
                clear_screen();
                break;
            case CREATE_ARMOR_PROPERTIES:
                // Await user input before clearing screen
                printf("Armor properties creation not implemented yet.\n");
                printf("Press Enter to continue...");
                scanf("%*c");
                while (getchar() != '\n');
                clear_screen();
                // Call armor properties creation function
                break;
            case CREATE_MODSET_DATA:
                printf("Modset data creation not implemented yet.\n");
                printf("Press Enter to continue...");
                scanf("%*c");
                while (getchar() != '\n');
                clear_screen();
                // Call modset data creation function
                break;
            case EXIT:
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}



void print_logo() {
    printf("┌──────────────────────────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│'||''|.   '||''''|      |     '||'      '||    ||' '||''''|  '|.   '|'  ..|'''.|  '||' '|.   '|' '||''''| │\n");
    printf("│ ||   ||   ||  .       |||     ||        |||  |||   ||  .     |'|   |  .|'     '   ||   |'|   |   ||  .   │\n");
    printf("│ ||''|'    ||''|      |  ||    ||        |'|..'||   ||''|     | '|. |  ||    ....  ||   | '|. |   ||''|   │\n");
    printf("│ ||   |.   ||        .''''|.   ||        | '|' ||   ||        |   |||  '|.    ||   ||   |   |||   ||      │\n");
    printf("│.||.  '|' .||.....| .|.  .||. .||.....| .|. | .||. .||.....| .|.   '|   ''|...'|  .||. .|.   '|  .||.....|│\n");
    printf("│                                                                                                          │\n");
    printf("│                                                                                                          │\n");
    printf("│'||''|.   '||''''|  '||'  '|' '||'  |'  '||' |''||''|                                                     │\n");
    printf("│ ||   ||   ||  .     '|.  .'   || .'     ||     ||                                                        │\n");
    printf("│ ||    ||  ||''|      ||  |    ||'|.     ||     ||                                                        │\n");
    printf("│ ||    ||  ||          |||     ||  ||    ||     ||                                                        │\n");
    printf("│.||...|'  .||.....|     |     .||.  ||. .||.   .||.                                                       │\n");
    printf("└──────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");

}