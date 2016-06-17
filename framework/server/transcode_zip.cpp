/*
 * zip_transcode.cpp
 *
 *  Created on: Nov 8, 2013
 *      Author: jszhang
 */

#include "transcode_zip.h"

ZipTranscode::ZipTranscode () {
}

ZipTranscode::ZipTranscode (Transcode* kernel) : Transcode(kernel) {
}

ZipTranscode::~ZipTranscode () {

}

BinaryData* ZipTranscode::doEncode (const BinaryData* message) {
	return nullptr;
}

BinaryData* ZipTranscode::doDecode (const BinaryData* message) {
	return nullptr;
}

