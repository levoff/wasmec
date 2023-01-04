#ifndef WASTRING_C_
#define WASTRING_C_



void intostr(int integer, char *string) {
    unsigned char i = 31;
    int flag = 0;
    if (integer < 0) {
        flag = 1;
        string[0] = '-';
        integer = -integer;
    }

    while (integer != 0) { 
        string[i--] = (integer % 10) + '0';
        integer /= 10;
    }
    int j;
    for (j = flag; i < 32; ++j)
    {
        string[j] = string[i+1];
        i++;
    }
    string[j-1] = '\0';
}

void strcat3(char *leftstr, char mid, char *rightstr) {
  int i = 0, j = 0;

  while (leftstr[i] != '\0') {
    i++;
  }
  leftstr[i] = mid;
  i++;
  while (rightstr[j] != '\0') {
    leftstr[i] = rightstr[j];
    j++;
    i++;
  }
  leftstr[i] = '\0';
}

#endif //WASTRING_C_
