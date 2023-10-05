/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * This file contains the skeleton for an LRU pager
 */

#include <stdio.h> 
#include <stdlib.h>
#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proc;
    int page;
    int pc;
    int oldpage;

    /* initialize timestamp array on first run */
    if (!initialized) {
        for (proc=0; proc < MAXPROCESSES; proc++) {
            for (page=0; page < MAXPROCPAGES; page++) {
                timestamps[proc][page] = 0; 
            }
        }
        initialized = 1;
    }
    
    /* TODO: Implement LRU Paging */
    for(proc = 0; proc < MAXPROCESSES; proc++)
    {
        //Is the process active?
        if(q[proc].active)
        {
            //Dedicate all work to first active process
            pc = q[proc].pc;       //program counter for process
            page = pc/PAGESIZE;    //page the program counter needs
            //update time stamp
            timestamps[proc][page] = tick;
            
            //is the page swapped out?
            if(!q[proc].pages[page])
            {
                //trying to swap in
                if(!pagein(proc,page))
                {
                    int minTime = tick; //moving tick
                    int victim;  // page to switch
                    //if swapping fails, swap out another page
                    for(oldpage = 0; oldpage < q[proc].npages; oldpage++)
                    {
                        if(timestamps[proc][oldpage] < minTime && q[proc].pages[oldpage])
                        {
                            victim = oldpage;
                            minTime = timestamps[proc][oldpage];
                        }
                    }
                    pageout(proc,victim);
                }
            }
        }
    }
    /* advance time for next pageit iteration */
    tick++;
} 