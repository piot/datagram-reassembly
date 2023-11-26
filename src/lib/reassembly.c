/*----------------------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved. https://github.com/piot/datagram-reassembly
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------------------*/
#include <datagram-reassembly/reassembly.h>
#include <flood/in_stream.h>
#include <imprint/tagged_allocator.h>

/// Initialize a blob stream
/// Allocates memory for a blob stream which is later defined by calling
/// datagramReassemblySetChunk().
/// @param self incoming blob stream
/// @param blobAllocator allocator for the target blob, freed in
/// datagramReassemblyDestroy()
/// @param maxOctetCount the total size of the blob to be received.
void datagramReassemblyInit(DatagramReassembly* self,
    struct ImprintAllocatorWithFree* blobAllocator, size_t maxOctetCount, Clog log)
{
    self->log = log;
    self->blob
        = IMPRINT_ALLOC((ImprintAllocator*)blobAllocator, maxOctetCount, "blob stream in payload");
    self->maxOctetCount = maxOctetCount;
    self->isComplete = false;
    self->blobAllocator = blobAllocator;
    self->expectingSequenceId = 0;
    self->receivedOctetCount = 0;
    self->expectingPart = 0;
    self->isReceivingSequence = false;

    CLOG_C_VERBOSE(&self->log, "initialize. maximum %zu octets", self->maxOctetCount)
}

/// Frees the blob memory
void datagramReassemblyDestroy(DatagramReassembly* self)
{
    IMPRINT_FREE(self->blobAllocator, self->blob);
    self->blob = 0;
}

/// Checks if the blob stream is complete
/// @param self incoming blob stream
/// @return true if blob stream is completely received.
bool datagramReassemblyIsComplete(const DatagramReassembly* self)
{
    return self->isComplete;
}

int datagramReassemblyReceive(DatagramReassembly* self, FldInStream* inStream)
{
    uint16_t sequenceId;
    int readErr = fldInStreamReadUInt16(inStream, &sequenceId);
    if (readErr < 0) {
        CLOG_NOTICE("could not read receive %hu", sequenceId)
        return readErr;
    }
    CLOG_VERBOSE("sequenceId: %hu", sequenceId)

    uint8_t partWithMask;
    int readLengthErr = fldInStreamReadUInt8(inStream, &partWithMask);
    if (readLengthErr < 0) {
        return readLengthErr;
    }

    uint8_t part = partWithMask & 0x7f;
    CLOG_VERBOSE("part: %hhu", part)

    if (sequenceId != self->expectingSequenceId || !self->isReceivingSequence) {
        if (part != 0) {
            // we received the middle of something else
            // return
            return 0;
        }

        self->expectingSequenceId = sequenceId;
        self->expectingPart = 0;
        self->isReceivingSequence = true;
        self->receivedOctetCount = 0;
        CLOG_VERBOSE("found new sequence")
    }

    if (part != self->expectingPart) {
        return 0;
    }

    uint16_t octetCountThatFollows;
    int readOctetCounthErr = fldInStreamReadUInt16(inStream, &octetCountThatFollows);
    if (readOctetCounthErr < 0) {
        return readOctetCounthErr;
    }

    CLOG_VERBOSE("octets count follows: %hu", octetCountThatFollows)
    uint8_t* target = self->blob + self->receivedOctetCount;
    fldInStreamReadOctets(inStream, target, octetCountThatFollows);
    self->receivedOctetCount += octetCountThatFollows;
    CLOG_VERBOSE("octets received so far: %zu", self->receivedOctetCount)

    self->expectingPart++;

    bool isLastPart = (partWithMask & 0x80) != 0;
    if (isLastPart) {
        self->isComplete = true;
        self->isReceivingSequence = false;
        self->expectingPart = 0;
        CLOG_VERBOSE("last part found!")
    }

    return 0;
}

/// returns a debug string of the state of the blob stream. Not implemented.
/// @param self incoming blob stream
/// @param buf target char buffer
/// @param maxBuf maximum number of characters in buf
/// @return buf pointer
const char* datagramReassemblyToString(const DatagramReassembly* self, char* buf, size_t maxBuf)
{
    (void)self;
    (void)maxBuf;

    buf[0] = 0;
    return buf;
}
