#include <stdint.h>
#include <stdio.h>

struct record {
  char name[80];
  char address[80];
  uint8_t semester;
};

int main() {
  FILE *file = fopen("data.bin", "w");
  struct record record = {0};
  sprintf(record.name, "Solodkov Maxim");
  sprintf(record.address, "Minsk");
  record.semester = 1;
  fwrite(&record, sizeof(record), 1, file);

  sprintf(record.name, "Lagodich Ilya");
  sprintf(record.address, "Minsk");
  record.semester = 1;
  fwrite(&record, sizeof(record), 1, file);

  sprintf(record.name, "Borisevich Matvey");
  sprintf(record.address, "Minsk");
  record.semester = 1;
  fwrite(&record, sizeof(record), 1, file);

  sprintf(record.name, "Dimka Terrabyte");
  sprintf(record.address, "Minsk");
  record.semester = 1;
  fwrite(&record, sizeof(record), 1, file);

  sprintf(record.name, "Moy Batya");
  sprintf(record.address, "Minsk");
  record.semester = 1;
  fwrite(&record, sizeof(record), 1, file);

  fclose(file);
}