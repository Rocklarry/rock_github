#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <pthread.h>
#include "sql.h"


struct result {
    int nrow;
    int ncol;
    char **azResult;
    char *zErrMsg;
    int listCnt;        //当前记录条数
};

static struct result db_result;
static sqlite3 *db	= NULL;
static char *sql	= NULL;

/***********************************************************************
 * Function:	dump_buff
 * Purpose:		打印一段内存数据
 * Input:		title -- 打印标题
 *				buff  -- 内存地址
 *				size  -- 内存长度
 * Output:		void
***********************************************************************/
inline void dump_buff(char *title, char *buff, int size)
{
#ifdef DEBUG_DB
    static int BARWIDTH = 32;
    int j, i;
    for(i = 0; i < (BARWIDTH - strlen(title))/2; i++)
        printf("=");
    printf("%s", title);
    for( ;i < BARWIDTH; i++)
        printf("=");
    printf("\n");
    i = j = 0;
    while(i < size) {
        printf("[%2d] ", i);
        for(j =0; i < size && j < 8; j++, i++) {
            printf("%02x ", buff[i]);
        }
        printf("\n");
    }
#endif
}
static int dec2bcd(int dec, int *bcd)
{
    int ret = 0, ls= 0;
    while (dec > 0) {
        ret |= (dec % 10) << ls;
        dec /= 10;
        ls += 4;
    }
    if (bcd)    *bcd = ret;
    return ret;
}
/***********************************************************************
 * Function:	bcd2str
 * Purpose:		将一串bcd码转换为字符串
 * Input:		dest  -- 目标字符串地址
 *				bcd   -- bcd码起始地址
 *				dot	  -- 小数点位置[0表示忽略]
 *				size  -- bcd码长度
 * Output:		char* -- 返回目标字符串地址, 出错NULL
***********************************************************************/
char *bcd2str(char *dest, const char *bcd, int dot, int size)
{ /**** Make sure dest[] space is enough ***/
    int i;
    char str[3];

    memset(dest, 0, size * 2 + 1);
	for(i = 0; i < size; i++) {
		if((bcd[i] >> 4) > 9 || (bcd[i] & 0x0f) > 9) {
			printf("Error: 0x%02x not BCD data\n", bcd[i]);
			return NULL;
		}
		sprintf(str, "%02x", bcd[i]);
		if(i == dot && dot > 0) strcat(dest, ".");
		strcat(dest, str);
	}
    return dest;
}



/***********************************************************************
 * Function:	str2bcd
 * Purpose:		将字符串转换为bcd
 * Input:		bcd   -- bcd码起始地址
 *				str   -- 字符串地址
 *				size  -- 字符串长度
 * Output:		<0    -- 出错
 *				>=0	  -- 有效的bcd码长度
***********************************************************************/
static int str2bcd(char *bcd, char *str, int size)
{
    int i;
    int j;
    int ret;
    char *p;

    p = malloc (size);
    if (!p) {
        fprintf (stderr, "Error: %s, malloc fail\n", __func__);
        goto err0;
    }
    memset (p, 0, size);

    for (i = 0, ret = 0; i < size && str[i] != '\0'; i++, ret++) {
        if (str[i] == '.')  {
            size += 1;  continue;
        }

        if (str[i] > '9' || str[i] < '0') {
            fprintf (stderr, "Error: %s, %c not BCD code\n", __func__, str[i]);
            goto err1;
        }
        p[ret] = str[i] - '0';
    }

    for (i = 0, j = 0; i < ret; i += 2, j++) {
        bcd[j] = p[i] << 4 | p[i+1];
    }

    free (p);
    return OK;
err1:
    free (p);
err0:
    return FAIL;
}
/***********************************************************************
 * Function:	dec2str
 * Purpose:		将一段内存数据转换为字符串
 * Input:		val  -- 待转换内存数据起始地址
 *				size  -- 字符串长度
 * Output:		dest  -- 输出的字符串地址
 *				返回内存数据的int形式
***********************************************************************/
static int dec2str(char *dest,const char *val, int size)
{
	int i;
	int ret = 0;

	for(i = 0; i < size; i++) {
		ret += *(val+i) << (8*i);
	}

	sprintf(dest, "%d", ret);

	return ret;
}
/***********************************************************************
 * Function:	str2dec
 * Purpose:		将字符串转换为int型整数
 * Input:		str  -- 字符串地址
 *				size  -- 字符串长度
 * Output:		返回转换后的int型整数
 *				dest  -- 保存int整数
***********************************************************************/
static int str2dec(char *dest, char *str, int size)
{
	char tmp[32] = { 0 };
	int ret = 0;

	if (size > sizeof(tmp)-1) {
		printf("Error: tmp[] size not enough\n");
		size = sizeof(tmp)-1;
	}

	strncpy(tmp, str, size);

	ret = atoi(tmp);

	memcpy(dest, &ret, sizeof(ret));

	return ret;
}


/*====================================================================*/
static void begin_trans(void)
{
#if 0
	sqlite3_exec(db,"begin transaction", NULL, NULL, NULL);
#endif
}
static void commit_trans(void)
{
#if 0
	sqlite3_exec(db,"commit transaction", NULL, NULL, NULL);
#endif
}

int execute_sql(const char *sqlstr)
{
	dbg("SQL='%s'\n", sqlstr);
	return sqlite3_get_table(db, sqlstr, &(db_result.azResult), &(db_result.nrow), &(db_result.ncol), &(db_result.zErrMsg));
}
/***********************************************************************
 * Function:	get_count
 * Purpose:		获得记录条数
 * Input:		表名称
 * Output:		-1  --  Fail
 *				0   --  记录条数
***********************************************************************/
int get_count(const char *table)
{
	strcpy(sql, "SELECT name FROM SQLITE_MASTER");
	if (execute_sql(sql) != SQLITE_OK) {
		printf("Error: %s\n", db_result.zErrMsg);
		return -1;
	}

	if (!db_result.nrow)	return 0;

	sprintf(sql, "SELECT count(*) FROM %s", table);
	execute_sql(sql);
	return atoi(db_result.azResult[1]);
}
/***********************************************************************
 * Function:	delete_db
 * Purpose:		删除表内所有记录
 * Input:		表名称
 * Output:		0  --  Success
 *				-1 --  Error
***********************************************************************/
int delete_db(const char *table)
{
	sprintf(sql, "DELETE FROM %s", table);
	if(sqlite3_exec(db, sql, NULL, NULL, &(db_result.zErrMsg)) != SQLITE_OK) {
		printf("Error: %s\n", db_result.zErrMsg);
		return -1;
	}
	return 0;
}
static int search_n_record(char *SQL, ChargeRecordTP msg[], int n)
{
	int i, j;

	if(execute_sql(SQL) != SQLITE_OK) {
		printf("Error: %s\n", db_result.zErrMsg);
		return -1;
	}

	for(i = 1; i <= db_result.nrow && i <= n; i++) {
		j = 1;
		str2bcd(&msg[i-1].SendHoutaiFlag, db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].SendHoutaiFlag));
		str2bcd(&msg[i-1].sExchangeType, db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sExchangeType));
		str2bcd(&msg[i-1].sExchangeSeq[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sExchangeSeq));
		str2bcd(&msg[i-1].sAreaCode[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sAreaCode));
		str2bcd(&msg[i-1].sStartCardNo[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sStartCardNo));
		str2bcd(&msg[i-1].sEndCardNo[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sEndCardNo));
		str2bcd(&msg[i-1].sStartCardType[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sStartCardType));
		str2bcd(&msg[i-1].sEndCardType[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sEndCardType));

		str2bcd(&msg[i-1].sStartEnerge[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sStartEnerge));
		str2bcd(&msg[i-1].sStartEnerge1[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sStartEnerge1));
		str2bcd(&msg[i-1].sStartEnerge2[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sStartEnerge2));
		str2bcd(&msg[i-1].sStartEnerge3[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sStartEnerge3));
		str2bcd(&msg[i-1].sStartEnerge4[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sStartEnerge4));

		str2bcd(&msg[i-1].sEndEnerge[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sEndEnerge));
		str2bcd(&msg[i-1].sEndEnerge1[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sEndEnerge1));
		str2bcd(&msg[i-1].sEndEnerge2[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sEndEnerge2));
		str2bcd(&msg[i-1].sEndEnerge3[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sEndEnerge3));
		str2bcd(&msg[i-1].sEndEnerge4[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sEndEnerge4));

		str2bcd(&msg[i-1].sPrice1[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sPrice1));
		str2bcd(&msg[i-1].sPrice2[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sPrice2));
		str2bcd(&msg[i-1].sPrice3[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sPrice3));
		str2bcd(&msg[i-1].sPrice4[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sPrice4));

		str2bcd(&msg[i-1].sParkFeePrice[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sParkFeePrice));
		str2bcd(&msg[i-1].sStartTime[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sStartTime));
		str2bcd(&msg[i-1].sEndTime[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sEndTime));
		str2bcd(&msg[i-1].sParkFee[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sParkFee));

		str2bcd(&msg[i-1].sGasPrice[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sGasPrice));
		str2bcd(&msg[i-1].sGasFee[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sGasFee));

		str2bcd(&msg[i-1].sMoneyBefore[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sMoneyBefore));
		str2bcd(&msg[i-1].sMoneyAfter[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sMoneyAfter));
		str2bcd(&msg[i-1].sCardCounter[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sCardCounter));
		str2bcd(&msg[i-1].sTermNo[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sTermNo));
		str2bcd(&msg[i-1].sCardVer[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sCardVer));
		str2bcd(&msg[i-1].sPosNo[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sPosNo));
		str2bcd(&msg[i-1].sCardStatus, db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sCardStatus));
		str2bcd(&msg[i-1].sTimeLong[0], db_result.azResult[i*db_result.ncol+j++], 2*sizeof(msg[i-1].sTimeLong));
	}
	return (db_result.nrow);
}
/***********************************************************************
 * Function:	readnlatest
 * Purpose:		获得最近的n条记录
 * Input:		n  --  所要获得的记录条数
 *				msg[] -- 记录输出指针
 *				table -- 表名称
 * Output:		>=0  --  获得的记录条数 <=n
 *				-1 --  Error
***********************************************************************/
int readnlatest(int n, ChargeRecordTP msg[], const char *table)
{
//	int i, j;

	memset(msg, 0, sizeof(ChargeRecordTP) * n);

	sprintf(sql, "SELECT * FROM %s ORDER BY [录入时间] DESC LIMIT 0,%d", table, n);
	
	return search_n_record(sql, msg, n);
}
/***********************************************************************
 * Function:	search_db
 * Purpose:		搜索相关记录, 按时间降序排列
 * Input:		key  --  字段名称
 *				word --  待搜索关键字
 *				n  --  所要获得的记录条数
 *				msg[] -- 记录输出指针
 *				table -- 表名称
 * Output:		>=0  --  获得的记录条数 <=n
 *				-1 --  Error
***********************************************************************/
int search_db(const char *key, const char *word, int n, ChargeRecordTP msg[], const char *table)
{
	int ret;
	//int i, j;

	if (!db) {
		printf("Error: open error\n");
		return -1;
	}

	if (!table) {
		printf("Error: Please input table name\n");
		return -1;
	}

	sprintf(sql, "SELECT * FROM %s WHERE %s='%s' ORDER BY [录入时间] DESC LIMIT 0,%d", table, key, word, n);

	ret = search_n_record(sql, msg, n);
	if(ret < 0) {
		return -1;
	}

	return ret;
}

static int insert_one_msg(char msg[][32], const char *table)
{
	int i;

	dbg("db: insert operation\n");
	strcpy(sql, "REPLACE INTO ");
	strcat(sql, table);
	strcat(sql, " VALUES(");
	for(i = 0; i < MAX_COLUMN; i++) {
		strcat(sql, "'");
		strcat(sql, msg[i]);
		strcat(sql, "',");
	}
	/* append current time, format "YYYY-MM-DD HH:mm:ss" */
	strcat(sql, "datetime('now', 'localtime')");
	strcat(sql, ")");
	dbg("INSERTSQL: %s\n", sql);

	if (!db) {
		printf("Error: Open error\n");
		return -1;
	}
	if(sqlite3_exec(db, sql, NULL, NULL, &(db_result.zErrMsg)) != SQLITE_OK) {
		printf("Error: %s\n", db_result.zErrMsg);
		return -1;
	} else {
		db_result.listCnt++;
	}

	return 0;
}

int insert_db(ChargeRecordTP *record, const char *table)
{
	int i;
	char tmp[20];
	char msg[MAX_COLUMN][32] = { {0},{0} };

	if (!table || !db) {
		printf("Error: open error\n");
		return RESULT_SQL_FAIL;
	}
	/*** Assemble mes ***/
	i = 1;		/* Start from msg[1], because mes[0] is id */
	bcd2str(msg[i++], &record->SendHoutaiFlag, 0, 1);
	bcd2str(msg[i++], &record->sExchangeType, 0, 1);
	bcd2str(msg[i++], &record->sExchangeSeq[0], 0, sizeof(record->sExchangeSeq));
	bcd2str(msg[i++], &record->sAreaCode[0], 0, sizeof(record->sAreaCode));
	bcd2str(msg[i++], &record->sStartCardNo[0], 0, sizeof(record->sStartCardNo));
	bcd2str(msg[i++], &record->sEndCardNo[0], 0, sizeof(record->sEndCardNo));
	bcd2str(msg[i++], &record->sStartCardType[0], 0, sizeof(record->sStartCardType));
	bcd2str(msg[i++], &record->sEndCardType[0], 0, sizeof(record->sEndCardType));

	bcd2str(msg[i++], &record->sStartEnerge[0], 0, sizeof(record->sStartEnerge));
	bcd2str(msg[i++], &record->sStartEnerge1[0], 0, sizeof(record->sStartEnerge1));
	bcd2str(msg[i++], &record->sStartEnerge2[0], 0, sizeof(record->sStartEnerge2));
	bcd2str(msg[i++], &record->sStartEnerge3[0], 0, sizeof(record->sStartEnerge3));
	bcd2str(msg[i++], &record->sStartEnerge4[0], 0, sizeof(record->sStartEnerge4));

	bcd2str(msg[i++], &record->sEndEnerge[0], 0, sizeof(record->sEndEnerge));
	bcd2str(msg[i++], &record->sEndEnerge1[0], 0, sizeof(record->sEndEnerge1));
	bcd2str(msg[i++], &record->sEndEnerge2[0], 0, sizeof(record->sEndEnerge2));
	bcd2str(msg[i++], &record->sEndEnerge3[0], 0, sizeof(record->sEndEnerge3));
	bcd2str(msg[i++], &record->sEndEnerge4[0], 0, sizeof(record->sEndEnerge4));

	bcd2str(msg[i++], &record->sPrice1[0], 1, sizeof(record->sPrice1));
	bcd2str(msg[i++], &record->sPrice2[0], 1, sizeof(record->sPrice2));
	bcd2str(msg[i++], &record->sPrice3[0], 1, sizeof(record->sPrice3));
	bcd2str(msg[i++], &record->sPrice4[0], 1, sizeof(record->sPrice4));

	bcd2str(msg[i++], &record->sParkFeePrice[0], 2, sizeof(record->sParkFeePrice));
	bcd2str(msg[i++], &record->sStartTime[0], 0, sizeof(record->sStartTime));
	bcd2str(msg[i++], &record->sEndTime[0], 0, sizeof(record->sEndTime));
	bcd2str(msg[i++], &record->sParkFee[0], 2, sizeof(record->sParkFee));

	bcd2str(msg[i++], &record->sGasPrice[0], 1, sizeof(record->sGasPrice));
	bcd2str(msg[i++], &record->sGasFee[0], 3, sizeof(record->sGasFee));

	bcd2str(msg[i++], &record->sMoneyBefore[0], 3, sizeof(record->sMoneyBefore));
	bcd2str(msg[i++], &record->sMoneyAfter[0], 3, sizeof(record->sMoneyAfter));
	bcd2str(msg[i++], &record->sCardCounter[0], 0, sizeof(record->sCardCounter));
	bcd2str(msg[i++], &record->sTermNo[0], 0, sizeof(record->sTermNo));
	bcd2str(msg[i++], &record->sCardVer[0], 0, sizeof(record->sCardVer));
	bcd2str(msg[i++], &record->sPosNo[0], 0, sizeof(record->sPosNo));
	bcd2str(msg[i++], &record->sCardStatus, 0, sizeof(record->sCardStatus));
	bcd2str(msg[i++], &record->sTimeLong[0], 0, sizeof(record->sTimeLong));
	/*** End assemble ***/

	if (db_result.listCnt < MAX_COUNT) {

		sprintf(msg[0], "%d", db_result.listCnt);

		if (insert_one_msg(msg, table)) {
			printf("Error: [0]insert error\n");
			return RESULT_SQL_FAIL;
		}

	} else {
		dbg("overlap\n");
		sprintf(sql, "SELECT [索引号] FROM %s ORDER BY [录入时间] ASC LIMIT 0,1", table);
		execute_sql(sql);
		strncpy(tmp, db_result.azResult[1], sizeof(tmp));

		db_result.listCnt--;

		strcpy(msg[0], tmp);

		if (insert_one_msg(msg, table)) {
			printf("Error: [1]insert error\n");
			return RESULT_SQL_FAIL;
		}
	}
	return 0;
}

int create_table(const char *table)
{
	if (!db || !table) {
		printf("Error: Open error\n");
		return RESULT_SQL_FAIL;
	}

	sprintf(sql, "SELECT name FROM SQLITE_MASTER WHERE name='%s'", table);
	sqlite3_get_table(db, sql,  &(db_result.azResult), &(db_result.nrow), &(db_result.ncol), NULL);
	if (db_result.nrow > 0)		// Exist
		return 0;

	sprintf(sql, "CREATE TABLE %s(\
索引号 INTEGER PRIMARY KEY,\
上传标志 VARCHAR(1),\
交易类型 VARCHAR(2),\
交易流水号 VARCHAR(10),\
地区代码 VARCHAR(6),\
开始卡号 VARCHAR(16),\
结束卡号 VARCHAR(16),\
开始卡型 VARCHAR(4),\
结束卡型 VARCHAR(4),\
开始交易电量行度 VARCHAR(10),\
开始交易费率1电量行度 VARCHAR(10),\
开始交易费率2电量行度 VARCHAR(10),\
开始交易费率3电量行度 VARCHAR(10),\
开始交易费率4电量行度 VARCHAR(10),\
结束交易电量行度 VARCHAR(10),\
结束交易费率1电量行度 VARCHAR(10),\
结束交易费率2电量行度 VARCHAR(10),\
结束交易费率3电量行度 VARCHAR(10),\
结束交易费率4电量行度 VARCHAR(10),\
交易费率1电价 VARCHAR(10),\
交易费率2电价 VARCHAR(10),\
交易费率3电价 VARCHAR(10),\
交易费率4电价 VARCHAR(10),\
停车费单价 VARCHAR(10),\
交易开始时间 VARCHAR(13),\
交易结束时间 VARCHAR(13),\
停车费 VARCHAR(10),\
燃气燃油加工费单价 VARCHAR(10),\
燃气燃油加工费代收金额 VARCHAR(10),\
交易前余额 VARCHAR(10),\
交易后余额 VARCHAR(10),\
卡交易计数器 VARCHAR(10),\
终端号 VARCHAR(10),\
卡版本号 VARCHAR(10),\
POS机号 VARCHAR(10),\
卡状态码 VARCHAR(2),\
充电时间 VARCHAR(6),\
录入时间 VARCHAR(20))", table);
	dbg("CREATESQL: %s\n", sql);

	if(sqlite3_exec(db, sql, NULL, NULL, &(db_result.zErrMsg)) != SQLITE_OK) {
		printf("Error: %s\n", db_result.zErrMsg);
		return -1;
	}

	return 0;
}

/* 检查索引号的连续性 */
static int check_integrality(const char *table)
{
	int i;

	sprintf(sql, "SELECT [索引号] FROM %s ORDER BY [索引号] ASC", table);
	execute_sql(sql);
	for(i = 1; i <= db_result.nrow; i++) {
		if(i != atoi(db_result.azResult[i]) + 1) {
			printf("warning: [索引号]=%d skipped\n", (i-1));
			return (i-1);
		} else
			continue;
	}
	return INTEGRALITY_OK;
}

static void release_space(void)
{
	sprintf(sql, "VACUUM");
	sqlite3_exec(db, sql, NULL, NULL, NULL);
}

int init_db(const char *dbname, const char *table)
{
	if (sqlite3_open(dbname, &db)) {
		printf("Error: Cannot open %s\n", sqlite3_errmsg(db));
		goto init_err;
	}
	
	if (!sql)
		sql = malloc(MALLOC_SIZE);
	if (!sql) {
		printf("Error: No enough memory\n");
		goto init_err;
	}
	dbg("Database initiation success\n");

	memset(&db_result, 0, sizeof(db_result));

	begin_trans();


	if(check_integrality(table) == INTEGRALITY_OK)
		dbg("db: integrality check OK\n");

	db_result.listCnt = get_count(table);
	dbg("listCnt=%d\n", db_result.listCnt);

	return 0;

init_err:
	sqlite3_close(db);
	return RESULT_SQL_FAIL;
}

void rm_db(void)
{
	commit_trans();
	release_space();
	sqlite3_free_table(db_result.azResult);
	free(sql);
	sql = NULL;
	sqlite3_close(db);
	memset(&db_result, 0, sizeof(db_result));
}

/*===================================================================*/
void print_result(void)
{
    int i, j;

    if(db_result.ncol == 0) {
        printf("No result match\n");
        return;
    }

print_header:
    for(i = 0; i < 1; i++) {
        for(j = 0; j < db_result.ncol; j++)
            printf("%s\t", db_result.azResult[i*db_result.ncol + j]);
        }
        printf("\n-----------------------------\n");

print_data:
    for(i = 1; i <= db_result.nrow; i++) {
        for(j = 0; j < db_result.ncol; j++)
            printf("%s\t", db_result.azResult[i*db_result.ncol + j]);

            /*  Just print breaker */
            printf("\n");
            for(j=0; j<30; j++) printf("-");
            printf("\n");
    }
}


/*=========================告警信息====================================*/
static struct result db_alarm_result;
static sqlite3 *db_alarm  = NULL;
static char *sql_alarm = NULL;
static pthread_mutex_t mutex_alarm;

int init_alarm_db(const char *dbname)
{
	if (sqlite3_open(dbname, &db_alarm)) {
		printf("Error: Cannot open %s\n", sqlite3_errmsg(db_alarm));
		goto init_err;
	}

	if (!sql_alarm)
		sql_alarm = malloc(MALLOC_SIZE);
	if (!sql_alarm) {
		printf("Error: No enough memory\n");
		goto init_err;
	}

	memset(&db_alarm_result, 0, sizeof(db_alarm_result));

	pthread_mutex_init(&mutex_alarm, NULL);

	return 0;
init_err:
	sqlite3_close(db_alarm);
	return RESULT_SQL_FAIL;
}

void rm_alarm_db(void)
{
	sqlite3_exec(db_alarm, "VACUUM", NULL, NULL, NULL);
	sqlite3_free_table(db_alarm_result.azResult);
	sqlite3_close(db_alarm);
	memset(&db_alarm_result, 0, sizeof(db_alarm_result));

	pthread_mutex_destroy(&mutex_alarm);

	free(sql_alarm);
	sql_alarm = NULL;
}

int create_alarm_table(const char *table)
{
	if (!db_alarm || !table) {
		printf("Error: Open error\n");
		return RESULT_SQL_FAIL;
	}

	sprintf(sql_alarm, "SELECT name FROM SQLITE_MASTER WHERE name='%s'", table);
        sqlite3_get_table(db_alarm, sql_alarm,  &(db_alarm_result.azResult), &(db_alarm_result.nrow), &(db_alarm_result.ncol), NULL);
        if (db_alarm_result.nrow > 0)         // Exist
                return 0;

	sprintf(sql_alarm, "CREATE TABLE %s(\
上传后台成功标记 VARCHAR(1),\
告警发生测量点号 VARCHAR(1),\
告警发生时间 VARCHAR(5),\
告警编码 VARCHAR(2),\
告警参数内容 VARCHAR(15),\
告警参数内容长度 VARCHAR(5),\
录入时间 VARCHAR(20) PRIMARY KEY)", table);
	dbg("CREATESQL: %s\n", sql_alarm);

	if(sqlite3_exec(db_alarm, sql_alarm, NULL, NULL, &(db_alarm_result.zErrMsg)) != SQLITE_OK) {
		printf("Error: %s\n", db_alarm_result.zErrMsg);
		return RESULT_SQL_FAIL;
	}

	return 0;
}
/***********************************************************************
 * Function:		get_alarmrecord_count
 * Purpose:			获得记录总条数
 * Input:			table  --  数据表名称
 * Output:			int    --  记录总条数
***********************************************************************/
static int get_alarmrecord_count(const char *table)
{
	int ret;
	char sql[256];
	struct result result_tmp;

	if (!db_alarm || !table) {
		printf("Error: Open error\n");
		return RESULT_SQL_FAIL;
	}

	sprintf(sql, "SELECT count(*) FROM %s", table);

	if(sqlite3_get_table(db_alarm, sql, &(result_tmp.azResult), &(result_tmp.nrow), &(result_tmp.ncol), &(result_tmp.zErrMsg)) != SQLITE_OK) {
		printf("Error: %s\n", result_tmp.zErrMsg);
		exit(1);
	}
	ret = atoi(result_tmp.azResult[1]);

	sqlite3_free_table(result_tmp.azResult);

	return ret;
}
/***********************************************************************
 * Function:		insertAlarmRecord
 * Purpose:			插入一条告警数据记录
 * Input:			record	--	新记录指针
 *					table	--	数据表名称
 * Output:			0		--	插入成功
***********************************************************************/
int insertAlarmRecord(AlarmRecordTP *record, const char *table)
{
	int i;
	char msg[6][32] = { {0},{0} };

	bcd2str(msg[0], &record->SendFlag, 0, sizeof(record->SendFlag));
	bcd2str(msg[1], &record->MeaPointNum, 0, sizeof(record->MeaPointNum));
	bcd2str(msg[2], (ST_CHAR *)&record->Time[0], 0, sizeof(record->Time));
	//bcd2str(msg[3], &record->AlarmCode[0], 0, sizeof(record->AlarmCode));
	dec2str(msg[3], (ST_CHAR *)&record->AlarmCode[0], sizeof(record->AlarmCode));
	bcd2str(msg[4], (ST_CHAR *)&record->AlarmContent[0], 0, sizeof(record->AlarmContent));
	//bcd2str(msg[5], &record->AlarmContentLen, 0, sizeof(record->AlarmContentLen));
	dec2str(msg[5], (ST_CHAR *)&record->AlarmContentLen, sizeof(record->AlarmContentLen));

	if(!sql_alarm) {
		printf("Error: sql_alarm malloc fail\n");
		return RESULT_SQL_FAIL;
	}

	pthread_mutex_lock(&mutex_alarm);

	/****** Delete overlap record ****/
	if(get_alarmrecord_count(table) >= MAX_ALARM_RECORD_COUNT) {
		sprintf(sql_alarm, "DELETE FROM %s WHERE [录入时间] IN (SELECT [录入时间] FROM %s ORDER BY [录入时间] ASC LIMIT 0,1)", table, table);
		dbg("SQL=%s\n", sql_alarm);
		if(sqlite3_exec(db_alarm, sql_alarm, NULL, NULL, &(db_alarm_result.zErrMsg)) != SQLITE_OK) {
			printf("Error: %s\n", db_alarm_result.zErrMsg);
			return RESULT_SQL_FAIL;
		}
	}

	/**** Insert new record ***/
	strcpy(sql_alarm, "REPLACE INTO ");
	strcat(sql_alarm, table);
	strcat(sql_alarm, " VALUES(");
	for(i = 0; i < ARRAY_SIZE(msg); i++) {
		strcat(sql_alarm, "'");
		strcat(sql_alarm, msg[i]);
		strcat(sql_alarm, "',");
	}
	strcat(sql_alarm, "datetime('now', 'localtime')");
	strcat(sql_alarm, ")");
	dbg("INSERTSQL: %s\n", sql_alarm);

	if(sqlite3_exec(db_alarm, sql_alarm, NULL, NULL, &(db_alarm_result.zErrMsg)) != SQLITE_OK) {
		printf("Error: %s\n", db_alarm_result.zErrMsg);
		return RESULT_SQL_FAIL;
	}

	pthread_mutex_unlock(&mutex_alarm);

	return 0;
}
/***********************************************************************
 * Function:		getAlarmRecord
 * Purpose:			搜索指定时间和告警编号的n条记录
 * Input:			time	--	指定的时间(BCD格式)
 *					AlarmCode	--	指定的告警编号
 *					n		--	限定搜索记录条数
 *					table	--	数据表名称
 * Output:			record	--	保存搜索结果的结构体
 *					返回搜索到的记录条数
***********************************************************************/
int SearchAlarmRecord(ST_CHAR *time,const ST_UCHAR *AlarmCode, AlarmRecordTP record[], int n, const char *table)
{
	int i;
	int ret;
	char sAlarmCode[32];
	char stime[32] = { 0 };

	//bcd2str(sAlarmCode, AlarmCode, 0, sizeof(record[0].AlarmCode));
	dec2str(sAlarmCode, (ST_CHAR *)AlarmCode, sizeof(record[0].AlarmCode));
	bcd2str(stime, time, 0, sizeof(record[0].Time));

	if(!db_alarm || !table) {
		printf("Error: Open error\n");
		return RESULT_SQL_FAIL;
	}

	pthread_mutex_lock(&mutex_alarm);

	sprintf(sql_alarm, "SELECT * FROM %s WHERE [告警发生时间]='%s' AND [告警编码]='%s' ORDER BY [录入时间] DESC LIMIT 0,%d", table, stime, sAlarmCode, n);
	dbg("getAlarmRecord SQL=%s\n", sql_alarm);
	if(sqlite3_get_table(db_alarm, sql_alarm, &(db_alarm_result.azResult), &(db_alarm_result.nrow), &(db_alarm_result.ncol), &(db_alarm_result.zErrMsg)) != SQLITE_OK) {
		printf("Error: %s\n", db_alarm_result.zErrMsg);
		return RESULT_SQL_FAIL;
	}

	memset(record, 0, n * sizeof(AlarmRecordTP));
	for(i = 1; i <= db_alarm_result.nrow; i++) {
		str2bcd(&record[i-1].SendFlag, db_alarm_result.azResult[i*db_alarm_result.ncol+0], 2*sizeof(record[i-1].SendFlag));
		str2bcd(&record[i-1].MeaPointNum, db_alarm_result.azResult[i*db_alarm_result.ncol+1], 2*sizeof(record[i-1].MeaPointNum));
		str2bcd((ST_CHAR *)&record[i-1].Time[0], db_alarm_result.azResult[i*db_alarm_result.ncol+2], 2*sizeof(record[i-1].Time));
		//str2bcd(&record[i-1].AlarmCode[0], db_alarm_result.azResult[i*db_alarm_result.ncol+3], 2*sizeof(record[i-1].AlarmCode));
		str2dec((ST_CHAR *)&record[i-1].AlarmCode[0], db_alarm_result.azResult[i*db_alarm_result.ncol+3], strlen(db_alarm_result.azResult[i*db_alarm_result.ncol+3]));
		str2bcd((ST_CHAR *)&record[i-1].AlarmContent[0], db_alarm_result.azResult[i*db_alarm_result.ncol+4], 2*sizeof(record[i-1].AlarmContent));
		//str2bcd(&record[i-1].AlarmContentLen, db_alarm_result.azResult[i*db_alarm_result.ncol+5], 2*sizeof(record[i-1].AlarmContentLen));
		str2dec((ST_CHAR *)&record[i-1].AlarmContentLen, db_alarm_result.azResult[i*db_alarm_result.ncol+5], strlen(db_alarm_result.azResult[i*db_alarm_result.ncol+5]));
	}
	ret = db_alarm_result.nrow;

	pthread_mutex_unlock(&mutex_alarm);

	return ret;
}
