//
// Created by Guy Harel on 6/14/2023.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"





int FindPageToEvict(){

}


void VMinitialize() {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        // TODO - where should I zero? does PMwrite automatically do it for words?
        PMwrite(i, 0);
    }
}


int VMread(uint64_t virtualAddress, word_t* value){
}

int VMwrite(uint64_t virtualAddress, word_t value){
    int bit_index = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
    int curr_offset;
    int curr_addy = 0;
    while (bit_index >= 0){
        curr_offset = virtualAddress >> bit_index & (2 ^ OFFSET_WIDTH - 1);
        PMread(curr_addy * PAGE_SIZE + curr_offset, &curr_addy);
        if (curr_addy == 0){
            int f = FindPageToEvict();
            // TODO Make sure we did not evict somewhere we already visited (one of the previous fs)
            if (bit_index >= OFFSET_WIDTH){
                for (int i = 0; i < PAGE_SIZE; ++i) {
                    PMwrite(f * PAGE_SIZE + i, 0);
                }
            } else {
                //TODO - not all the way there yet
                PMrestore(f, curr_addy);
            }
            PMwrite(curr_addy * PAGE_SIZE + curr_offset, f);
            curr_addy = f;
        }
        bit_index -= OFFSET_WIDTH;
    }
    PMwrite(curr_addy * PAGE_SIZE + curr_offset, value);
    return 1;
}

