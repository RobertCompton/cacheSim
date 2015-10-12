#include "cacheSim.h"

int main(int argc, char *argv[]){
  int i,s,e,b,flag,addr,args;
  bool err, verbose;
  FILE *f;
  cacheType* cache;
  args = 0;
  err = verbose = false;

  for(i = 1; i < argc; i++){
    if(strcmp(argv[i], "-S") == 0){
      s = atoi(argv[++i]);
      if(s & (s-1) != 0){
        err = true;
        printf("Set size (-S s) was not a power of 2.\n");
      }args++;
    }else if(strcmp(argv[i], "-E") == 0){
      e = atoi(argv[++i]);
      if(e & (e-1) != 0){
        err = true;
        printf("Associativity (-E e) was not a power of 2.\n");
      }args++;
    }else if(strcmp(argv[i], "-B") == 0){
      b = atoi(argv[++i]);
      if(b & (b-1) != 0){
        err = true;
        printf("Line size (-B b) was not a power of 2.\n");
      }args++;
    }else if(strcmp(argv[i], "-F") == 0){
      f = fopen(argv[++i], "r");
      if(!f){
        err = true;
        printf("File could not be opened. Check the path.\n");
      }args++;
    }else if(strcmp(argv[i], "-D") == 0){
      verbose = true;
    }
  }
  if(args != NUM_ARGS) err = true;
  if(err){
    printf("usage:cacheSim -S s -E e -B b -F <filename>\n"
      "      s - number of Sets\n"
      "      e - number of Lines per Set\n"
      "      b - number of Bytes per Block\n"
      "      <fileName> - name of trace file\n"
      "s,e,b must be powers of two\n");
    exit(1);
  }

  cache = initCache(s,e,b);

  //dump starting file comments
  for(i=0;i<NUM_COMMENTS;i++)
    fscanf(f, "%*[^\n]\n", NULL);

  //loop through .din and simulate access for each access in the log
  while(!feof(f)){
    fscanf(f, "%d %x%*[^\n]\n",&flag,&addr);
    access(cache, flag, addr);
    if(verbose) printCache(cache);
  }

  fclose(f);
  printResults(cache);
}
