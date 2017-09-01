/*
 * Include the provided hashtable library
 */
#include "hashtable.h"

/*
 * Include the header file
 */
#include "philspel.h"

/*
 * Standard IO and file routines
 */
#include <stdio.h>

/*
 * General utility routines (including malloc())
 */
#include <stdlib.h>

/*
 * Character utility routines.
 */
#include <ctype.h>

/*
 * String utility routines
 */
#include <string.h>

/*
 * this hashtable stores the dictionary
 */
HashTable *dictionary;

/*
 * the MAIN routine.  You can safely print debugging information
 * to standard error (stderr) and it will be ignored in the grading
 * process, in the same way which this does.
 */
int main(int argc, char **argv){
  if(argc != 2){
    fprintf(stderr, "Specify a dictionary\n");
    return 0;
  }
  /*
   * Allocate a hash table to store the dictionary
   */
  fprintf(stderr, "Creating hashtable\n");
  dictionary = createHashTable(2255, &stringHash, &stringEquals);

  fprintf(stderr, "Loading dictionary %s\n", argv[1]);
  readDictionary(argv[1]);
  fprintf(stderr, "Dictionary loaded\n");

  fprintf(stderr, "Processing stdin\n");
  processInput();

  // printf("--%d--\n", isindictionary("sentences"));

  /* main in C should always return 0 as a way of telling
     whatever program invoked this that everything went OK
     */
  return 0;
}

/*
 * You need to define this function. void *s can be safely casted
 * to a char * (null terminated string) which is done for you here for
 * convenience.
 */
unsigned int stringHash(void *s){
  int hash = (int) *((char *)s);

  while (*((char *)s) != '\0') {
    hash += 128*(int) *((char *)s++);
  }

  return hash;
/* Cited from http://stackoverflow.com/questions/7666509/hash-function-for-string */
}
/*
 * You need to define this function.  It should return a nonzero
 * value if the two strings are identical (case sensitive comparison)
 * and 0 otherwise.
 */
int stringEquals(void *s1, void *s2){
  char *string1 = (char *) s1;
  char *string2 = (char *) s2;
  if (strcmp(string1, string2) == 0)
    return 1;
  else
    return 0;
}

/*
 * this function should read in every word in the dictionary and
 * store it in the dictionary.  You should first open the file specified,
 * then read the words one at a time and insert them into the dictionary.
 * Once the file is read in completely, exit.  You will need to allocate
 * (using malloc()) space for each word.  As described in the specs, you
 * can initially assume that no word is longer than 60 characters.  However,
 * for the final 20% of your grade, you cannot assumed that words have a bounded
 * length You can NOT assume that the specified file exists.  If the file does
 * NOT exist, you should print some message to standard error and call exit(0)
 * to cleanly exit the program. Since the format is one word at a time, with
 * returns in between, you can
 * safely use fscanf() to read in the strings.
 */
void readDictionary(char *filename) {
  /* Open the file, and define the file as 'file'.*/
  FILE *file = fopen(filename, "r");
  /* Check if the file exists.*/
  if (file != NULL) {
  /* Read words one at a time.*/
  /* Basically read string of characters.*/
    int result = 1;
    while (result != EOF) {
    /* Make another memory block for each element.*/
      char * element = malloc(sizeof(char)*60);
      result = fscanf(file, "%s", element);
    /* Insert the data into the Dictionary.*/

      insertData(dictionary, (void *) element, (void *) element);
    }
  /* Closing the file.*/
    fclose(file);
  }
  else {
    fprintf(stderr, "File does not exist.\n");
  }
}
/* Check if a word is in the dictionary.*/
int isindictionary(char * buffer) {

  void * value_1 = findData(dictionary, buffer);

  char new_buffer[60];
  int i;
  for (i = 0; i <= sizeof(buffer); i++) {
    new_buffer[i] = tolower(buffer[i]);
  }
  void * value_3 = findData(dictionary, new_buffer);

  new_buffer[0] = buffer[0];

  void * value_2 = findData(dictionary, new_buffer);

  // printf("\n\n11 %s\n\n", (char *)value_1);
  // printf("\n\n22 %s\n\n", (char *)value_2);
  // printf("\n\n33 %s\n\n", (char *)value_3);

  if (value_1 == NULL && value_2 == NULL && value_3 == NULL) {
    return 0;
  }
  return 1;
}
/*
 * This should process standard input and copy it to standard output
 * as specified in specs.  EG, if a standard dictionary was used
 * and the string "this is a taest of  this-proGram" was given to
 * standard input, the output to standard output (stdout) should be
 * "this is a teast [sic] of  this-proGram".  All words should be checked
 * against the dictionary as they are input, again with all but the first
 * letter converted to lowercase, and finally with all letters converted
 * to lowercase.  Only if all 3 cases are not in the dictionary should it
 * be reported as not being found, by appending " [sic]" after the
 * error.
 *
 * Since we care about preserving whitespace, and pass on all non alphabet
 * characters untouched, and with all non alphabet characters acting as
 * word breaks, scanf() is probably insufficent (since it only considers
 * whitespace as breaking strings), so you will probably have
 * to get characters from standard input one at a time.
 *
 * As stated in the specs, you can initially assume that no word is longer than
 * 60 characters, but you may have strings of non-alphabetic characters (eg,
 * numbers, punctuation) which are longer than 60 characters. For the final 20%
 * of your grade, you can no longer assume words have a bounded length.
 */
void processInput() {
  char buffer[60];
  int index = 0;
  int character;
  while ((character = getchar()) != EOF) {
    if (isalpha(character) != 0) {
      putchar(character);
      buffer[index++] = character;
    } else {
      int isinDic = isindictionary(buffer);
      if (isinDic == 0) {
        putchar(' ');
        putchar('[');
        putchar('s');
        putchar('i');
        putchar('c');
        putchar(']');
      }
      putchar(character);

      while ((character = getchar()) != EOF && isalpha(character) == 0) {
        putchar(character);
      }

      // clear buffer
      if (character != EOF) {
        memset(buffer, '\0', sizeof(buffer));
        buffer[0] = character;
        index = 1;
        putchar(character);
      }
      
    }
  }

  int isinDic = isindictionary(buffer);
      if (isinDic == 0) {
        putchar(' ');
        putchar('[');
        putchar('s');
        putchar('i');
        putchar('c');
        putchar(']');
      }
      
}
