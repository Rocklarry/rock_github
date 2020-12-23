static u8 ft5x36_ctpm_get_i_file_ver(struct ft5x06_ts_data *ts)
{
    u16 ui_sz;

    ui_sz = ts->fw_ifile_data_len;
    if (ui_sz > 2) {
        return ts->fw_ifile_data[ui_sz - 2];
    }

    return 0x00;    /*default value */
}

static int ft5x36_get_upgrade_info(struct i2c_client *client, struct fw_upgrade_info *upgrade_info)
{
    struct ft5x06_ts_data *data = i2c_get_clientdata(client);

    switch (data->chip_id) {

        case FT5436_ID:
        case FT5336_ID:
            upgrade_info->delay_55 = FT5X36_UPGRADE_55_DELAY;
            upgrade_info->delay_aa = FT5X36_UPGRADE_AA_DELAY;
            upgrade_info->upgrade_id_1 = FT5X36_UPGRADE_ID_1;
            upgrade_info->upgrade_id_2 = FT5X36_UPGRADE_ID_2;
            upgrade_info->delay_readid = FT5X36_UPGRADE_READID_DELAY;
            upgrade_info->delay_erase_flash = FT5X36_UPGRADE_EARSE_DELAY;
            printk("[FST] Device type IC_FT5X36.\n");
            break;
        case FT5406_ID:
            upgrade_info->delay_55 = FT5406_UPGRADE_55_DELAY;
            upgrade_info->delay_aa = FT5406_UPGRADE_AA_DELAY;
            upgrade_info->upgrade_id_1 = FT5406_UPGRADE_ID_1;
            upgrade_info->upgrade_id_2 = FT5406_UPGRADE_ID_2;
            upgrade_info->delay_readid = FT5406_UPGRADE_READID_DELAY;
            upgrade_info->delay_erase_flash = FT5406_UPGRADE_EARSE_DELAY;
            printk("[FST] Device type IC_FT5406.\n");
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

static E_UPGRADE_ERR_TYPE  ft5x36_ctpm_fw_upgrade(struct i2c_client * client, u8* pbt_buf, u32 dw_lenth)
{
    u8 reg_val[2] = {0};
    u32 i = 0;
    u8 is_5336_new_bootloader = 0;
    u8 is_5336_fwsize_30 = 0;
    u32 packet_number;
    u32 j;
    u32 temp;
    u32 lenght;
    u8 packet_buf[FTS_PACKET_LENGTH + 6];
    u8 auc_i2c_write_buf[10];
    u8 bt_ecc;
    int i_ret;
    struct fw_upgrade_info upgradeinfo;
    struct ft5x06_ts_data *data = i2c_get_clientdata(client);

    if(ft5x36_get_upgrade_info(client, &upgradeinfo)){
        printk( "[FTS] Cannot get upgrade information!\n");
        return -EINVAL;
    }

    if(pbt_buf[dw_lenth-12] == 30)
    {
        is_5336_fwsize_30 = 1;
    }
    else
    {
        is_5336_fwsize_30 = 0;
    }

    for (i = 0; i < FTS_UPGRADE_LOOP; i++) {
        /*********Step 1:Reset  CTPM *****/
        /*write 0xaa to register 0xfc*/
        ft5x0x_write_reg(client, 0xfc, FT_UPGRADE_AA);
        mdelay(upgradeinfo.delay_aa);
        /*write 0x55 to register 0xfc*/
        ft5x0x_write_reg(client, 0xfc, FT_UPGRADE_55);
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
        }while(i_ret <= 0 && i < 5 );
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

    auc_i2c_write_buf[0] = 0xcd;
    ft5x06_i2c_read(client, auc_i2c_write_buf, 1, reg_val, 1);

    /*********0705 mshl ********************/
    /*if (reg_val[0] > 4)
        is_5336_new_bootloader = 1;*/

    if (reg_val[0] <= 4)
    {
        is_5336_new_bootloader = BL_VERSION_LZ4 ;
    }
    else if(reg_val[0] == 7)
    {
        is_5336_new_bootloader = BL_VERSION_Z7 ;
    }
    else if(reg_val[0] >= 0x0f && data->chip_id != FT5406_ID)
    {
        is_5336_new_bootloader = BL_VERSION_GZF ;
    } else {
        is_5336_new_bootloader = BL_VERSION_LZ4 ;
    }

    printk("[TSP] reg_val : %d ,is 5336 new bootloader = %d.\n", reg_val[0], is_5336_new_bootloader);

    /*********Step 4:erase app*******************************/
    printk("[TSP] Step 4: erase. \n");
    if (is_5336_fwsize_30) {
        auc_i2c_write_buf[0] = 0x61;
        ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
        msleep(upgradeinfo.delay_erase_flash);
        /*erase panel parameter area */
        auc_i2c_write_buf[0] = 0x63;
        ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
        msleep(50);
    } else {
        auc_i2c_write_buf[0] = 0x61;
        ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
        msleep(upgradeinfo.delay_erase_flash);
    }

    /*********Step 5:write firmware(FW) to ctpm flash*********/
    bt_ecc = 0;
    printk("[TSP] Step 5: start upgrade. \n");

    if(is_5336_new_bootloader == BL_VERSION_LZ4 || is_5336_new_bootloader == BL_VERSION_Z7 ) {
        dw_lenth = dw_lenth - 8;
    } else if(is_5336_new_bootloader == BL_VERSION_GZF) {
        dw_lenth = dw_lenth - 14;
    }

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
            packet_buf[6+i] = pbt_buf[packet_number*FTS_PACKET_LENGTH + i];
            bt_ecc ^= packet_buf[6+i];
        }

        ft5x06_i2c_write(client, packet_buf, temp+6);
        msleep(20);
    }

    /*send the last six byte */
    if(is_5336_new_bootloader == BL_VERSION_LZ4 || is_5336_new_bootloader == BL_VERSION_Z7) {
        for (i = 0; i < 6; i++) {
            if (is_5336_new_bootloader == BL_VERSION_Z7) {
                temp = 0x7bfa + i;
            } else if(is_5336_new_bootloader == BL_VERSION_LZ4) {
                temp = 0x6ffa + i;
            }

            packet_buf[2] = (u8) (temp >> 8);
            packet_buf[3] = (u8) temp;
            temp = 1;
            packet_buf[4] = (u8) (temp >> 8);
            packet_buf[5] = (u8) temp;
            packet_buf[6] = pbt_buf[dw_lenth + i];
            bt_ecc ^= packet_buf[6];
            ft5x06_i2c_write(client, packet_buf, 7);
            msleep(20);
        }
    } else if(is_5336_new_bootloader == BL_VERSION_GZF) {

        for (i = 0; i<12; i++)
        {
            if (is_5336_fwsize_30)
            {
                temp = 0x7ff4 + i;
            }
            else
            {
                temp = 0x7bf4 + i;
            }
            packet_buf[2] = (u8)(temp>>8);
            packet_buf[3] = (u8)temp;
            temp =1;
            packet_buf[4] = (u8)(temp>>8);
            packet_buf[5] = (u8)temp;
            packet_buf[6] = pbt_buf[ dw_lenth + i];
            bt_ecc ^= packet_buf[6];

            ft5x06_i2c_write(client, packet_buf, 7);
            msleep(10);

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
    printk("[TSP] Step 7: reset the new FW\n");
    auc_i2c_write_buf[0] = 0x07;
    ft5x06_i2c_write(client, auc_i2c_write_buf, 1);
    msleep(300);

    return ERR_OK;

}

static int ft5x36_ctpm_fw_upgrade_with_i_file(struct i2c_client *client)
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

    /*judge the fw that will be upgraded
    * if illegal, then stop upgrade and return.
    */
    if (buf_size < 8 || buf_size > 32 * 1024) {
        printk("[FST] %s:Firmware length error\n", __func__);
        return -ERR_DATA;
    }

    if ((pbt_buf[buf_size - 8] ^ pbt_buf[buf_size - 6]) == 0xFF
        && (pbt_buf[buf_size - 7] ^ pbt_buf[buf_size - 5]) == 0xFF
        && (pbt_buf[buf_size - 3] ^ pbt_buf[buf_size - 4]) == 0xFF) {

        /*call the upgrade function */
        i_ret = ft5x36_ctpm_fw_upgrade(client, pbt_buf, buf_size);
        if (i_ret != 0)
            printk("[FST] Firmware upgrade failed!report error %d .\n", i_ret);
    } else {
        printk("[FST] %s:FirmWare format error\n", __func__);
        return -ERR_DATA;
    }

    return i_ret;
}
