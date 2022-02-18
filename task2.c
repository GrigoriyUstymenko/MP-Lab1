#include <stdio.h>
#include <stdlib.h>
#define INITIAL_BUFFER_SIZE 10
#define MAX_WORD_LENGTH 50
#define MAX_OCCURENCES 100
#define PAGE_SIZE 45

typedef struct occurrences_list {
  char word[MAX_WORD_LENGTH];
  int pages[MAX_OCCURENCES];
  int wordLength;
  int pagesLength;
  int wordCount;
} occur;

int main(int argc, char **argv){
    int bufferSize = INITIAL_BUFFER_SIZE;
    char *input;
    input = (char *) calloc(bufferSize, sizeof(char));
    occur *occurrences;
    int occurrencesLength = 0;
    occurrences = (occur *) calloc(occurrencesLength, sizeof(occur));
    FILE *inputFile;
    inputFile = fopen(argv[1], "r");
    if(inputFile == NULL) return 1;

    int pageCount = 1, count = 0, wordStart = 0, lineCount = 0;
    char inputChar;
    read_character:
      inputChar = fgetc(inputFile);
      if(inputChar <= 'Z' && inputChar >= 'A') inputChar += 'a' - 'A';

      if (count >= bufferSize) {
        bufferSize *= 2;
        input = realloc(input, sizeof(char) * bufferSize);
      }
      input[count] = inputChar;
      count++;

      if (!(inputChar <= 'z' && inputChar >= 'a')) {
        int wordLength = count - wordStart;
        if(wordLength <= 1) {
          if(inputChar == EOF) goto read_character_end;
          wordStart = count;
          goto read_character;
        }

        char inputWord[wordLength];
        int i = 0;
        init_input_word:
          if(i >= wordLength - 1) goto init_input_word_end;
          inputWord[i] = *(input + wordStart + i);
          i++;
          goto init_input_word;
        init_input_word_end:
        inputWord[wordLength - 1] = '\0';
        wordStart = count;
        
        unsigned existsAlready = 0;
        int i1 = 0;
        update_occurrences:
          if(i1 >= occurrencesLength) goto update_occurrences_end;
          if (occurrences[i1].wordLength != wordLength) {
            i1++;
            goto update_occurrences;
          }

          int i2 = 0;
          check_existence:
            if(i2 >= wordLength) goto check_existence_end;
            existsAlready = occurrences[i1].word[i2] == inputWord[i2];
            if(!existsAlready) goto check_existence_end;
            i2++;
            goto check_existence;
          check_existence_end:;

          int pageExistsAlready = 0;
          if(existsAlready) {
            occurrences[i1].wordCount++;
            int oldLength = occurrences[i1].pagesLength;

            int i3 = 0;
            check_page_existence:
              if (i3 >= oldLength) goto check_page_existence_end;
              if(occurrences[i1].pages[i3] == pageCount) {
                pageExistsAlready = 1;
                goto check_page_existence_end;
              }
              i3++;
              goto check_page_existence;
            check_page_existence_end:;

            if(!pageExistsAlready) {
              if(occurrences[i1].wordCount > MAX_OCCURENCES) goto update_occurrences_end;
              occurrences[i1].pagesLength = oldLength + 1;
              occurrences[i1].pages[oldLength] = pageCount;
            }
            goto update_occurrences_end;
          }
          i1++;
          goto update_occurrences;
        update_occurrences_end:;

        if (!existsAlready) {
          occurrences = realloc(occurrences, (occurrencesLength + 1) * sizeof(occur));
          int i = 0;
          copy_word:
            if(i >= wordLength) goto copy_word_end;
            occurrences[occurrencesLength].word[i] = inputWord[i];
            i++;
            goto copy_word;
          copy_word_end:;
          occurrences[occurrencesLength].wordLength = wordLength;
          occurrences[occurrencesLength].pagesLength = 1;
          occurrences[occurrencesLength].wordCount = 1;
          occurrences[occurrencesLength].pages[0] = pageCount;
          occurrencesLength++;
        }

        if(inputChar == '\n') {
          lineCount++;
          if (lineCount == PAGE_SIZE) {
            pageCount++;
            lineCount = 0;
          }
        }

        if(inputChar == EOF) goto read_character_end;
      }
      goto read_character;
    read_character_end:;

    int i = 0;
    outer_sort:
      if(i >= occurrencesLength - 1) goto outer_sort_end;

      int j = 0;
      inner_sort:
        if(j >= occurrencesLength - i - 1) goto inner_sort_end;
        
        int wordDiff = 0;
        int firstWordLonger = occurrences[j].wordLength > occurrences[j + 1].wordLength;
        int shorterLength = firstWordLonger ? 
            occurrences[j].wordLength : occurrences[j + 1].wordLength;
        
        int k = 0;
        string_compare:
          if(k >= shorterLength) goto string_compare_end;
          char firstLetter = occurrences[j].word[k];
          char secondLetter = occurrences[j + 1].word[k];
          if(firstLetter != secondLetter) {
              wordDiff = firstLetter - secondLetter;
              goto swap;
          }
          k++;
          goto string_compare;
        string_compare_end:;
        wordDiff -= firstWordLonger;
        swap:
        if (wordDiff > 0) {
          occur temp = occurrences[j];
          occurrences[j] = occurrences[j + 1];
          occurrences[j + 1] = temp;
        }

        j++;
        goto inner_sort;
      inner_sort_end:

      i++;
      goto outer_sort;
    outer_sort_end:;

    FILE *outputFile;
    outputFile = fopen(argv[2], "w");

    int i1 = 0;
    write_row:
      if(i1 >= occurrencesLength) goto write_row_end;

      if(occurrences[i1].wordCount > MAX_OCCURENCES) {
        i1++;
        goto write_row;
      }

      fprintf(outputFile, "%s - ", occurrences[i1].word);

      int j1 = 0;
      write_pages:
        if(j1 >= occurrences[i1].pagesLength) goto write_pages_end;

        fprintf(outputFile, "%d", occurrences[i1].pages[j1]);
        if(j1 == occurrences[i1].pagesLength - 1) {
          fprintf(outputFile, "\n");
          j1++;
          goto write_pages;
        }

        fprintf(outputFile, ", ");

        j1++;
        goto write_pages;
      write_pages_end:;

      i1++;
      goto write_row;
    write_row_end:;

    fclose(outputFile);
    free(input);
    free(occurrences);
    fclose(inputFile);
    return 0;
}