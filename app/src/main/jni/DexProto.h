//
// Created by hewei05 on 16/1/17.
//

#ifndef PLUGDEMO_DEXPROTO_H
#define PLUGDEMO_DEXPROTO_H

#include "DexFile.h"

/*
 * Method prototype structure, which refers to a protoIdx in a
 * particular DexFile.
 */
struct DexProto {
    const DexFile* dexFile;     /* file the idx refers to */
    u4 protoIdx;                /* index into proto_ids table of dexFile */
};

#endif //PLUGDEMO_DEXPROTO_H
