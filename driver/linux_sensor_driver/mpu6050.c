//mpu6050_common.h
#define MPU6050_MAGIC 'K'

union mpu6050_data
{
    struct {
        short x;
        short y;
        short z;
    }accel;
    struct {
        short x;
        short y;
        short z;
    }gyro;
    unsigned short temp;
};

#define GET_ACCEL _IOR(MPU6050_MAGIC, 0, union mpu6050_data)
#define GET_GYRO  _IOR(MPU6050_MAGIC, 1, union mpu6050_data) 
#define GET_TEMP  _IOR(MPU6050_MAGIC, 2, union mpu6050_data)
//mpu6050_drv.h

#define SMPLRT_DIV      0x19    //�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define CONFIG          0x1A    //��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define GYRO_CONFIG     0x1B    //�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define ACCEL_CONFIG        0x1C    //���ټ��Լ졢������Χ����ͨ�˲�������ֵ��0x18(���Լ죬2G��5Hz)
#define ACCEL_XOUT_H        0x3B
#define ACCEL_XOUT_L        0x3C
#define ACCEL_YOUT_H        0x3D
#define ACCEL_YOUT_L        0x3E
#define ACCEL_ZOUT_H        0x3F
#define ACCEL_ZOUT_L        0x40
#define TEMP_OUT_H      0x41
#define TEMP_OUT_L      0x42
#define GYRO_XOUT_H     0x43
#define GYRO_XOUT_L     0x44
#define GYRO_YOUT_H     0x45
#define GYRO_YOUT_L     0x46
#define GYRO_ZOUT_H     0x47    //������z����ٶ����ݼĴ�������λ��
#define GYRO_ZOUT_L     0x48    //������z����ٶ����ݼĴ�������λ��
#define PWR_MGMT_1      0x6B    //��Դ��������ֵ��0x00(��������)
#define WHO_AM_I        0x75    //IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)
#define SlaveAddress        0x68    //MPU6050-I2C��ַ�Ĵ���
#define W_FLG           0
#define R_FLG           1
//mpu6050.c
struct mpu6050_pri {
    struct cdev dev;
    struct i2c_client *client;
};
struct mpu6050_pri dev;

static void mpu6050_write_byte(struct i2c_client *client,const unsigned char reg,const unsigned char val)
{ 
    char txbuf[2] = {reg,val};
    struct i2c_msg msg[2] = {
        [0] = {
            .addr = client->addr,
            .flags= W_FLG,
            .len = sizeof(txbuf),
            .buf = txbuf,
        },
    };
    i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
}
static char mpu6050_read_byte(struct i2c_client *client,const unsigned char reg)
{
    char txbuf[1] = {reg};
    char rxbuf[1] = {0};
    struct i2c_msg msg[2] = {
        [0] = {
            .addr = client->addr,
            .flags = W_FLG,
            .len = sizeof(txbuf),
            .buf = txbuf,
        },
        [1] = {
            .addr = client->addr,
            .flags = I2C_M_RD,
            .len = sizeof(rxbuf),
            .buf = rxbuf,
        },
    };

    i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
    return rxbuf[0];
}
static int dev_open(struct inode *ip, struct file *fp)
{
    return 0;
}
static int dev_release(struct inode *ip, struct file *fp)
{
    return 0;
}
static long dev_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
    int res = 0;
    union mpu6050_data data = {{0}};
    switch(cmd){
    case GET_ACCEL:
        data.accel.x = mpu6050_read_byte(dev.client,ACCEL_XOUT_L);
        data.accel.x|= mpu6050_read_byte(dev.client,ACCEL_XOUT_H)<<8;
        data.accel.y = mpu6050_read_byte(dev.client,ACCEL_YOUT_L);
        data.accel.y|= mpu6050_read_byte(dev.client,ACCEL_YOUT_H)<<8;
        data.accel.z = mpu6050_read_byte(dev.client,ACCEL_ZOUT_L);
        data.accel.z|= mpu6050_read_byte(dev.client,ACCEL_ZOUT_H)<<8;
        break;
    case GET_GYRO:
        data.gyro.x = mpu6050_read_byte(dev.client,GYRO_XOUT_L);
        data.gyro.x|= mpu6050_read_byte(dev.client,GYRO_XOUT_H)<<8;
        data.gyro.y = mpu6050_read_byte(dev.client,GYRO_YOUT_L);
        data.gyro.y|= mpu6050_read_byte(dev.client,GYRO_YOUT_H)<<8;
        data.gyro.z = mpu6050_read_byte(dev.client,GYRO_ZOUT_L);
        data.gyro.z|= mpu6050_read_byte(dev.client,GYRO_ZOUT_H)<<8;
        printk("gyro:x %d, y:%d, z:%d\n",data.gyro.x,data.gyro.y,data.gyro.z);
        break;
    case GET_TEMP:
        data.temp = mpu6050_read_byte(dev.client,TEMP_OUT_L);
        data.temp|= mpu6050_read_byte(dev.client,TEMP_OUT_H)<<8;
        printk("temp: %d\n",data.temp);
        break;
    default:
        printk(KERN_INFO "invalid cmd");
        break;
    }
    printk("acc:x %d, y:%d, z:%d\n",data.accel.x,data.accel.y,data.accel.z);
    res = copy_to_user((void *)arg,&data,sizeof(data));
    return sizeof(data);
}

struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .unlocked_ioctl = dev_ioctl, 
};

#define DEV_CNT 1
#define DEV_MI 0
#define DEV_MAME "mpu6050"

struct class *cls;
dev_t dev_no ;

static void mpu6050_init(struct i2c_client *client)
{
    mpu6050_write_byte(client, PWR_MGMT_1, 0x00);
    mpu6050_write_byte(client, SMPLRT_DIV, 0x07);
    mpu6050_write_byte(client, CONFIG, 0x06);
    mpu6050_write_byte(client, GYRO_CONFIG, 0x18);
    mpu6050_write_byte(client, ACCEL_CONFIG, 0x0);
}
static int mpu6050_probe(struct i2c_client * client, const struct i2c_device_id * id)
{
    dev.client = client;
    printk(KERN_INFO "xj_match ok\n");
    cdev_init(&dev.dev,&fops);
    
    alloc_chrdev_region(&dev_no,DEV_MI,DEV_CNT,DEV_MAME);
    
    cdev_add(&dev.dev,dev_no,DEV_CNT);
    
    mpu6050_init(client);

    /*�Զ������豸�ļ�*/
    cls = class_create(THIS_MODULE,DEV_MAME);
    device_create(cls,NULL,dev_no,NULL,"%s%d",DEV_MAME,DEV_MI);
    
    printk(KERN_INFO "probe\n");
    
    return 0;
}

static int mpu6050_remove(struct i2c_client * client)
{
    device_destroy(cls,dev_no);
    class_destroy(cls);
    unregister_chrdev_region(dev_no,DEV_CNT);
    return 0;
}

struct of_device_id mpu6050_dt_match[] = {
    {.compatible = "invensense,mpu6050"},
    {},
};

struct i2c_device_id mpu6050_dev_match[] = {};
struct i2c_driver mpu6050_driver = {
    .probe = mpu6050_probe,
    .remove = mpu6050_remove,
    .driver = {
        .owner = THIS_MODULE,
        .name = "mpu6050drv",
        .of_match_table = of_match_ptr(mpu6050_dt_match), 
    },
    .id_table = mpu6050_dev_match,
};
module_i2c_driver(mpu6050_driver);
MODULE_LICENSE("GPL");



///ͨ�����������, ���ǿ�����Ӧ�ò�����豸�ļ���mpu6050�Ĵ����ж�ȡԭʼ����, Ӧ�ò�����

/////////////////////////////////////////////////////////////////////////

int main(int argc, char * const argv[])
{
    int fd = open(argv[1],O_RDWR);
    if(-1== fd){
        perror("open");
        return -1;
    }
    union mpu6050_data data = {{0}};
    while(1){
        ioctl(fd,GET_ACCEL,&data);
        printf("acc:x %d, y:%d, z:%d\n",data.accel.x,data.accel.y,data.accel.z);
        ioctl(fd,GET_GYRO,&data);
        printf("gyro:x %d, y:%d, z:%d\n",data.gyro.x,data.gyro.y,data.gyro.z);
        ioctl(fd,GET_TEMP,&data);
        printf("temp: %d\n",data.temp);
        sleep(1);
    }
    return 0;
}
