
#ifndef __CDZ_CHARGERECORD_H__
#define __CDZ_CHARGERECORD_H__

#include "DataType.h"
#include "public.h"


ST_RET IsNewChargeRecord(void);


ST_RET InitNewChargeRecord(void);

ST_RET GetNewChargeRecord(ChargeRecordTP *pChargeRecord);


ST_RET GetChargeRecord(ChargeRecordTP *pChargeRecord,ST_INT32 *n);


void ConfirmChargeRecord(void);
ST_RET IsNewChargeRecord_Gprs(void);
ST_RET GetNewChargeRecord_Gprs(ChargeRecordTP *pChargeRecord);
void ConfirmChargeRecord_Gprs(void);



#endif



