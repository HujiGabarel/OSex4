#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#include <cstdio>
#include <cassert>

#include <iostream>
#include <cstdlib>

using namespace std;
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void print_RAM ()
{
  word_t value;
  cout << "\n##############\nRAM\n#####" << endl;
  for (int i = 0; i < RAM_SIZE; ++i)
  {
    PMread (i, &value);
    cout << value << endl << "#####" << endl;
  }
  cout << "\nEND OF RAM\n##############\n";
}

int simple_test ()
{
  VMinitialize ();
  for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i)
  {
    printf ("writing to %llu\n", (long long int) i);
    VMwrite (5 * i * PAGE_SIZE, i);
  }

  for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i)
  {
    word_t value;
    VMread (5 * i * PAGE_SIZE, &value);
    printf ("reading from %llu %d\n", (long long int) i, value);
    assert(uint64_t(value) == i);
  }
  printf ("success\n");

  return 0;
}

int simpler_test ()
{
  VMinitialize ();
  word_t value;

  cout << "##############\nBasic write to memory\n" << endl;
  VMwrite (13, 3);
  //print_RAM (); // worked!
  cout << "##############\n" << endl;

  cout << "##############\nBasic read from memory" << endl;
  VMread (13, &value);
//  print_RAM();
  cout << "##############\n" << endl;

  printf ("read %d\n", value);
  assert(uint64_t(value) == 3);
  cout << "##############\n" << endl;

  cout << "##############\nRead 6 from the memory\n" << endl;
  VMread(6, &value);
  print_RAM();
  cout << "##############\n" << endl;

  cout << "##############\nRead 31 from the memory\n" << endl;
  VMread (31, &value);
  print_RAM();
  cout << "##############\n" << endl;

  return 1;
}

int invalid_test(){
  VMinitialize ();
  word_t value;

  int res;
  res = VMwrite (31, 177);
  assert(res == 1);

  res = VMread (31, &value);
  assert(res == 1);
  assert(value == 177);

  res = VMwrite (32, 177);
  assert(res == 0);

  res = VMread (32, &value);
  assert(res == 0);
  assert(value == 177);

  cout << "Passed Test of invalid input!\n";

  return 1;
}

int random_test(){
    printf("hi\n");
  VMinitialize ();
  word_t values[VIRTUAL_MEMORY_SIZE];
  word_t value;
  int res;

  long test_len = MIN(VIRTUAL_MEMORY_SIZE, 10000);

  for (int i = 0; i < test_len; ++i)
  {
    values[i] = 0;
  }

  for (int i = 0; i < test_len; ++i)
  {
    int random_address = (rand() % test_len);
    word_t random_value = (word_t)(rand() % 10000);

    values[random_address] = random_value;

    res = VMwrite (random_address, random_value);
    assert(res == 1);
  }

  for (int i = 0; i < test_len; ++i)
  {
    if(values[i] != 0){
        res = VMread (i, &value);
        assert(res == 1);
        assert(value == values[i]);
    }
  }

  cout << "Passed Test of random input!\n";

  return 1;
}

int main (int argc, char **argv)
{
    simple_test();
  random_test();
}
