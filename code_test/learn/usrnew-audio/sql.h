#include "public.h"
#include "AlarmRecord.h"
#define MAX_COLUMN	(37)
#define MALLOC_SIZE	(5120)
#define MAX_COUNT	(10000)
#define INTEGRALITY_OK		(MAX_COUNT + 1)
#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))
#define OK   0
#define FAIL 1

#define RESULT_SQL_OK  2
#define RESULT_SQL_FAIL  -2



extern void print_result(void);
extern inline void dump_buff(char *title, char *buff, int size);

extern int search_db(const char *key, const char *word, int n, ChargeRecordTP msg[], const char *table);
extern int readnlatest(int n, ChargeRecordTP msg[], const char *table);
extern int insert_db(ChargeRecordTP *record, const char *table);
extern int get_count(const char *table);
extern int create_table(const char *table);
extern int delete_db(const char *table);
extern int execute_sql(const char *sqlstr);

extern int init_db(const char *dbname, const char *table);
extern void rm_db(void);
extern char *bcd2str(char *dest,const char *bcd, int dot, int size);
/**********************�澯��Ϣ**********************/
#define MAX_ALARM_RECORD_COUNT  10000

int init_alarm_db(const char *dbname);
void rm_alarm_db(void);
int insertAlarmRecord(AlarmRecordTP *record, const char *table);
int SearchAlarmRecord(ST_CHAR *time, const ST_UCHAR *AlarmCode, AlarmRecordTP record[], int n, const char *table);

int create_alarm_table(const char *table);
