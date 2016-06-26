#include "blockMem.h"

using namespace std;

blockMem::blockMem()
{
    //set everything 0 or null respectivelly
    startAddress = 0;
    size = 0;
    
}

blockMem::blockMem(string pID, int Size, string blockID, char type)
{
    //set the pid
    pid = pID;
    
    //set the size
    size = Size;
    
    //set the block id
    blockid = blockID;
    
    transaction = type;
}

blockMem::blockMem(char type, string pID)
{
    //set the transaction type
    transaction = type;
    
    //set the process id
    pid = pID;
    
}


blockMem::blockMem(char type, string pID, string blockID)
{
    transaction = type;
    
    pid = pID;
    
    blockid = blockID;
}

blockMem::blockMem(int Size, int address)
{
    size = Size;
    
    startAddress = address;
}


//setters

void blockMem::setStartAddress(int address)
{
    startAddress = address;
}

void blockMem::setSize(int Size)
{
    size = Size;
}

//getters

int blockMem::getStartAddress()
{
    return startAddress;
}

int blockMem::getSize()
{
    return size;
}

string blockMem::getPid()
{
    return pid;
}

string blockMem::getBlockid()
{
    return blockid;
}

char blockMem::getTypeTrans()
{
    
    return transaction;
}


