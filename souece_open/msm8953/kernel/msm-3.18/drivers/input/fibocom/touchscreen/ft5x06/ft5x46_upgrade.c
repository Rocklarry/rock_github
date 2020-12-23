static unsigned char FT5X46_CTPM_FW_BOOT[] = {
    #include "pramboot.i"
};

static u8 ft5x46_ctpm_get_i_file_ver(struct ft5x06_ts_data *ts)
{
    u16 ui_sz;

    ui_sz = ts->fw_ifile_data_len;
    if (ui_sz > 2) {
        return ts->fw_ifile_data[ui_sz - 2];
    }

    return 0x00;    /*default value */
}

static int ft5x46_get_upgrade_info(struct i2c_client *client, struct fw_upgrade_info *upgrade_info)
{
    struct ft5x06_ts_data *data = i2c_get_clientdata(client);

    switch (data->chip_id) {

    case FT5X46_ID:
        upgrade_info->delay_55 = FT5446_UPGRADE_55_DELAY;
        upgrade_info->delay_aa = FT5446_UPGRADE_AA_DELAY;
        upgrade_info->upgrade_id_1 = FT5446_UPGRADE_ID_1;
        upgrade_info->upgrade_id_2 = FT5446_UPGRADE_ID_2;
        upgrade_info->delay_readid = FT5446_UPGRADE_READID_DELAY;
        upgrade_info->delay_erase_flash = FT5446_UPGRADE_EARSE_DELAY;
        printk("[FST] Device type IC_FT5446.\n");
        break;
    default:
        upgrade_info->delay_55 = 0;
        upgrade_info->delay_aa = 0;
        upgrade_info->upgrade_id_1 =0;
        upgrade_info->upgrade_id_2 = 0;
        upgrade_info->delay_readid = 0;
        upgrade_info->delay_erase_flash = 0;
        printk("[FST] Do not matched any device type !\n");
        return -EINVAL;
    }
    return 0;
}

static int  ft5x46_ctpm_fw_preupgrade(struct i2c_client * client, u8* pbt_buf, u32 dw_lenth)
{
    u8 reg_val[4] = {0};
    u32 i = 0;
    u32 packet_number;
    u32 j;
    u32 temp;
    u32 lenght;
    u8 packet_buf[FTS_PACKET_LENGTH + 6];
    u8 auc_i2c_write_buf[10];
    u8 bt_ecc;
    u8 i_ret;
    struct fw_upgrade_info upgradeinfo;

    ft5x46_get_upgrade_info(client, &upgradeinfo);

    for (i = 0; i < FTS_UPGRADE_LOOP; i++)
    {
        /*********Step 1:Reset  CTPM *****/
        /*write 0xaa to register 0xfc */

        ft5x0x_write_reg(client, 0xfc, FT_UPGRADE_AA);
        msleep(upgradeinfo.delay_aa);

        ft5x0x_write_reg(client, 0xfc, FT_UPGRADE_55);
        msleep(upgradeinfo.delay_55);

        auc_i2c_write_buf[0] = 0xfc;
        auc_i2c_write_buf[1] = 0x66;
        i_ret = ft5x0x_write_reg(client, auc_i2c_write_buf[0], auc_i2c_write_buf[1]);

        if(i_ret < 0)
            printk("[FTS] failed writing  0x66 to register 0xbc or oxfc! \n");

        msleep(50);

        /*********Step 2:Enter upgrade mode *****/
        auc_i2c_write_buf[0] = FT_UPGRADE_55;
        i_ret = ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
        if(i_ret < 0)
        {
            printk("[FTS] failed writing  0x55 ! \n");
            continue;
        }

        auc_i2c_write_buf[0] = FT_UPGRADE_AA;
        i_ret = ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
        if(i_ret < 0)
        {
            printk("[FTS] failed writing  0xaa ! \n");
            continue;
        }
        //msleep(upgradeinfo.delay_readid);

        /*********Step 3:check READ-ID***********************/
        msleep(1);
        auc_i2c_write_buf[0] = 0x90;
        auc_i2c_write_buf[1] = auc_i2c_write_buf[2] = auc_i2c_write_buf[3] =
            0x00;
        reg_val[0] = reg_val[1] = 0x00;

        ft5x06_i2c_read(client, auc_i2c_write_buf, 4, reg_val, 2);


        if (reg_val[0] == 0x54
            && reg_val[1] == 0x22)
        {
            i_ret = 0x00;
            ft5x0x_read_reg(client, 0xd0, &i_ret);

            if(i_ret == 0)
            {
                printk("[FTS] Step 3: READ State fail \n");
                continue;
            }
            printk("[FTS] Step 3: i_ret = %d \n", i_ret);
            printk("[FTS] Step 3: READ CTPM ID OK,ID1 = 0x%x,ID2 = 0x%x\n",
                reg_val[0], reg_val[1]);
            break;
        }
        else
        {
            printk("[FTS] CTPM request ID1 = 0x54, ID2 = 0x22, update id is ID1 = 0x%x, ID2 = 0x%x .\n",
                     reg_val[0], reg_val[1]);
            continue;
        }
    }
    if (i >= FTS_UPGRADE_LOOP )
        return -EIO;

    /*********Step 4:write firmware(FW) to ctpm flash*********/
    bt_ecc = 0;
    printk("Step 5:write firmware(FW) to ctpm flash\n");

    dw_lenth = dw_lenth - 8;
    temp = 0;
    packet_number = (dw_lenth) / FTS_PACKET_LENGTH;
    packet_buf[0] = 0xae;
    packet_buf[1] = 0x00;

    for (j = 0; j < packet_number; j++) {
        temp = j * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8) (temp >> 8);
        packet_buf[3] = (u8) temp;
        lenght = FTS_PACKET_LENGTH;
        packet_buf[4] = (u8) (lenght >> 8);
        packet_buf[5] = (u8) lenght;

        for (i = 0; i < FTS_PACKET_LENGTH; i++) {
            packet_buf[6 + i] = pbt_buf[j * FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6 + i];
        }
        ft5x06_i2c_write(client, packet_buf, FTS_PACKET_LENGTH + 6);
    }

    if ((dw_lenth) % FTS_PACKET_LENGTH > 0) {
        temp = packet_number * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8) (temp >> 8);
        packet_buf[3] = (u8) temp;
        temp = (dw_lenth) % FTS_PACKET_LENGTH;
        packet_buf[4] = (u8) (temp >> 8);
        packet_buf[5] = (u8) temp;

        for (i = 0; i < temp; i++) {
            packet_buf[6 + i] = pbt_buf[packet_number * FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6 + i];
        }
        ft5x06_i2c_write(client, packet_buf, temp + 6);
    }

    temp = FT_APP_INFO_ADDR;
    packet_buf[2] = (u8) (temp >> 8);
    packet_buf[3] = (u8) temp;
    temp = 8;
    packet_buf[4] = (u8) (temp >> 8);
    packet_buf[5] = (u8) temp;
    for (i = 0; i < 8; i++)
    {
        packet_buf[6+i] = pbt_buf[dw_lenth + i];
        bt_ecc ^= packet_buf[6+i];
    }
    ft5x06_i2c_write(client, packet_buf, 6+8);

    /*********Step 5: read out checksum***********************/
    /*send the opration head */
    printk("Step 6: read out checksum\n");
    auc_i2c_write_buf[0] = 0xcc;
    //msleep(2);
    ft5x06_i2c_read(client, auc_i2c_write_buf, 1, reg_val, 1);
    if (reg_val[0] != bt_ecc) {
        printk("[FTS]--ecc error! FW=%02x bt_ecc=%02x\n", reg_val[0], bt_ecc);
        return -EIO;
    }
    printk(KERN_WARNING "checksum %X %X \n",reg_val[0],bt_ecc);
    printk("Read flash and compare\n");

    msleep(50);

    /*********Step 6: start app***********************/
    printk("Step 7: start app\n");
    auc_i2c_write_buf[0] = 0x08;
    ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
    msleep(20);

    return 0;
}


static int  ft5x46_ctpm_fw_upgrade(struct i2c_client * client, u8* pbt_buf, u32 dw_lenth)
{

    u8 reg_val[4] = {0};
    u32 i = 0;
    u32 packet_number;
    u32 j;
    u32 temp;
    u32 lenght;
    u8 packet_buf[FTS_PACKET_LENGTH + 6];
    u8 auc_i2c_write_buf[10];
    u8 bt_ecc;
    int i_ret;
    u8 *pbt_buf_boot = NULL;
    struct fw_upgrade_info upgradeinfo;

    pbt_buf_boot = FT5X46_CTPM_FW_BOOT;

    //call the upgrade function
    i_ret = ft5x46_ctpm_fw_preupgrade(client, pbt_buf_boot, sizeof(FT5X46_CTPM_FW_BOOT));

    if (i_ret != 0)
    {
        printk("[FTS] %s:upgrade failed. err.\n", __func__);
        return -EIO;
    }

    ft5x46_get_upgrade_info(client, &upgradeinfo);

    for (i = 0; i < FTS_UPGRADE_LOOP; i++) {
        /*********Step 1:Enter upgrade mode *****/
        msleep(10);
        auc_i2c_write_buf[0] = FT_UPGRADE_55;
        auc_i2c_write_buf[1] = FT_UPGRADE_AA;
        i_ret = ft5x06_i2c_write(client, auc_i2c_write_buf, 2);
        if(i_ret < 0)
        {
            printk("[FTS] failed writing  0x55 and 0xaa ! \n");
            continue;
        }

        /*********Step 2:check READ-ID***********************/
        msleep(1);
        auc_i2c_write_buf[0] = 0x90;
        auc_i2c_write_buf[1] = auc_i2c_write_buf[2] = auc_i2c_write_buf[3] = 0x00;
        reg_val[0] = reg_val[1] = 0x00;

        ft5x06_i2c_read(client, auc_i2c_write_buf, 4, reg_val, 2);

        if (reg_val[0] == upgradeinfo.upgrade_id_1
            && reg_val[1] == upgradeinfo.upgrade_id_2) {
            printk("[FTS] Step 3: READ OK CTPM ID,ID1 = 0x%x,ID2 = 0x%x\n",
            reg_val[0], reg_val[1]);
            break;
        } else {
            printk("[FTS] CTPM request ID1 = 0x%x, ID2 = 0x%x, update id is ID1 = 0x%x, ID2 = 0x%x .\n",
                    upgradeinfo.upgrade_id_1, upgradeinfo.upgrade_id_2, reg_val[0], reg_val[1]);
            continue;
        }
    }
    if (i >= FTS_UPGRADE_LOOP )
        return -EIO;

    /*Step 3:erase app and panel paramenter area*/
    printk("Step 3:erase app and panel paramenter area\n");
    auc_i2c_write_buf[0] = 0x61;
    ft5x06_i2c_write(client, auc_i2c_write_buf, 1); //erase app area
    msleep(upgradeinfo.delay_erase_flash);

    /*********Step 4:write firmware(FW) to ctpm flash*********/
    bt_ecc = 0;
    printk("Step 4:write firmware(FW) to ctpm flash\n");

    dw_lenth = dw_lenth - 8;
    temp = 0;
    packet_number = (dw_lenth) / FTS_PACKET_LENGTH;
    packet_buf[0] = 0xbf;
    packet_buf[1] = 0x00;

    for (j = 0; j < packet_number; j++) {
        temp = j * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8) (temp >> 8);
        packet_buf[3] = (u8) temp;
        lenght = FTS_PACKET_LENGTH;
        packet_buf[4] = (u8) (lenght >> 8);
        packet_buf[5] = (u8) lenght;

        for (i = 0; i < FTS_PACKET_LENGTH; i++) {
            packet_buf[6 + i] = pbt_buf[j * FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6 + i];
        }
        ft5x06_i2c_write(client, packet_buf, FTS_PACKET_LENGTH + 6);
        msleep(10);
    }

    if ((dw_lenth) % FTS_PACKET_LENGTH > 0) {
        temp = packet_number * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8) (temp >> 8);
        packet_buf[3] = (u8) temp;
        temp = (dw_lenth) % FTS_PACKET_LENGTH;
        packet_buf[4] = (u8) (temp >> 8);
        packet_buf[5] = (u8) temp;

        for (i = 0; i < temp; i++) {
            packet_buf[6 + i] = pbt_buf[packet_number * FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6 + i];
        }
        ft5x06_i2c_write(client, packet_buf, temp + 6);
        msleep(30);
    }

    temp = FT_APP_INFO_ADDR;
    packet_buf[2] = (u8) (temp >> 8);
    packet_buf[3] = (u8) temp;
    temp = 8;
    packet_buf[4] = (u8) (temp >> 8);
    packet_buf[5] = (u8) temp;
    for (i = 0; i < 8; i++)
    {
        packet_buf[6+i] = pbt_buf[dw_lenth + i];

        bt_ecc ^= packet_buf[6+i];
    }
    ft5x06_i2c_write(client, packet_buf, 6+8);
    msleep(50);

    /*********Step 5: read out checksum***********************/
    /*send the opration head */
    printk("Step 5: read out checksum\n");
    auc_i2c_write_buf[0] = 0x64;
    ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
    msleep(300);

    temp = 0;
    auc_i2c_write_buf[0] = 0x65;
    auc_i2c_write_buf[1] = (u8)(temp >> 16);
    auc_i2c_write_buf[2] = (u8)(temp >> 8);
    auc_i2c_write_buf[3] = (u8)(temp);
    temp = dw_lenth;
    auc_i2c_write_buf[4] = (u8)(temp >> 8);
    auc_i2c_write_buf[5] = (u8)(temp);
    i_ret = ft5x06_i2c_write(client, auc_i2c_write_buf, 6);
    msleep(300);

    temp = FT_APP_INFO_ADDR;
    auc_i2c_write_buf[0] = 0x65;
    auc_i2c_write_buf[1] = (u8)(temp >> 16);
    auc_i2c_write_buf[2] = (u8)(temp >> 8);
    auc_i2c_write_buf[3] = (u8)(temp);
    temp = 8;
    auc_i2c_write_buf[4] = (u8)(temp >> 8);
    auc_i2c_write_buf[5] = (u8)(temp);
    i_ret = ft5x06_i2c_write(client, auc_i2c_write_buf, 6);
    msleep(100);

    auc_i2c_write_buf[0] = 0x66;
    ft5x06_i2c_read(client, auc_i2c_write_buf, 1, reg_val, 1);
    if (reg_val[0] != bt_ecc)
    {
        printk("[FTS]--ecc error! FW=%02x bt_ecc=%02x\n", reg_val[0], bt_ecc);
        return -EIO;
    }
    printk("checksum %X %X \n",reg_val[0],bt_ecc);

    /*********Step 6: reset the new FW***********************/
    printk("Step 6: reset the new FW\n");
    auc_i2c_write_buf[0] = 0x07;
    ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
    msleep(130);    //make sure CTP startup normally

    return 0;
}

static int ft5x46_ctpm_fw_upgrade_with_i_file(struct i2c_client *client)
{
    u8* pbt_buf = 0;
    int i_ret, buf_size;
    struct ft5x06_ts_data *ts = dev_get_drvdata(&client->dev);

    //=========FW upgrade========================*/
    pbt_buf = ts->fw_ifile_data;
    buf_size = ts->fw_ifile_data_len;

    if (pbt_buf == NULL) {
        printk("[FST] %s ERR:firmware data is NULL.\n", __func__);
        return -ERR_DATA;
    }

    /*call the upgrade function*/
    i_ret =  ft5x46_ctpm_fw_upgrade(client, pbt_buf, buf_size);
    if (i_ret != 0)
    {
        printk("[FST] Firmware upgrade failed!report error %d .\n", i_ret);
    }

    return i_ret;
}
