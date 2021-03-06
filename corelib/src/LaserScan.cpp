/*
Copyright (c) 2010-2016, Mathieu Labbe - IntRoLab - Universite de Sherbrooke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Universite de Sherbrooke nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <rtabmap/core/LaserScan.h>
#include <rtabmap/utilite/ULogger.h>
#include <rtabmap/utilite/UConversion.h>

namespace rtabmap {

int LaserScan::channels(Format format)
{
	int channels=0;
	switch (format) {
		case kXY:
			channels = 2;
			break;
		case kXYZ:
		case kXYI:
			channels = 3;
			break;
		case kXYZI:
		case kXYZRGB:
			channels = 4;
			break;
		case kXYNormal:
			channels = 5;
			break;
		case kXYZNormal:
		case kXYINormal:
			channels = 6;
			break;
		case kXYZINormal:
		case kXYZRGBNormal:
			channels = 7;
			break;
		default:
			UFATAL("Unhandled type %d!", (int)format);
			break;
	}
	return channels;
}

bool LaserScan::isScan2d(const Format & format)
{
	return format==kXY || format==kXYI || format == kXYNormal || format == kXYINormal;
}
bool LaserScan::isScanHasNormals(const Format & format)
{
	return format==kXYZNormal || format==kXYZINormal || format==kXYZRGBNormal || format == kXYNormal || format == kXYINormal;
}
bool LaserScan::isScanHasRGB(const Format & format)
{
	return format==kXYZRGB || format==kXYZRGBNormal;
}
bool LaserScan::isScanHasIntensity(const Format & format)
{
	return format==kXYZI || format==kXYZINormal || format == kXYI || format == kXYINormal;
}

LaserScan LaserScan::backwardCompatibility(const cv::Mat & oldScanFormat, int maxPoints, int maxRange, const Transform & localTransform)
{
	if(!oldScanFormat.empty())
	{
		if(oldScanFormat.channels() == 2)
		{
			return LaserScan(oldScanFormat, maxPoints, maxRange, kXY, localTransform);
		}
		else if(oldScanFormat.channels() == 3)
		{
			return LaserScan(oldScanFormat, maxPoints, maxRange, kXYZ, localTransform);
		}
		else if(oldScanFormat.channels() == 4)
		{
			return LaserScan(oldScanFormat, maxPoints, maxRange, kXYZRGB, localTransform);
		}
		else if(oldScanFormat.channels() == 5)
		{
			return LaserScan(oldScanFormat, maxPoints, maxRange, kXYNormal, localTransform);
		}
		else if(oldScanFormat.channels() == 6)
		{
			return LaserScan(oldScanFormat, maxPoints, maxRange, kXYZNormal, localTransform);
		}
		else if(oldScanFormat.channels() == 7)
		{
			return LaserScan(oldScanFormat, maxPoints, maxRange, kXYZRGBNormal, localTransform);
		}
	}
	return LaserScan();
}

LaserScan::LaserScan() :
		maxPoints_(0),
		maxRange_(0),
		format_(kUnknown),
		localTransform_(Transform::getIdentity())
{
}

LaserScan::LaserScan(const cv::Mat & data, int maxPoints, float maxRange, Format format, const Transform & localTransform) :
	data_(data),
	maxPoints_(maxPoints),
	maxRange_(maxRange),
	format_(format),
	localTransform_(localTransform)
{
	UASSERT(data.empty() || data.rows == 1);
	UASSERT(data.empty() || data.type() == CV_8UC1 || data.type() == CV_32FC2 || data.type() == CV_32FC3 || data.type() == CV_32FC(4) || data.type() == CV_32FC(5) || data.type() == CV_32FC(6)  || data.type() == CV_32FC(7));
	UASSERT(!localTransform.isNull());

	if(!data.empty() && !isCompressed())
	{
		if(format == kUnknown)
		{
			*this = backwardCompatibility(data_, maxPoints_, maxRange_, localTransform_);
		}
		else // verify that format corresponds to expected number of channels
		{
			UASSERT_MSG(data.channels() != 2 || (data.channels() == 2 && format == kXY), uFormat("format=%d", format).c_str());
			UASSERT_MSG(data.channels() != 3 || (data.channels() == 3 && (format == kXYZ || format == kXYI)), uFormat("format=%d", format).c_str());
			UASSERT_MSG(data.channels() != 4 || (data.channels() == 4 && (format == kXYZI || format == kXYZRGB)), uFormat("format=%d", format).c_str());
			UASSERT_MSG(data.channels() != 5 || (data.channels() == 5 && (format == kXYNormal)), uFormat("format=%d", format).c_str());
			UASSERT_MSG(data.channels() != 6 || (data.channels() == 6 && (format == kXYINormal || format == kXYZNormal)), uFormat("format=%d", format).c_str());
			UASSERT_MSG(data.channels() != 7 || (data.channels() == 7 && (format == kXYZRGBNormal || format == kXYZINormal)), uFormat("format=%d", format).c_str());
		}
	}
}

}
