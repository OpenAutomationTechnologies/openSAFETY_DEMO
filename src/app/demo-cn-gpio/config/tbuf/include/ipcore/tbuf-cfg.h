//-------------------------------------------------------------------------
// DO NOT MODIFY THIS FILE!
// This file is generated automatically depending on the ipcore settings!
// Hence, it is highly recommended to avoid manual modifications!
//
// timestamp = 2013-12-18 14:03:02
//-------------------------------------------------------------------------

#ifndef _INC_tbuf_cfg_H_
#define _INC_tbuf_cfg_H_

#define TBUF_OFFSET_CONACK 0
#define TBUF_SIZE_CONACK 4
#define TBUF_PORTA_ISPRODUCER_CONACK -1

#define TBUF_OFFSET0 4
#define TBUF_SIZE0 12
#define TBUF_PORTA_ISPRODUCER0 0

#define TBUF_OFFSET1 16
#define TBUF_SIZE1 12
#define TBUF_PORTA_ISPRODUCER1 0

#define TBUF_OFFSET2 28
#define TBUF_SIZE2 8
#define TBUF_PORTA_ISPRODUCER2 0

#define TBUF_OFFSET3 36
#define TBUF_SIZE3 4
#define TBUF_PORTA_ISPRODUCER3 1

#define TBUF_OFFSET4 40
#define TBUF_SIZE4 12
#define TBUF_PORTA_ISPRODUCER4 1

#define TBUF_OFFSET5 52
#define TBUF_SIZE5 4
#define TBUF_PORTA_ISPRODUCER5 1

#define TBUF_OFFSET_PROACK 56
#define TBUF_SIZE_PROACK 4
#define TBUF_PORTA_ISPRODUCER_PROACK -1

#define TBUF_NUM_CON 3
#define TBUF_NUM_PRO 3

#define TBUF_INIT_VEC { \
                        { TBUF_OFFSET_CONACK, TBUF_SIZE_CONACK, TBUF_PORTA_ISPRODUCER_CONACK },  \
                        { TBUF_OFFSET0, TBUF_SIZE0, TBUF_PORTA_ISPRODUCER0 },  \
                        { TBUF_OFFSET1, TBUF_SIZE1, TBUF_PORTA_ISPRODUCER1 },  \
                        { TBUF_OFFSET2, TBUF_SIZE2, TBUF_PORTA_ISPRODUCER2 },  \
                        { TBUF_OFFSET3, TBUF_SIZE3, TBUF_PORTA_ISPRODUCER3 },  \
                        { TBUF_OFFSET4, TBUF_SIZE4, TBUF_PORTA_ISPRODUCER4 },  \
                        { TBUF_OFFSET5, TBUF_SIZE5, TBUF_PORTA_ISPRODUCER5 },  \
                        { TBUF_OFFSET_PROACK, TBUF_SIZE_PROACK, TBUF_PORTA_ISPRODUCER_PROACK } \
                      }

#endif /* _INC_tbuf_cfg_H_ */