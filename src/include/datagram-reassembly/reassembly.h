/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/datagram-reassembly
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef DATAGRAM_REASSEMBLY_H
#define DATAGRAM_REASSEMBLY_H

#include <clog/clog.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct ImprintAllocator;
struct ImprintAllocatorWithFree;
struct FldInStream;

typedef struct DatagramReassembly {
    size_t maxOctetCount;
    uint8_t* blob;
    bool isComplete;
    uint16_t expectingSequenceId;
    size_t receivedOctetCount;
    uint8_t expectingPart;
    bool isReceivingSequence;

    struct ImprintAllocatorWithFree* blobAllocator;
    Clog log;
} DatagramReassembly;

void datagramReassemblyInit(DatagramReassembly* self,
    struct ImprintAllocatorWithFree* blobAllocator, size_t totalOctetCount, Clog log);
void datagramReassemblyDestroy(DatagramReassembly* self);
void datagramReassemblyReset(DatagramReassembly* self);
bool datagramReassemblyIsComplete(const DatagramReassembly* self);
int datagramReassemblyReceive(DatagramReassembly* self, struct FldInStream* inStream);
const char* datagramReassemblyToString(const DatagramReassembly* self, char* buf, size_t maxBuf);

#endif
