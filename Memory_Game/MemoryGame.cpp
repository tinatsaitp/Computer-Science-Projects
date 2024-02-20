#include "MemoryGame.hpp"
#include <cstdlib> //srand, rand
#include <ctime>
#include <iomanip> //setw
#include <iostream> //std
#include <vector>
#include <string>
using namespace std;

int* randomize(int numSlots);
void swap(int* arr, int i, int j);
void displaySeparateLine(int numSlots);

MemoryGame::MemoryGame() 
{
    srand(1);

    numPairs = 3; 
    numSlots = 8; 
  
    //generate three random ints in [0, 1000),
    int *data = new int[numPairs];
    for (int i = 0; i < numPairs; i++) {
        data[i] = rand() % 1000;
    }
  
    int *arr = randomize(numSlots);
    values = new string[numSlots];
 
    for (int i = 0; i < numSlots; i++) {
      if (i < numPairs*2) {
        values[arr[i]] = to_string(data[i/2]);
      }
      else {
        values[arr[i]] = "";
      }
    }
  
    delete[] data;
    data = nullptr;

    delete[] arr;
    arr = nullptr;
}

MemoryGame::~MemoryGame()
{
    delete[] values;
	  values = nullptr;
}

int* randomize(int size)
{
    //static allocated memory
    //create an array in [0, 8),
    int *arr = new int[size];
    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }

    int index;
    //change order of the array randomly
    while(size > 0){
        index = rand() % size;
        swap(arr, index, size-1);
        size -= 1;
    }

    return arr;
}

void swap(int *arr, int i, int j)
{
  int chnum = arr[i];
  arr[i] = arr[j];
  arr[j] = chnum;
}

void displaySeparateLine(int numSlots)
{
    for (int i = 0; i < numSlots; i++)
    {
        if (i == 0)
           cout << "+";
        cout << "-----+";
    }
    cout << endl;
}

//display the items in values array where bShown is true.
void MemoryGame::display(bool* bShown)
{
  cout << " ";
    for (int label = 0; label < numSlots; label++) {
        cout << setw(3) << label;
        cout << setw(3) << " ";
    }
    cout << endl;

    displaySeparateLine(numSlots);

    cout << "|"; 
    for (int i = 0; i < numSlots; i++) {
      if (bShown[i] == true) {
        cout << setw(5) << values[i];
      }
      else {
        cout << setw(5) << "";
      }
      cout << "|";
    }

    cout << endl;
  
    displaySeparateLine(numSlots);
}

void MemoryGame::play()
{ 
  int pairsFound = 0;
  int numFlips = 0;
  int index;
  int first;
  
  bool* bShown = new bool[numSlots]; 
  for (int i = 0; i < numSlots; i++) {
    bShown[i] = false;
  }
  
  display(bShown);
  
  
  
  while (pairsFound < numPairs) {
    cout << "Pick a cell to flip: ";
    cin >> index;
    while (index < 0 || index >= numSlots || bShown[index]==true)
    {
      if (index < 0 || index >= numSlots) {
        cout << "Index needs to be in range of [0, " << numSlots << "]." << endl;
      }
      else {
        cout << "The cell indexed at " << index << " is shown." << endl;
      }
    cout << "Re-enter an index: ";
    cin >> index;  
    }
    
    if (numFlips % 2 == 0) {
          bShown[index] = true;
          first = index;
        }
    else {
      if (values[first] == values[index] && values[first] != "") {
        bShown[index] = true;
        pairsFound++;
      }
      else {
        bShown[first] = false;
      }    
    }
    display(bShown);
    numFlips++;
  }

  cout << "Congratulations! Take " << numFlips << " steps to find all matched pairs." << endl;
  delete[] bShown;
  bShown = nullptr;
}