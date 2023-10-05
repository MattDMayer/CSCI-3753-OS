#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int prev[MAXPROCESSES];
    static int procType[MAXPROCESSES];

    /* Local vars */
    int proc;
    int page;
    int pc;
    int jump;
    /* initialize static vars on first run */
    if(!initialized)
    {
	    /* Init complex static vars here */
        for(int i=0; i<MAXPROCESSES; i++)
        {
            prev[i] = 0;
            procType[i]=0;
        }
	    initialized = 1;
    }

    /* TODO: Implement Predictive Paging */
    for(proc = 0; proc < MAXPROCESSES; proc++)
    {
        
        if(q[proc].active)
        {
            pc = q[proc].pc;
            page = pc/PAGESIZE;
            jump = pc - prev[proc];
            prev[proc] = q[proc].pc;

            switch(jump)
            {
                case 132:
                {
                    procType[proc] = 1;
                    break;
                }
                case 902:
                {
                    procType[proc] = 1;
                    break;
                }
                case -1129:
                {
                    procType[proc] = 2;
                    break;
                }
                case -1130:
                {
                    procType[proc] = 2;
                    break;
                }
                case -516:
                {
                    procType[proc] = 3;
                    break;
                }
                case -501:
                {
                    procType[proc] = 5;
                    break;
                }
                case -503:
                {
                    procType[proc] = 5;
                    break;
                }
                case -504:
                {
                    procType[proc] = 5;
                    break;
                }
            }
            switch(procType[proc])
            {
                case 1: //loop with inner branch
                {
                    switch(page)
                    {
                        case 0:
                        {
                            pagein(proc, page+1);
                            pagein(proc, page);
                            pageout(proc, 11);
                            pageout(proc, 12);
                            break;
                        }
                        case 2:
                        {
                            pagein(proc, 3);
                            pagein(proc, 10);
                            pagein(proc, 11);
                            pageout(proc, 1);
                            break;
                        }
                        case 5:
                        {
                            pagein(proc, 6);
                            pageout(proc, 4);
                            pageout(proc, 10);
                            pageout(proc, 11);
                            break;
                        }
                        case 10:
                        {
                            pagein(proc, 0);
                            pageout(proc, 3);
                            pageout(proc, 4);
                            pageout(proc, 9);
                            pagein(proc, 10);
                            pagein(proc, 11);
                            break;
                        }
                        default:
                        {
                            pagein(proc, page+1);
                            pagein(proc, page);
                            pageout(proc, page-1);
                        }
                    }
                    break;
                }
                case 2: // single loop
                {
                    switch(page)
                    {
                        case 0:
                        {
                            pagein(proc, 0);
                            pagein(proc, 1);
                            pageout(proc, 8);
                            break;
                        }
                        case 8: 
                        {
                            pagein(proc, 0);
                            pageout(proc, 7);
                            pageout(proc,9);
                            break;
                        }
                        default:
                        {
                            pagein(proc, page+1);
                            pagein(proc, page);
                            pageout(proc, page-1);
                        }
                    }
                    break;
                }
                case 3: //Double Nested Loop
                {
                    switch(page)
                    {
                        case 0:
                        {
                            pagein(proc, 1);
                            pageout(proc, 13);
                            break;
                        }
                        case 9:
                        {
                            pageout(proc, 0);
                            pageout(proc, 13);
                            pagein(proc, 10);
                            pagein(proc, 9);
                            pageout(proc, 8);
                            break;
                        }
                        case 12:
                        {
                            pagein(proc, 0);
                            pagein(proc, 9);
                            pagein(proc, 13);
                            pageout(proc, 11);
                            break;
                        }
                        default:
                        {
                            pagein(proc, page+1);
                            pagein(proc, page);
                            pageout(proc, page-1);
                        }
                    }
                    break;
                }
                //case 4 would go here but is linear so we will just use the linear default
                
                case 5://probalistic backwards branch but literally a single loop.
                {
                    switch(page)
                    {
                        case 0:
                        {
                            pagein(proc, 0);
                            pagein(proc, 1);
                            pageout(proc, 3);
                            break;
                        }
                        case 3: 
                        {
                            pagein(proc, 0);
                            pageout(proc, 2);
                            pageout(proc,4);
                            break;
                        }
                        default:
                        {
                            pagein(proc, page+1);
                            pagein(proc, page);
                            pageout(proc, page-1);
                        }
                    }
                    break;
                    break;
                }
                default:
                {
                    //undecided type go linear
                    pagein(proc, page+1);
                    pagein(proc, page);
                    pageout(proc, page-1);
                }
            }
        }
    }

    /* advance time for next pageit iteration */
    tick++;
} 