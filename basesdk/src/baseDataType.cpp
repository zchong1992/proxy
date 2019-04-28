#include "baseDataType.h"
#include "log.h"
using namespace std;
namespace baseservice
{

baseKV_s::baseKV_s() {
	memset(this, 0, sizeof(baseKV_s));
}
baseKV_s::~baseKV_s() {
	if (data)
		delete (char*)data;
}

memKV_s::memKV_s() {
	memset(this, 0, sizeof(memKV_s));
}
memKV_s::~memKV_s() {
	if (buf)
		delete  (char*)buf;
}
baseImg_s::baseImg_s() {
	memset(this, 0, sizeof(struct baseImg_s));
}
baseImg_s::~baseImg_s() {
	if (data)
		delete  (char*)data;
}

baseImg_s::baseImg_s(const struct baseImg_s &A) {
	memset(this, 0, sizeof(struct baseImg_s));
	copy(A);
}
const struct baseImg_s &baseImg_s::copy(const struct baseImg_s &A) {
	Realease();
	setData(A.data, A.width, A.height, A.channels, A.widthStep);
	return *this;
}
const struct baseImg_s &baseImg_s::operator=(const struct baseImg_s &A) {
	Realease();
	setData(A.data, A.width, A.height, A.channels, A.widthStep);
	return *this;
}
void baseImg_s::setData(const void *data, long width, long height, long channels, long widthStep) {
	Realease();
	if (data == 0 || width == 0 || height == 0 || channels == 0) {
		SYS_LOG(ZLOGERROR, "data==%d||width==%d||heigth==%d||channels==%d true\n",
				data, width, height, channels);
		return;
	}
	if (widthStep == 0)
		widthStep = width * channels;
	this->data = new unsigned char[widthStep * height];
	if (!this->data) {
		SYS_LOG(ZLOGERROR, "data=new[%d * %d *%d]  ; fail\n",
				width, height, widthStep);
		return;
	}
	memcpy(this->data, data, widthStep * height);
	this->width = width;
	this->height = height;
	this->channels = channels;
	this->widthStep = widthStep;
}
void baseImg_s::Realease() {
	if (data)
		delete data;
	memset(this, 0, sizeof(struct baseImg_s));
}
}; // namespace baseservice
