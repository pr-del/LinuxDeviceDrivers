#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
 
#define I2C_BUS_AVAILABLE   (   2      )              // I2C Bus available in BBB
#define SLAVE_DEVICE_NAME   ("AT24C256")              // Device and Driver Name
#define eeprom_SLAVE_ADDR   (     0x50 )              // eeprom Slave Address
 
static struct i2c_adapter *desd_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *desd_i2c_client_eeprom = NULL;  // I2C Cient Structure (In our case it is EEPROM)
 
/*
** This function writes the data into the I2C client
**
**  Arguments:
**      buff -> buffer to be sent
**      len  -> Length of the data
**   
*/
static int I2C_Write(unsigned char *buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_send(desd_i2c_client_eeprom, buf, len);
    
    return ret;
}
 
/*
** This function reads one byte of the data from the I2C client
**  Arguments:
**      out_buff -> buffer wherer the data to be copied
**      len      -> Length of the data to be read 
*/
static int I2C_Read(unsigned char *out_buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_recv(desd_i2c_client_eeprom, out_buf, len);
    
    return ret;
}
 
/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int desd_eeprom_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	char buf[32];
    pr_info("EEPROM Probed!!!\n"); 
	
	return 0;
}
 
/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int desd_eeprom_remove(struct i2c_client *client)
{   
    pr_info("EEPROM Removed!!!\n");
    return 0;
}
 
/*
** Structure that has slave device id
*/
static const struct i2c_device_id desd_eeprom_id[] = {
        { SLAVE_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, desd_eeprom_id);
 
/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver desd_eeprom_driver = {
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = desd_eeprom_probe,
        .remove         = desd_eeprom_remove,
        .id_table       = desd_eeprom_id,
};
 
/*
** I2C Board Info strucutre
*/
static struct i2c_board_info eeprom_i2c_board_info = {
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME, eeprom_SLAVE_ADDR)
    };
 
/*
** Module Init function
*/
static int __init desd_driver_init(void)
{
    int ret = -1;
    desd_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
    
    if( desd_i2c_adapter != NULL )
    {
        //desd_i2c_client_eeprom = i2c_new_device(desd_i2c_adapter, &eeprom_i2c_board_info);
        desd_i2c_client_eeprom = i2c_new_client_device(desd_i2c_adapter, &eeprom_i2c_board_info);
        
        if( desd_i2c_client_eeprom != NULL )
        {
            i2c_add_driver(&desd_eeprom_driver);
    		pr_info("Driver Added!!!\n");
            ret = 0;
        }
		else
    		pr_info("EEPROM client not found!!!\n");

        
        i2c_put_adapter(desd_i2c_adapter);
    }
	else
    	pr_info("I2C Bus Adapter Not Available!!!\n");
    
    return ret;
}
 
/*
** Module Exit function
*/
static void __exit desd_driver_exit(void)
{
	if(desd_i2c_client_eeprom != NULL) {
	    i2c_unregister_device(desd_i2c_client_eeprom);
		i2c_del_driver(&desd_eeprom_driver);
	}
    pr_info("Driver Removed!!!\n");
}
 
module_init(desd_driver_init);
module_exit(desd_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Blizzard");
MODULE_DESCRIPTION("Simple I2C driver(EEPROM)");

