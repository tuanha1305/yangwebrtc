#ifndef INCLUDE_YANGPUSH_YANGSENDVIDEOI_H_
#define INCLUDE_YANGPUSH_YANGSENDVIDEOI_H_
#include <stdint.h>
class YangSendVideoI{
public:
	YangSendVideoI(){};
	virtual ~YangSendVideoI(){};
	virtual void putVideoRgba(uint8_t* data,int len)=0;
	virtual void putVideoI420(uint8_t* data,int len)=0;
};




#endif /* INCLUDE_YANGPUSH_YANGSENDVIDEOI_H_ */
