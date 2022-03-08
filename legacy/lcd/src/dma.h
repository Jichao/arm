#ifndef _DMA_HEADER_
#define _DMA_HEADER_

typedef enum tag_dma_source {
    nXDREQ0 = 0,
    nXDREQ1 = 1,
    UART0 = 2,
    UART1 = 3,
    I2SSDO = 4,
    I2SSDI = 5,
    SDI = 6,
    SPI0 = 7,
    SPI1 = 8,
    Timer = 9,
    PCMOUT = 10,
    PCMIN = 11,
    MICIN = 12,
    USB_EP1 = 13,
    USB_EP2 = 14,
    USB_EP3 = 15,
    USB_EP4 = 16
} dma_source_t;

#endif