#include "blockMem.h"

/*
 * Assignment Description: Simulate memory management on a small computer.
 * The memory restricted to 16MB of memory for the entire computer and
 * the first 3 MB of the memory addresses are reserved for the OS. The 
 * memory is maintained in two STL linked lists which are pointers to
 * blocks of memory -- block is a custom class. One command line arguments
 * is accepted which should be either F, f or B, b -- f for first fit
 * strategy and B for best fit strategy. The input file is read one
 * at a time and depending on the first char read an instruction is given
 * and the correct function call made. Deallocate function performs both
 * deallocation and termination of process. At the point of deallocating
 * a block the merge function is called every time to merge any blocks
 * smaller or equal to 4MB. The state of the system is printed every 5th
 * time however events are displayed to the console as they happen.
 * 
 */ 

//exception classes
class noFile{}NoFileEx;

//wrong command line arguments in main
class argumets{}wrong_arguments;

using namespace std;

//function prototypes
void create_avail();;

int search_bestfit(blockMem*);

int search_firstfit(blockMem*);

void print_transaction(blockMem* const);

void print_state();

int create_process(ifstream&, blockMem**);

void allocate_memory(list<blockMem*>::iterator, blockMem**);

void deallocate_mem(blockMem**);

void mem_merge();

void destroy_lists();

//global vars
const char fileName [] = "input.txt";

//list of available memory blocks
list <blockMem*> avail;

//list containging blocks in use
list <blockMem*> inUse;

//eof marker when to exit;
const char eofMark = '?';


//main accepts 1 command line argument B,b or F,f
int main(int argc, char * argv[] )
{

    //input file stream used for opeaning input file
    ifstream inputFile;
    
    //next pointer in line
    blockMem * Next = NULL;
    
    //holds return value of create process
    int test = -999;
    
    //hold return value from search algorithm
    int ffTest;
    
    //counter for the printing of the system state
    int printCount = 0;
    
    //exception handling
    try
    {
       
       //test for number of command line arguments
       if(argc == 1 || argc > 2)
       {
           throw wrong_arguments;
       }
       
       //if the second command line parameter is
       //more than 1 letter long throw an error
       if(strlen(argv[1]) > 1)
       {
           throw wrong_arguments;
           
       }
       //check for F,f, b or B in the command line
       //character
       if ((int)*argv[1] != 70 && (int)*argv[1] != 102 && 
           (int)*argv[1] != 66 && (int)*argv[1] != 98)
       {
           throw wrong_arguments;
       }
       
       //open the file
       inputFile.open(fileName);
       
       //if opening the file fails throw an exception 
       //of type exception class
       if(inputFile.fail())
       {
           throw NoFileEx;
       }
       
    }
    catch(noFile NoFileEx)
    {
        //invalid input file given
        cerr << "Inputfile specified is not valid!" << endl;
        
        //exit the program
        exit(EXIT_FAILURE);
        
    }
    catch(argumets wrong_arguments)
    {
        //incorrect arguments given
        cerr << "Incorrect command line parameters!" << endl;
        
        //exit program with -1
        exit(-1);
    }
    
    //entry message
    cout <<"Welcome to the Memory Management Program for a really small computer!" 
         << endl << endl;
    
    //check the command line arguments and display selected algorithm
    if((int)*argv[1] == 70 || (int)*argv[1] == 102)
    {
        cout << "First fit algorithm is being used for the following run." 
             << endl << endl;
    }
    else
    {
        cout << "Best fit algorithm is being used for the following run." 
             << endl <<endl;
    }
         
    //open file success message
    cout << fileName << " has been successfully opened for execution" 
         << endl << endl;
    
    //begin of run message
    cout << "Beginning of run:" << endl;
    
    //create the avail list
    create_avail();
    
    //main loop for exectution -- 0 is returned out of create process when
    //at the end of the file
    while(test != 0)
    { 
        //check if the count is evenly divisible by how often (global constant)
        //call to print state function is initiated
        if((printCount % HOW_OFTEN) == 0)
        {
            //if system is not at beginning print the time before the state
            if(printCount > 0)
            {
                cout << endl << "At time " << printCount 
                     << " the system has the following state" << endl;
            }
            
            //print the state of system
            print_state();
        
        }
        //create process store return value in test
        test = create_process(inputFile, &Next);      
        
        //0 is returned on hitting the EOF marker
        //1 is returned on successful creation and allocation
        //-1 is returned on hitting a deallocation or termination
        if(test > 0)
        {
            //check if first fit is selected
            if((int)*argv[1] == 70 || (int)*argv[1] == 102)
            {
                //call the first fit algorithm
                //storing return value into fftest
                ffTest = search_firstfit(Next);
                
                //if there is no element found by the first fit search
                //error is returned to the console
                if(ffTest == -1)
                {
                    cout << "Unable to comply as no block of" 
                         <<" adequate size is available." << endl;
                }
                else
                {
                    //if block has been found and allocated -- print
                    //success message to the console
                    cout << "Success in allocating a block!" << endl;
                }
                
            }
            else
            {
                //if best fit is selected
                if((int)*argv[1] == 66 || (int)*argv[1] == 98)
                {
                    //run best fit algorithm
                    //storing the return value in fftest
                    ffTest = search_bestfit(Next);
                    
                    //if nothing is found -1 is returned and 
                    //error is printed to the screen
                    if(ffTest == -1)
                    {
                        cout << "Unable to comply as no block of" 
                             << " adequate size is available." << endl;
                    }
                    else
                    {
                        //on finding block print successufl allocation
                        cout << "Success in allocating a block!" << endl;
                    }
                
                }
                
            }
         
         
        }
        
        //increment the printer counter
        printCount++;
    }
    
    //two blank lines printed for readability of output
    cout << endl << endl;
    
    //end of run hass been reach print the 
    //final state of the system
    cout<< "End of run" << endl;
    
    //print the final state of the system
    print_state();
        
    //close the file
    inputFile.close();
    
    //remove all the memory the program uses
    destroy_lists();
    
    //successful execution of the program
    return 0;
    
}

/* Create avail list function is responsible for
 * creating the initial available list of block
 * pointers. The addresses start from 3 times
 * MB -- which is the size of a megabyte. The list
 * is created from one 1MB blcok, two 2MB blocks
 * and two 4MB blocks. No block in the system can
 * be bigger than 4 MB.
 */
void create_avail()
{
    //create a 1 MB block and push it on the list
    blockMem * temp = new blockMem(1*MB, (3*MB));
    
    avail.push_back(temp);
    
    //create 2 -- 2MB blocks and push them on the list
    temp = new blockMem(2*MB, (4*MB));
    
    avail.push_back(temp);
    
    temp = new blockMem(2*MB, (6*MB));
    
    avail.push_back(temp);
    
    
    //create 2 -- 4 MB blocks and push them on the list
    temp = new blockMem(4*MB, (8*MB));
    
    avail.push_back(temp);
    
    temp = new blockMem(4*MB, (12*MB));
    
    avail.push_back(temp);
}


/* Create process function responsible for reading the input file and
 * creating a new module of the custom class. Further checking the
 * read information from the input file the ppropriate dicision path,
 * if the first letter read from the file is an L the function creates
 * a new process with all of the information available, the Next argument
 * is set equal to this new process and the pointer passed from main is updated
 * if a destructive function such deallocate or terminate is picked up from the
 * input file the deallocate function is called at those respective times.
 * After deallocate or terminate the memory at the next pointer is freed and
 * it is reset to point to NULL. Function produces return values of 0 if the
 * EOF character is read. On successful creation of a process or allocation
 * 1 is returned out of the function. On a terminate or deallocate -1 is 
 * returned out of the function.
 * 
 * Function does requre a reference to an input file stream opened in main
 * and a reference to a pointer of custom class so that it can be modified.
 * Function does not output to the console.
 * 
 */
int create_process(ifstream& inputFile, blockMem** Next)
{
   
    //temporary string
    string temp;
        
    //grab a line from the file
    getline(inputFile, temp);
    
    //test for end of file marker
    if(temp[0] == eofMark)
    {
        return 0;
    }
    
    //strng stream
    istringstream iss(temp);
    
    //to hold parsed strings
    vector<string> parsed;
    
    //use the copy algorithm to put the extracted strings
    //into the vector
    copy(istream_iterator<string>(iss), istream_iterator<string>(),
         back_inserter(parsed));
    
    
    
    //terminate has been initiated
    if(parsed.size() == 2)   
    {
        //create a block to be terminated
        *Next = new blockMem(parsed[0][0],parsed[1]);
        
        //print transaction state
        print_transaction(*Next);       
        
        //deallocate call - handles both terminate and
        //deallocate operations
        deallocate_mem(Next);
        
        //free the memory held by next
        delete *Next;
        
        //return -1 for terminate
        return -1;
        
    }
    else if(parsed.size() == 3)
    {
        //deallocate prosedure initiated
        *Next = new blockMem(parsed[0][0],parsed[1], parsed[2]);
        
        //print the transction state
        print_transaction(*Next); 
        
        //deallocate the block
        deallocate_mem(Next);
        
        //free memory held in next
        delete *Next;
        
        //return -1 for deallocate
        return -1;
    }

    //create a new block of memory using the secondary
    //constructor and set the pointer in main
    *Next = new blockMem(parsed[1], atoi(parsed[2].c_str()), 
                         parsed[3],parsed[0][0]);
    
    print_transaction(*Next);
    
    
    //return 1 on successful creation of new memory block
    return 1;

}


/* Function Search first fit performs a search through avail
 * linked list to find the first block that fits th size of 
 * the block requested from the input file. If a block is found
 * the function returns 1 and sets the pointer Next which is
 * a pointer in Main. STL find if algorithm used for search
 * 
 * Function does not print to the screen and requires a reference
 * to a pointer to be passed in as an argument
 * 
 */
int search_firstfit(blockMem* Next)
{
    //search the avail for the first block that is big enough fit the 
    //memory requested and call the allocate function to give the memory
    list<blockMem*>::iterator first_found = find_if(avail.begin(), avail.end(),
    [=] (blockMem* n){ if(Next->getSize() < n->getSize())
        {return true;} else {return false;}} );
    
    //if no memory chunk is available return -1 -- main displayes error
    if(first_found == avail.end())
    {
        return -1;
    }
    
    //if successfully found an element in the list
    allocate_memory(first_found, &Next);
    
    return 1;
}

/* Search best fit function is responsible for searching the 
 * avail list for a block that fits within the smallest difference
 * of size between the size that is looked for and the size of the
 * block. This is perfomed by computing the difference between
 * the size requrested and the size of each block and choosing the
 * smallest one. On success 1 is returned if no block is found
 * -1 is returned out of the function.
 * 
 * Function requires a reference to a pointer to be passed in 
 * it is set in main. The function does not print to the console
 * 
 */
int search_bestfit(blockMem* Next)
{
    //search the avail for the best fit block
    //memory requested and call the allocate function 
    //to give the memory
    
    //set difference in the size to 0
    int difference = -1;
    
    //set the number of times the loop executed to 0
    //count is used as exit condition if nothing if ound
    int count = 0;
    
    //best found itterator
    list<blockMem*>::iterator best_found;
    
    //start a for loop going though the entire list
    for(auto i = avail.begin(); i != avail.end(); i++)
    {
        //if the block to be allocated is larger in size
        //than the current block move on to the next
        //block at the next itteration of the loop
        if(Next->getSize() > (*i) -> getSize())
        {
            continue;
        }
        
        //if the difference is at its starting location
        //set it to the first fit block and continue search
        if(difference == -1)
        {
            difference = (*i)->getSize() - Next->getSize();
            
            if(difference == 0)
            {
                best_found = i;
                
                count++;
                
                break;
            }
            
            best_found = i;
                        
        }
        
        //if the difference previously computed is larger
        //than the difference of the current block reset the
        //difference to the smaller numbber
        if(difference > ((*i)->getSize() - Next->getSize()))
        {
        
            difference = (*i)->getSize() - Next->getSize();
            
            //if the difference is now 0 that means an exact
            //size block has been located and we must exit
            //the loop.
            if(difference == 0)
            {
                best_found = i;
                
                count++;
                
                break;
            }
            
            //reset the itterator for best found to 
            //the current itterator and continue the loop
            best_found = i;
        }
       
        //if a loop has fully executed increment the count    
        count++;
    }     
        
    //if no memory chunk is available return -1 -- main displayes error
    if(count == 0)
    {
        return -1;
    }
    
    //if successfully found an element in the list
    
    allocate_memory(best_found, &Next);
    
    //success return 1
    return 1;
}

/* Allocate memory is called from the search algorithms to
 * perform the actual allocation of memory. The function requres
 * an itterator of the found elent be passed along with the block
 * that is to be constructed. The function will detach the block
 * from the avail list - it will reset the link and decrement the
 * abail block size and update the addresses and will push the block
 * into the in use list.
 * 
 * The function does not return anything but will print the block that
 * was found with its size to the console.
 * 
 */
void allocate_memory(list<blockMem*>::iterator found, blockMem ** Next)
{
    //success in finding the block
    cout << "Found a block of size " << (*found)->getSize() << endl;
    
    //set the starting address of next
    (*Next)->setStartAddress(((*found)->getStartAddress()));
    
    //set the size of avail block to 
    //its size minus new block size
    (*found)->setSize((*found)->getSize()-((*Next)->getSize()));
    
    //if avail block size = 0
    //delete avail block
    if((*found)->getSize() == 0)
    {
        //remove the avail block
        avail.remove(*found);
        
        //add the new block to inuse list
        inUse.push_front(*Next);

        //set new to NULL
        *Next = NULL;
        
        //exit the function
        return;
    }
    else
    {
        //set the starting address of avail block
        (*found)->setStartAddress((*found)->getStartAddress() 
        + (*Next)->getSize());
    }
        
    //push the new block in front of in use list    
    inUse.push_front(*Next);
        
    //set new block to NULL
    *Next = NULL;
    
}

/* Print transaction function is responsible for printing events as
 * they occur. Events are Terminate, Deallocate, Allocate and Load.
 * The event is described with the necessary information.
 * 
 * The function does not return but it does print each transaction
 * information to the console. The function requires a pointer to
 * custom class to extract the information from. The information
 * is filled in the create process function.
 * 
 */
void print_transaction(blockMem* const transaction)
{
    //transaction heading
    cout << endl;
    cout << "Transaction:  request to ";
    
   
    //evaluate the type of transaction
    switch (transaction-> getTypeTrans())
    {
        //if terminate is selected print the transaction that is to be terminated
        case 'T':
            cout << "terminate process " << transaction->getPid() << endl;
        
        //if deallocate is selected print the transaction details    
            break;
        case 'D':
            cout << "deallocate block ID: " << transaction->getBlockid() 
                 << " for process " 
                 << transaction->getPid() << endl;
            
            break;
        
        //if load is selected print the transaction details
        case 'L':

                cout << "load process " << transaction->getPid() 
                     << ", block ID " << transaction->getBlockid()
                     << " using " << transaction->getSize() << " bytes"
                     << endl;
                break;
                
        case 'A':
     
                cout << "allocate ";
                cout << transaction->getSize() << " bytes for process " 
                     << transaction->getPid()
                     <<" block ID: " << transaction->getBlockid() << endl;         
            
            break;
            
        default:
            //if something is wrong in the reading of the first character
            //the incorrect input file is given and program is terminated.
            cout << "Inccoret Input file! Exiting the program" << endl;
            exit(-1);
            
            break;
    }
    
}

/* Deallocate function is responsible for taking the information
 * from the input file and searching for a memory block in the in
 * use list and taking it away from the in use list after which it
 * is added to the avail list and memory merge function is run so
 * that all blocks of memory in the avail are close to 4 MB.
 * Memory merge is called every time a block of memory is deallocated
 * no matter if the entire process needs to be de allocated.
 * 
 * Function prints succcess message to the console and it does requre
 * an argument of a reference of a pointer to custom class.
 */
void deallocate_mem(blockMem** block)
{
    //counter for while loop
    int count = 0;
    
    //holds the block that is to be removed from inuse
    list<blockMem*>::iterator remove;
    
    //holds the insert into the avail list
    list<blockMem*>::iterator insert;
    
    
    //is the transaction Terminate
    if((*block)->getTypeTrans() == 'T')
    {
        while(true)
        {
            //find the block that needs to be terminated
            remove = find_if(inUse.begin(), inUse.end(),[=] (blockMem* n)
            { if(((*block)->getPid().compare(n->getPid())) == 0) 
                {return true;} else {return false;}});
            
            if(remove == inUse.end())
            {
                //if nothing was found on first itteration of the loop
                //print error and exit the function
               if(count == 0)
               {
                   //print error message
                   cout << "Unable to comply as indicated block cannot be found." 
                        << endl;
                        
                    //free memory held at block
                    delete *block;
                    
                   //reset the pointer to NULL
                   *block = NULL;
                   
                   //exit the function
                   return;
               }
               
               //free memory held at block
               delete *block;
               
               //reset the pointer to NULL
               *block = NULL;
               
               //get out of the loop and print success message
               break;
            }
                        
            //remove the element from inuse
            inUse.remove(*remove);           
            
            //search to find where to insert found in the avail list
            insert = find_if(avail.begin(), avail.end(), [=] (blockMem* x)
            { if(x->getStartAddress() > (*remove)->getStartAddress()) 
                {return true;} else {return false;}});
            
            //insert the block by order of starting address in avail
            avail.insert(insert, (*remove));
            
            //scan if memory needs to be merged
            mem_merge();
            
            //increment stats counter
            count++;
        } 
        
        //success messsage for termination
        cout << "Success in terminating a block" << endl;
                
        
    }
    else if( (*block)->getTypeTrans() == 'D')
    {
        //the transactin type is deallocate
                
        //find the block that needs to be terminated
        remove = find_if(inUse.begin(), inUse.end(),[=] (blockMem* n)
        { if((((*block)->getPid().compare(n->getPid())) == 0) && 
            ((*block)->getBlockid().compare(n->getBlockid()) == 0)) 
        {return true;} else {return false;}});
        
        
        //if nothing was found
        if(remove == inUse.end())
        {
            //give out an error.

            cout << "Unable to comply with indicated block cannot be found." 
                 << endl;
            
            //free memory held at block
            delete *block;     
                 
            //reset the pointer to NULL
            *block = NULL;
            
            //exit out of the function
            return;
        }
                    
        //remove the element from inuse
        inUse.remove(*remove);           
        
        //search to find where to insert found in the avail list
        insert = find_if(avail.begin(), avail.end(), [=] (blockMem* x)
        { if(x->getStartAddress() > (*remove)->getStartAddress()) 
            {return true;} else {return false;}});
        
        //insert back in the avail list
        avail.insert(insert, (*remove));
        
        //scan if memory needs to be merged
        mem_merge();
        
        //success message
        cout << "Success in deallocate a block" << endl;
        
    }
    
    //free memory held at block
    delete *block;
    
    //reset the pointer to NULL
    *block = NULL;
    
}

/* Print list state function responsible for stepping through each
 * list within a loop and printing the contents of the list to to
 * the console. If the list is empty, empty is printed on the screen
 * this function is called from main with a counter of HOW_OFTEN to
 * print the state every HOW_OFTEN times.
 * 
 * Function does not require any arguments but it prints everything
 * to the console.
 */
void print_state()
{
    //total size accumulator
    int total_size = 0;
    
    cout << endl;
    
    cout << "List of available blocks" << endl;
    
    //check if the list is empty
    if(avail.size() == 0)
    {
        cout << "(none)" << endl;
    }
    else
    {
        //if not empty print the contents
        
        for(auto i = avail.begin(); i != avail.end(); i++)
        {
            //display the starting address
            cout << "Start Address = " << setw(8) << (*i)->getStartAddress() 
                 << setw(9) << "Size = " << setw(8) << (*i)->getSize() << endl;
            
            //accumulate the total size
            total_size += (*i)->getSize();
            
        }
    }
    //print size of the list
    cout << "Total size of the list = " << total_size << endl << endl;
    
    //reset the total size back to 0
    total_size = 0;
    
    //inuse list
    cout << "List of blocks in use" << endl;
    
    //check if the list is empty
    if(inUse.size() == 0)
    {
        cout << "(none)" << endl;
    }
    else
    {
        //if not empty print the list
        for(auto i = inUse.begin(); i != inUse.end(); i++)
        {
            //display the starting address
            cout << "Start Address = " << setw(8) << (*i)->getStartAddress() 
                 << setw(9) << "Size = " << setw(8) << (*i)->getSize() 
                 <<" Process ID = " << (*i)->getPid() << " " << (*i)->getBlockid()
                 << endl;
            
            //accumulate the total size
            total_size += (*i)->getSize();
            
        }
    }
    cout << "Total size of the list = " << total_size << endl;
    
}

/* Merge block function scanns the avail list for blocks that are next to
 * one another by starting address. This means that the starting address of
 * one block is equal to the starting address of the next block plus the size
 * these are concecutive blocks and could be combined if the sum of the size
 * of the two blocks is smaller or equal to 4MB. The blocks are combined by 
 * reassigning the starting address and updating the size and removing the
 * now unneeded block. This function runs every time a deallocate occurs.
 * 
 * Function does not require any arguments and it does nnot print anything
 * to the console 
 */
void mem_merge()
{
    //itterators to help with removing
    list<blockMem*>::iterator first = avail.begin();
    list<blockMem*>::iterator second = avail.begin();
   
    //set second
    second++;
    
    while (second != avail.end())
    {
             
        //run through the entire list with a for loop  
        if(((((*first)->getSize() + (*first)->getStartAddress())) == 
            (*second)->getStartAddress()) && 
            (((*first)->getSize() + (*second)->getSize()) <= (4*MB)))
        {
            cout << "Merging two blocks at " << (*first)->getStartAddress() 
                 << " and " << (*second)->getStartAddress() << endl;
            
            //set the second block's starting address to the 
            //first block's starting address
            (*second)->setStartAddress((*first)->getStartAddress());
            
            //set the second size
            (*second)->setSize((*first)->getSize()+(*second)->getSize());
            
            //remove the un needed block
            avail.remove((*first));
            
            first = avail.begin();
            second = avail.begin();
        
            if(second != avail.end())
            {
                second++;
            }
        
        }
        else
        {
            first++;
            second++;
        }
    }    
     
}

/* Function destroy lists is for memory deallocation purposes
 * and to avoid memory leaks for assignment 5. It takes the lists
 * and clears them distroying any and all dynamically allocated 
 * elements in each list if the size is greater than 0. The
 * function called is STL list container clear function
 * which goes through each element and calls the destructor. 
 */
void destroy_lists()
{
    //call the clear method of both lists if
    //their size is not 0

    if(inUse.size() != 0)
    {
        inUse.clear();
    }
    
    if(avail.size() != 0)
    {
        avail.clear();
    }
    
}
