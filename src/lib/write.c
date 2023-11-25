/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/datagram-reassembly
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <datagram-reassembly/write.h>
#include <flood/out_stream.h>

void datagramReassemblyWriteInit(DatagramReassemblyWrite* self)
{
    self->sequenceId = 0;
    self->part = 0;
}

void datagramReassemblyWriteNextSequence(DatagramReassemblyWrite* self)
{
    self->sequenceId++;
    self->part = 0;
}

int datagramReassemblyWriteHeader(
    DatagramReassemblyWrite* self, FldOutStream* outStream, uint16_t octetCount, bool isFinalOne)
{
    int readErr = fldOutStreamWriteUInt16(outStream, self->sequenceId);
    if (readErr < 0) {
        return readErr;
    }

    uint8_t partWithMask = self->part;
    if (isFinalOne) {
        partWithMask |= 0x80;
    }

    int readLengthErr = fldOutStreamWriteUInt8(outStream, partWithMask);
    if (readLengthErr < 0) {
        return readLengthErr;
    }

    int readOctetCounthErr = fldOutStreamWriteUInt16(outStream, octetCount);
    if (readOctetCounthErr < 0) {
        return readOctetCounthErr;
    }

    return 0;
}
