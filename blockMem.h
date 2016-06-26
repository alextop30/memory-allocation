#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <list>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <string.h>
#include <iomanip>

using namespace std;

#ifndef MEM_MANAGE_H
#define MEM_MANAGE_H


//constants

const int HOW_OFTEN = 5;

//a kb in bytes
const int KB = 1024;

//a megabyte in kb
const int MB = 1048576;


//memory block definition
class blockMem
{
    private:
        int startAddress;                 //starting address of the memory block
        int size;                         //memory block size
        string pid;                       //process id
        string blockid;                   //block id
        char transaction;                 //type of transaction

        
    public:
        //constructors
        blockMem();
        blockMem(string, int, string, char);
        blockMem(int, int);
        blockMem(char, string);
        blockMem(char, string, string);
        
        //setters
        void setStartAddress(int);
        void setSize(int);
        
        //getters
        int getStartAddress();
        int getSize();
        string getPid();
        string getBlockid();
        char getTypeTrans();

       
};


#endif