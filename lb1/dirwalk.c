#define PATH_LEN 4096

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"

#include "sys/stat.h"
#include "dirent.h"

int compare_lc(const struct dirent** a, const struct dirent** b) {
   return strcoll((*a)->d_name, (*b)->d_name);
}

void dirwalk(const char* catalog, int symbollinks, int directories, int files, int sort ) {
   struct dirent** dnt;
   int num_dnt;

   if (sort)
      num_dnt = scandir(catalog, &dnt, NULL, compare_lc);
   else
      num_dnt = scandir(catalog, &dnt, NULL, NULL);

   char full_path[PATH_LEN];
   struct stat buf;
   for (int i = 0; i < num_dnt; i++) {
      sprintf(full_path, "%s/%s", catalog, dnt[i]->d_name);
      if (lstat(full_path, &buf) == 0) {
         if (S_ISLNK(buf.st_mode) && symbollinks)
            printf("%s\n", full_path);
         else 
         if (S_ISDIR(buf.st_mode) && directories)
            printf("%s\n", full_path);
         else 
         if (S_ISREG(buf.st_mode) && files)
            printf("%s\n", full_path);
         else 
         if (symbollinks == 0 && directories == 0 && files == 0)
            printf("%s\n", full_path);

         if (S_ISDIR(buf.st_mode))
            if (strcmp(dnt[i]->d_name, ".") != 0 && strcmp(dnt[i]->d_name, "..") != 0)
               dirwalk(full_path, symbollinks, directories, files, sort);
      }
   }
}

int main(int argc, char* argv[]) 
{
   int symbollinks = 0;
   int directories = 0;
   int files = 0;
   int sort =0 ;
   char catalog[PATH_LEN];
   catalog[0] = '.';
   catalog[1] = '\0';
   struct stat buf;
   int i;
   for (i = 1; i < argc; i++) {
      if (stat(argv[i], &buf) == 0) {
         if (S_ISDIR(buf.st_mode)) {
            strcpy(catalog, argv[i]);
            int len = strlen(catalog);
            if (catalog[len - 1] == '/') {
               catalog[len - 1] == '\0';
            }
            break;
         }
      }
   }

   int option;
   while((option = getopt(argc, argv, "ldfs")) != -1) {
      switch(option) {
         case 'l':
            symbollinks = 1;
            break;
         case 'd':
            directories = 1;
            break;
         case 'f':
            files = 1;
            break;
         case 's':
            sort = 1;
            break;
      }
   }

   dirwalk(catalog, symbollinks, directories, files, sort);
   return 0;
}
