//
// Created by Guy Harel on 6/14/2023.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define max(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a, b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#define abs(a) max(a, -a)

uint64_t FindPageToEvict(uint64_t);

//TODO should I save father page?
void TraverseTree(word_t frame_index, uint64_t address_of_reference, int depth, uint64_t virtual_page_index,
                  uint64_t virtual_dest_index, word_t frame_index_to_ignore, word_t *max_frame_index,
                  word_t *frame_found_index, word_t *max_dist_frame_index, uint64_t *max_dist_page_index,
                  uint64_t *max_dist, uint64_t *founded_address_of_reference) {
    if (depth >= TABLES_DEPTH) {
        uint64_t abs_dist;
        if (virtual_page_index > virtual_dest_index) {
            abs_dist = virtual_page_index - virtual_dest_index;
        }
        else {
            abs_dist = virtual_dest_index - virtual_page_index;
        }
        uint64_t cyc_dist = min(abs_dist, NUM_PAGES - abs_dist);
        if (cyc_dist > *max_dist) {
            *max_dist_frame_index = frame_index;
            *max_dist_page_index = virtual_page_index;
            *max_dist = cyc_dist;
            *founded_address_of_reference = address_of_reference;
        }
        return;
    }

    bool is_empty = true;
    word_t res = 0;
    for (uint64_t i = 0; i < PAGE_SIZE; i++) {
        //TODO - memory counted by word size? i * sizeof(word_t)?
        uint64_t son_index_reference = frame_index * PAGE_SIZE + i;
        PMread(son_index_reference, &res);
        if (res != 0) {
            is_empty = false;
            TraverseTree(res, son_index_reference, depth + 1, virtual_page_index * PAGE_SIZE + i,
                         virtual_dest_index, frame_index_to_ignore, max_frame_index, frame_found_index, max_dist_frame_index,
                         max_dist_page_index, max_dist, founded_address_of_reference);
            if (*frame_found_index != 0) {
                return;
            }
            if (res > *max_frame_index) {
                *max_frame_index = res;
            }
        }
    }
    if (is_empty && (*frame_found_index == 0) && (frame_index != frame_index_to_ignore)){
        *frame_found_index = frame_index;
        *founded_address_of_reference = address_of_reference;
        return;
    }

}

word_t GetEmptyFrameIndex(uint64_t virtual_dest_index, word_t frame_index_to_ignore) {
    word_t frame_found_index = 0;
    word_t max_index = 0;
    word_t max_dist_frame_index = 0;
    uint64_t founded_address_of_reference = 0;
    uint64_t max_dist_page_index = 0;
    uint64_t max_dist = 0;
    TraverseTree(0, 0, 0, 0, virtual_dest_index, frame_index_to_ignore, &max_index, &frame_found_index, &max_dist_frame_index,
                 &max_dist_page_index, &max_dist, &founded_address_of_reference);
    if (frame_found_index != 0) {
        PMwrite(founded_address_of_reference, 0);
        return frame_found_index;
    }
    if (max_index + 1 < NUM_FRAMES) {
        return (max_index + 1);
    }
    if (max_dist_frame_index == 0) {
        return 0;
    }
    PMevict(max_dist_frame_index, max_dist_page_index);
    PMwrite(founded_address_of_reference, 0);
    return max_dist_frame_index;
}


void VMinitialize() {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(i, 0);
    }
}


word_t get_frame_index(uint64_t virtualAddress) {

//    int bit_index = CEIL(VIRTUAL_ADDRESS_WIDTH / (double)OFFSET_WIDTH) * OFFSET_WIDTH - OFFSET_WIDTH;
    int bit_index = TABLES_DEPTH * OFFSET_WIDTH;
    uint64_t curr_offset;
    word_t curr_frame_index = 0;
    word_t father_frame_index;
    while (bit_index > 0) {
        curr_offset = (virtualAddress >> bit_index) % PAGE_SIZE;
        father_frame_index = curr_frame_index;
        uint64_t curr_address = curr_frame_index * PAGE_SIZE + curr_offset;
        PMread(curr_address, &curr_frame_index);
        if (curr_frame_index == 0) {
            word_t f = GetEmptyFrameIndex(virtualAddress >> OFFSET_WIDTH, father_frame_index);
            if (bit_index > OFFSET_WIDTH) {
                for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
                    PMwrite(f * PAGE_SIZE + i, 0);
                }
            } else {
                PMrestore(f, (virtualAddress >> OFFSET_WIDTH));
            }
            PMwrite(curr_address, f);
            curr_frame_index = f;
        }
        bit_index -= OFFSET_WIDTH;
    }
    return curr_frame_index;
}


int VMread(uint64_t virtualAddress, word_t *value) {
    if (virtualAddress >= VIRTUAL_MEMORY_SIZE || TABLES_DEPTH >= NUM_FRAMES) {
        return 0;
    }
    word_t curr_frame_index = get_frame_index(virtualAddress);
    uint64_t curr_offset = (virtualAddress) % PAGE_SIZE;
    PMread(curr_frame_index * PAGE_SIZE + curr_offset, value);
    return 1;
    //TODO - return 0 if failed
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    if (virtualAddress >= VIRTUAL_MEMORY_SIZE  || TABLES_DEPTH >= NUM_FRAMES) {
        return 0;
    }
    word_t curr_frame_index = get_frame_index(virtualAddress);
    uint64_t curr_offset = (virtualAddress) % PAGE_SIZE;
    PMwrite(curr_frame_index * PAGE_SIZE + curr_offset, value);
    return 1;
    //TODO - return 0 if failed
}

