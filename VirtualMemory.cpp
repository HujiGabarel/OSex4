//
// Created by Guy Harel on 6/14/2023.
//

#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#define abs(a) max(a, -a)
uint64_t FindPageToEvict(uint64_t);

//TODO should I save father page?
void TraverseTree(word_t frame_index, int depth, word_t *max_frame_index,
                  word_t *frame_found_index) {
    if (depth + 1 >= VIRTUAL_ADDRESS_WIDTH / OFFSET_WIDTH) {
        return;
    }

    bool is_empty = true;
    word_t res = 0;
    for (int i = 0; i < PAGE_SIZE; i++) {
        //TODO - memory counted by word size? i * sizeof(word_t)?
        PMread(frame_index * PAGE_SIZE + i, &res);
        if (res != 0) {
            is_empty = false;
            TraverseTree(res, depth + 1, max_frame_index, frame_found_index);
            if (*frame_found_index != 0) {
                return;
            }
            if (res > *max_frame_index) {
                *max_frame_index = res;
            }
        }
    }
    if (is_empty && *frame_found_index == 0) {
        *frame_found_index = frame_index;
        return;
    }

}

word_t FindUnusedFrameIndex() {
    word_t frame_found_index = 0;
    word_t max_index = 0;
    TraverseTree(0, 0, &frame_found_index, &max_index);
    if (frame_found_index != 0) {
        return frame_found_index;
    }
    if (max_index + 1 < NUM_FRAMES) {
        return (max_index + 1);
    }
    return 0;
}


void VMinitialize() {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        // TODO - where should I zero? does PMwrite automatically do it for words?
        PMwrite(i, 0);
    }
}


int VMread(uint64_t virtualAddress, word_t *value) {
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    int bit_index = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
    int curr_offset;
    int curr_addy = 0;
    while (bit_index >= 0) {
        curr_offset = virtualAddress >> bit_index & (2 ^ OFFSET_WIDTH - 1);
        PMread(curr_addy * PAGE_SIZE + curr_offset, &curr_addy);
        if (curr_addy == 0) {
            int f = FindUnusedFrameIndex();
            if (f == 0) {
                PMevict(FindPageToEvict(0), 0); //TOdo - where to evict?
            }
            // TODO Make sure we did not evict somewhere we already visited (one of the previous fs)
            if (bit_index >= OFFSET_WIDTH) {
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

uint64_t FindPageToEvict(int index) {
    uint64_t min_frame_index = 0;
    uint64_t max_frame_distance = 0;
    for (int i = 0; i < NUM_FRAMES; ++i)
        max_frame_distance = max(max_frame_distance, min(abs(index - i), abs(NUM_FRAMES - index + i)));
}

