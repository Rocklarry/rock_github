static u8 ft6x36_ctpm_get_i_file_ver(struct ft5x06_ts_data *ts)
{
    u16 ui_sz;

    ui_sz = ts->fw_ifile_data_len;
    if (ui_sz > 2) {
        return ts->fw_ifile_data[0x10a];
    }

    return 0x00;    /*default value */
}

static int ft6x36_get_upgrade_info(struct i2c_client *client, struct fw_upgrade_info *upgrade_info)
{
    struct ft5x06_ts_data *data = i2c_get_clientdata(client);

    switch (data->chip_id) {

    case FT6X36_ID:
        upgrade_info->delay_55 = FT6436_UPGRADE_55_DELAY;
        upgrade_info->delay_aa = FT6436_UPGRADE_AA_DELAY;
        upgrade_info->upgrade_id_1 =FT6436_UPGRADE_ID_1;
        upgrade_info->upgrade_id_2 = FT6436_UPGRADE_ID_2;
        upgrade_info->delay_readid = FT6436_UPGRADE_READID_DELAY;
        upgrade_info->delay_erase_flash = FT6436_UPGRADE_EARSE_DELAY;
        printk("[FST] Device type IC_FT6436.\n");
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

/*
[function]:
    burn the FW to ctpm.
[parameters]:(ref. SPEC)
    pbt_buf[in]    :point to Head+FW ;
    dw_lenth[in]:the length of the FW + 6(the Head length);
    bt_ecc[in]    :the ECC of the FW
[return]:
    ERR_OK        :no error;
    ERR_MODE    :fail to switch to UPDATE mode;
    ERR_READID    :read id fail;
    ERR_ERASE    :erase chip fail;
    ERR_DATA     :data error;
    ERR_ECC        :ecc error.
*/
static E_UPGRADE_ERR_TYPE  ft6x36_ctpm_fw_upgrade(struct i2c_client *client, u8* pbt_buf, u32 dw_lenth)
{
    u8 reg_val[2] = {0};
    u32 i = 0;
    u32 packet_number;
    u32 j;
    u32 temp;
    u32 lenght;
    u8 packet_buf[FTS_PACKET_LENGTH + 6];
    u8 auc_i2c_write_buf[10];
    u8 bt_ecc;
    int i_ret;
    u32 fw_length;
    struct fw_upgrade_info upgradeinfo;

    if(pbt_buf[0] != 0x02)
    {
        printk("[FTS] FW first byte is not 0x02. so it is invalid \n");
        return -1;
    }

    if(dw_lenth > 0x11f)
    {
        fw_length = ((u32)pbt_buf[0x100]<<8) + pbt_buf[0x101];
        if(dw_lenth < fw_length)
        {
            printk("[FTS] Fw length is invalid \n");
            return -1;
        }
    }
    else
    {
        printk("[FTS] Fw length is invalid \n");
        return -1;
    }

    if(ft6x36_get_upgrade_info(client, &upgradeinfo)){
        printk( "[FTS] Cannot get upgrade information!\n");
        return -EINVAL;
    }

    for (i = 0; i < FTS_UPGRADE_LOOP; i++) {
        /*********Step 1:Reset  CTPM *****/
        /*write 0xaa to register 0xbc*/
        ft5x0x_write_reg(client, 0xbc, FT_UPGRADE_AA);
        mdelay(upgradeinfo.delay_aa);
        /*write 0x55 to register 0xbc*/
        ft5x0x_write_reg(client, 0xbc, FT_UPGRADE_55);
        mdelay(upgradeinfo.delay_55);
        printk("[TSP] Step 1: Reset CTPM test \n");

        /*********Step 2:Enter upgrade mode *****/
        auc_i2c_write_buf[0] = FT_UPGRADE_55;
        auc_i2c_write_buf[1] = FT_UPGRADE_AA;
        do
        {
            i ++;
            i_ret = ft5x06_i2c_write(client, auc_i2c_write_buf, sizeof(auc_i2c_write_buf));
            msleep(5);
        }while(i_ret <= 0 && i < 30 );
        msleep(upgradeinfo.delay_readid);

        /*********Step 3:check READ-ID***********************/
        auc_i2c_write_buf[0] = 0x90;
        auc_i2c_write_buf[1] = auc_i2c_write_buf[2] = auc_i2c_write_buf[3] = 0x00;
        ft5x06_i2c_read(client, auc_i2c_write_buf, 4, reg_val, 2);
        printk("[TSP] CTPM request ID1 = 0x%x, ID2 = 0x%x, update id is ID1 = 0x%x, ID2 = 0x%x .\n",
                upgradeinfo.upgrade_id_1, upgradeinfo.upgrade_id_2, reg_val[0], reg_val[1]);
        if (reg_val[0] == upgradeinfo.upgrade_id_1 && reg_val[1] == upgradeinfo.upgrade_id_2)
        {
            printk("[TSP] Step 3: CTPM ID,ID1 = 0x%x,ID2 = 0x%x\n", reg_val[0], reg_val[1]);
            break;
        }
    }
    if (i >= FTS_UPGRADE_LOOP) {
        printk("[TSP] CTPM upgrade loop readid err\n");
        return -ERR_READID;
    }

    /*********Step 4:erase app*******************************/
    auc_i2c_write_buf[0] = 0x61;
    ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
    msleep(upgradeinfo.delay_erase_flash);

    printk("[TSP] Step 4: erase. \n");

    for(i = 0;i < 200;i++)
    {
        auc_i2c_write_buf[0] = 0x6a;
        auc_i2c_write_buf[1] = 0x00;
        auc_i2c_write_buf[2] = 0x00;
        auc_i2c_write_buf[3] = 0x00;
        reg_val[0] = 0x00;
        reg_val[1] = 0x00;
        ft5x06_i2c_read(client, auc_i2c_write_buf, 4, reg_val, 2);
        if(0xb0 == reg_val[0] && 0x02 == reg_val[1])
        {
            printk("[FTS] erase app finished \n");
            break;
        } else if (i == 199) {
            printk("[FTS] erase app failed \n");
            return -ERR_ERASE;
        }
        msleep(1);
    }
    msleep(50);


    /*********Step 5:write firmware(FW) to ctpm flash*********/
    bt_ecc = 0;
    printk("[TSP] Step 5: start upgrade. \n");

    dw_lenth = dw_lenth - 8;

    packet_number = (dw_lenth) / FTS_PACKET_LENGTH;
    packet_buf[0] = 0xbf;
    packet_buf[1] = 0x00;
    for (j=0;j<packet_number;j++)
    {
        temp = j * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8)(temp>>8);
        packet_buf[3] = (u8)temp;
        lenght = FTS_PACKET_LENGTH;
        packet_buf[4] = (u8)(lenght>>8);
        packet_buf[5] = (u8)lenght;

        for (i=0;i<FTS_PACKET_LENGTH;i++)
        {
            packet_buf[6+i] = pbt_buf[j*FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6+i];
        }

        ft5x06_i2c_write(client, packet_buf, FTS_PACKET_LENGTH + 6);
        msleep(FTS_PACKET_LENGTH/6 + 1);

        for(i = 0;i < 30;i++)
        {
            auc_i2c_write_buf[0] = 0x6a;
            auc_i2c_write_buf[1] = 0x00;
            auc_i2c_write_buf[2] = 0x00;
            auc_i2c_write_buf[3] = 0x00;
            reg_val[0] = 0x00;
            reg_val[1] = 0x00;
            ft5x06_i2c_read(client, auc_i2c_write_buf, 4, reg_val, 2);
            if(0xb0 == (reg_val[0] & 0xf0) && (0x03 + (j % 0x0ffd)) == (((reg_val[0] & 0x0f) << 8) |reg_val[1]))
            {
                printk("[FTS] write a block data finished \n");
                break;
            } else if (i == 29) {
                printk("[FTS] write a block data failed \n");
                return -ERR_DATA;
            }
            msleep(1);
        }

    }

    if ((dw_lenth) % FTS_PACKET_LENGTH > 0)
    {
        temp = packet_number * FTS_PACKET_LENGTH;
        packet_buf[2] = (u8)(temp>>8);
        packet_buf[3] = (u8)temp;

        temp = (dw_lenth) % FTS_PACKET_LENGTH;
        packet_buf[4] = (u8)(temp>>8);
        packet_buf[5] = (u8)temp;

        for (i=0;i<temp;i++)
        {
            packet_buf[6+i] = pbt_buf[ packet_number*FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6+i];
        }

        ft5x06_i2c_write(client, packet_buf, temp+6);
        msleep(20);

        for(i = 0;i < 30;i++)
        {
            auc_i2c_write_buf[0] = 0x6a;
            auc_i2c_write_buf[1] = 0x00;
            auc_i2c_write_buf[2] = 0x00;
            auc_i2c_write_buf[3] = 0x00;
            reg_val[0] = 0x00;
            reg_val[1] = 0x00;
            ft5x06_i2c_read(client, auc_i2c_write_buf, 4, reg_val, 2);
            if(0xb0 == (reg_val[0] & 0xf0) && (0x03 + (j % 0x0ffd)) == (((reg_val[0] & 0x0f) << 8) |reg_val[1]))
            {
                printk("[FTS] write a block data finished \n");
                break;
            } else if (i == 29) {
                printk("[FTS] write a block data failed \n");
                return -ERR_DATA;
            }
            msleep(1);
        }
    }

    /*********Step 6: read out checksum***********************/
    /*send the opration head*/
    auc_i2c_write_buf[0] = 0xcc;
    ft5x06_i2c_read(client, auc_i2c_write_buf, 1, reg_val, 1);
    printk("[TSP] Step 6:  ecc read 0x%x, new firmware 0x%x. \n", reg_val[0], bt_ecc);
    if(reg_val[0] != bt_ecc)
    {
        printk("[TSP] CTPM upgrade ecc error!\n");
        return -ERR_ECC;
    }

    /*********Step 7: reset the new FW***********************/
    auc_i2c_write_buf[0] = 0x07;
    ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
    msleep(300);

    return ERR_OK;
}


static int ft6x36_ctpm_fw_upgrade_with_i_file(struct i2c_client *client)
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
    i_ret =  ft6x36_ctpm_fw_upgrade(client, pbt_buf, buf_size);
    if (i_ret != 0)
    {
         printk("[FST] Firmware upgrade failed!report error %d .\n", i_ret);
    }

    return i_ret;
}
