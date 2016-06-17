/*
 * zip_transcode.h
 *
 *  Created on: Nov 8, 2013
 *      Author: jszhang
 */

#ifndef FRAMEWORK_SERVER_TRANSCODE_ZIP_H_
#define FRAMEWORK_SERVER_TRANSCODE_ZIP_H_

#include <zlib.h>

#include "transcode.h"

#define PACKETLEN sizeof(uint32_t)

class ZipTranscode : public Transcode {
public:
	ZipTranscode ();
	explicit ZipTranscode (Transcode* kernel);
	virtual ~ZipTranscode ();

protected:
	virtual BinaryData* doEncode (const BinaryData* message);
	virtual BinaryData* doDecode (const BinaryData* message);

};

#endif  // FRAMEWORK_SERVER_TRANSCODE_ZIP_H_
