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
    int bit_index = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
    int curr_offset;
    int curr_addy = 0;
    while (bit_index >= 0){
        curr_offset = virtualAddress >> bit_index & (2 ^ OFFSET_WIDTH - 1);
        PMread(curr_addy * PAGE_SIZE + curr_offset, &curr_addy);
        if (curr_addy == 0){
            int f = FindPageToEvict();
            PMwrite(curr_addy * PAGE_SIZE + curr_offset, f);
            curr_addy = f;
        }
        bit_index -= OFFSET_WIDTH;
    }
    PMread(curr_addy * PAGE_SIZE + curr_offset, value);
}

int VMwrite(uint64_t virtualAddress, word_t value){

}

