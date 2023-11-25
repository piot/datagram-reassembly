/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/datagram-reassembly
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#ifndef DATAGRAM_REASSEMBLY_WRITE_H
#define DATAGRAM_REASSEMBLY_WRITE_H

#include <stdint.h>
#include <stdbool.h>

struct FldOutStream;

typedef struct DatagramReassemblyWrite {
    uint16_t sequenceId;
    uint8_t part;
} DatagramReassemblyWrite;

void datagramReassemblyWriteInit(DatagramReassemblyWrite* self);
void datagramReassemblyWriteNextSequence(DatagramReassemblyWrite* self);
int datagramReassemblyWriteHeader(
    DatagramReassemblyWrite* self, struct FldOutStream* outStream, uint16_t octetCount, bool isFinalOne);

#endif
