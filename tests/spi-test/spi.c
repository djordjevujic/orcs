#include <fcntl.h>				//Needed for SPI port
#include <sys/ioctl.h>			//Needed for SPI port
#include <linux/spi/spidev.h>	//Needed for SPI port
#include <unistd.h>			//Needed for SPI port
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int spi_cs0_fd;				//file descriptor for the SPI device
static int spi_cs1_fd;				//file descriptor for the SPI device
static int spi_cs_fd;

static unsigned char spi_mode;
static unsigned char spi_bitsPerWord;
static unsigned int spi_speed;

//********** SPI OPEN PORT **********
//spi_device	0=CS0, 1=CS1
int spi_open_port (int spi_device, unsigned int speed)
{
	int status_value = -1;
///   int *spi_cs_fd;

    //----- SET SPI MODE -----
    //SPI_MODE_0 (0,0) 	CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
    //SPI_MODE_1 (0,1) 	CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
    //SPI_MODE_2 (1,0) 	CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
    //SPI_MODE_3 (1,1) 	CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
    spi_mode = SPI_MODE_0;
    
    //----- SET BITS PER WORD -----
    spi_bitsPerWord = 8;
    
    //----- SET SPI BUS SPEED -----
    spi_speed = speed;		//1000000 = 1MHz (1uS per bit) 


    if (spi_device)
    	spi_cs_fd = open("/dev/spidev0.1", O_RDWR | O_NONBLOCK);
    else
    	spi_cs_fd = open("/dev/spidev0.0", O_RDWR | O_NONBLOCK);


    if (spi_cs_fd < 0)
    {
        perror("Error - Could not open SPI device");
        exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_WR_MODE, &spi_mode);
    if(status_value < 0)
    {
        perror("Could not set SPIMode (WR)...ioctl fail");
        exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_RD_MODE, &spi_mode);
    if(status_value < 0)
    {
      perror("Could not set SPIMode (RD)...ioctl fail");
      exit(1);
    }
    
    status_value = ioctl(spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
      perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
      perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
      perror("Could not set SPI speed (WR)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_cs_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
      perror("Could not set SPI speed (RD)...ioctl fail");
      exit(1);
    }
    return(status_value);
}

//********** SPI CLOSE PORT **********
int spi_close_port (int spi_device)
{
	int status_value = -1;
    int *spi_cs_fd;

    if (spi_device)
    	spi_cs_fd = &spi_cs1_fd;
    else
    	spi_cs_fd = &spi_cs0_fd;


    status_value = close(*spi_cs_fd);
    if(status_value < 0)
    {
    	perror("Error - Could not close SPI device");
    	exit(1);
    }
    return(status_value);
}

//********** SPI WRITE & READ DATA **********
//data		Bytes to write.  Contents is overwritten with bytes read.
int spi_rw (int spi_device, unsigned char *tx_buff, unsigned char *rx_buff, int length)
{
	struct spi_ioc_transfer spi[length];
	int i = 0;
	int retVal = -1;

	//one spi transfer for each byte
	
	for (i = 0 ; i < length ; i++)
	{
		memset(&spi[i], 0, sizeof (spi[i]));
		spi[i].tx_buf        = (unsigned long)(tx_buff + i); // transmit from "data"
		spi[i].rx_buf        = (unsigned long)(rx_buff + i) ; // receive into "rx_buff"
		spi[i].len           = sizeof(*(tx_buff + i)) ;
		spi[i].delay_usecs   = 0 ;
		spi[i].speed_hz      = spi_speed ;
		spi[i].bits_per_word = spi_bitsPerWord ;
		spi[i].cs_change = 0;
	}
	
	retVal = ioctl(spi_cs_fd, SPI_IOC_MESSAGE(length), &spi) ;
	
	if(retVal >= 0)
	{
		printf("retVal: %d \n", retVal);
	}
	
	if(retVal < 0)
	{
		perror("Error - Problem transmitting spi data..ioctl");

		exit(1);
	}

	return retVal;
}
