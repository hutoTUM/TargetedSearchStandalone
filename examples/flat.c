int catchme(int i);

__attribute__((noinline)) int flat(int i) {
  i++;
  return catchme(i);
}

__attribute__((noinline)) int noreach(int i) {
  i++;
  return i;
}

int main(int argc, char** argv) {
  return flat(argc);
}
