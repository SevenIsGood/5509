#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#include <csl.h>
#include ��spi.h��
//
#ifdef __cplusplus
extern "C" {
#endif

    ///����оƬ��ʱ��Ҫ��ȫ������ָ�ȫ��������ָ�оƬ��ʱ̫��������ѡ���ÿ����ָ��
    //

    //===оƬID
    #define W25Q80                                0xEF13
    #define W25Q16                                0xEF14
    #define W25Q32                                0xEF15
    #define W25Q64                                0xEF16

    //===����ָ�
    #define W25QXX_CMD_ENABLE_WRITE               0x06
    #define W25QXX_CMD_ENABLE_VOLATILE_WRITE      0x50
    #define W25QXX_CMD_DISABLE_WRITE              0x04
    #define W25QXX_CMD_READ_REG_SR1               0x05
    #define W25QXX_CMD_READ_REG_SR2               0x35
    #define W25QXX_CMD_WRITE_REG_SR               0x01
    #define W25QXX_CMD_READ_DATA                  0x03
    #define W25QXX_CMD_READ_FAST                  0x0B
    #define W25QXX_CMD_READ_DUAL_FAST             0x3B
    #define W25QXX_CMD_PAGE_PROGRAM               0x02
    #define W25QXX_CMD_ERASE_SECTOR               0x20
    #define W25QXX_CMD_ERASE_BLOCK_64KB           0xD8
    #define W25QXX_CMD_ERASE_BLOCK_32KB           0x52
    #define W25QXX_CMD_ERASE_CHIP                 0xC7
    #define W25QXX_CMD_ERASE_SUSPEND              0x75
    #define W25QXX_CMD_ERASE_RESUME               0x7A
    #define W25QXX_CMD_POWER_DOWN                 0xB9
    #define W25QXX_CMD_RELEASE_POWER_DOWN         0xAB
    #define W25QXX_CMD_DEVICE_ID                  0xAB
    #define W25QXX_CMD_MANUFACT_DEVICE_ID         0x90
    #define W25QXX_CMD_READ_UNIQUE_ID             0x4B
    #define W25QXX_CMD_JEDEC_ID                   0x9F
    #define W25QXX_CMD_ERASE_SECURITY_REG         0x44
    #define W25QXX_CMD_PROGRAM_SECURITY_REG       0x42
    #define W25QXX_CMD_READ_SECURITY_REG          0x48
    #define W25QXX_CMD_ENABLE_RESET               0x66
    #define W25QXX_CMD_RESET                      0x99

    //===��ַ
    #define W25QXX_SECURITY_REG1                  0x001000
    #define W25QXX_SECURITY_REG2                  0x002000
    #define W25QXX_SECURITY_REG3                  0x003000

    //===ÿҳ���ֽ���
    #define W25QXX_PAGE_BYTE_SIZE                 256
    #define W25QXX_PAGE_NUM_MASK                  0xFFFFFF00
    #define W25QXX_PAGE_BYTE_MASK                 0xFF

    //===ÿ���������ֽ���
    #define W25QXX_SECTOR_BYTE_SIZE               4096
    #define W25QXX_SECTOR_NUM_MASK                0xFFFF0000
    #define W25QXX_SECTOR_BYTE_MASK               0xFFFF

    //===32Kb��С�Ŀ��ֽ���
    #define W25QXX_BLOCK_32KB_BYTE_SIZE           4096*32
    #define W25QXX_BLOCK_32KB_NUM_MASK            0xFFFE0000
    #define W25QXX_BLOCK_32KB_BYTE_MASK           0x1FFFF

    //===64Kb��С�Ŀ��ֽ���
    #define W25QXX_BLOCK_64KB_BYTE_SIZE           4096*64
    #define W25QXX_BLOCK_64KB_NUM_MASK            0xFFFC0000
    #define W25QXX_BLOCK_64KB_BYTE_MASK           0x3FFFF

    //===64Kb��С�Ŀ��ֽ���
    #define OK_0                                  0
    #define ERROR_1                               1


    //===����ṹ��
    typedef struct _W25QXX_HandlerType            W25QXX_HandlerType;

    //===����ָ��ṹ��
    typedef  struct _W25QXX_HandlerType           *pW25QXX_HandlerType;

    //===�ṹ����
    struct _W25QXX_HandlerType
    {
        Uint8     msgInit;                                                    //---�ж��Ƿ��ʼ������ 0---δ��ʼ����1---��ʼ��
        Uint8     msgBuffer[W25QXX_SECTOR_BYTE_SIZE];                         //---���ݻ�����
        Uint32    msgEraseDelayMS;                                            //---ȫ�����������ʱʱ�䣬
        Uint16    msgEraseSectorDelayMS;                                      //---��������ʱʱ��
        Uint16    msgErase32KbBlockDelayMS;                                   //---32Kb�����ʱʱ��
        Uint16    msgErase64KbBlockDelayMS;                                   //---64Kb�����ʱʱ��
        Uint16    msgChipID;                                                  //---�豸ID��Ϣ
        void      (*msgFuncDelayms)(Uint32 delay);                            //---��ʱ����
        Uint32    msgSPI;                                                     //---ʹ�õ�SPIģʽ
    };

    //===������
    #define W25QXX_TASK_ONE                        pW25QXXDevice0
    #define W25QXX_TASK_TWO                        0
    #define W25QXX_TASK_THREE                      0

    //===�ⲿ���ýӿ�
    extern W25QXX_HandlerType                    g_W25QXXDevice0;
    extern pW25QXX_HandlerType                    pW25QXXDevice0;

    //===��������
    Uint8 W25QXX_SPI_Init(W25QXX_HandlerType *W25Qx, void(*pFuncDelayus)(Uint32 delay), void(*pFuncDelayms)(Uint32 delay), Uint32(*pFuncTimerTick)(void), Uint8 isHW);
    Uint8 W25QXX_SPI_ReadRegSR1(W25QXX_HandlerType* W25Qx);
    Uint8 W25QXX_SPI_ReadRegSR2(W25QXX_HandlerType *W25Qx);
    void W25QXX_SPI_WriteRegSR1(W25QXX_HandlerType *W25Qx, Uint8 cmd);
    void W25QXX_SPI_WriteRegSR2(W25QXX_HandlerType *W25Qx, Uint8 cmd);
    void W25QXX_SPI_EnableWrite(W25QXX_HandlerType *W25Qx);
    void W25QXX_SPI_DisableWrite(W25QXX_HandlerType *W25Qx);
    Uint16 W25QXX_SPI_ReadID(W25QXX_HandlerType *W25Qx);
    Uint8 W25QXX_SPI_WaitBusy(W25QXX_HandlerType *W25Qx, Uint32 timeOut);
    void W25QXX_SPI_ReadData(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8 *pVal, Uint16 length);
    void W25QXX_SPI_ReadFast(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8 *pVal, Uint16 length);
    void W25QXX_SPI_WritePage(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8 *pVal, Uint16 length);
    void W25QXX_SPI_EraseChip(W25QXX_HandlerType *W25Qx);
    void W25QXX_SPI_EraseResume(W25QXX_HandlerType* W25Qx);
    void W25QXX_SPI_EraseSuspend(W25QXX_HandlerType* W25Qx);
    void W25QXX_SPI_EraseSector(W25QXX_HandlerType *W25Qx, Uint32 sectorAddr);
    void W25QXX_SPI_EraseBlock32KB(W25QXX_HandlerType *W25Qx, Uint32 blockAddr);
    void W25QXX_SPI_EraseBlock64KB(W25QXX_HandlerType *W25Qx, Uint32 blockAddr);
    void W25QXX_SPI_PowerDown(W25QXX_HandlerType *W25Qx);
    void W25QXX_SPI_WakeUp(W25QXX_HandlerType *W25Qx);
    void W25QXX_SPI_ReadUniqueIDNumber(W25QXX_HandlerType *W25Qx, Uint8 *pVal);
    void W25QXX_SPI_ReadJEDECID(W25QXX_HandlerType *W25Qx, Uint8 *pVal);
    void W25QXX_SPI_EraseSecurityReg(W25QXX_HandlerType *W25Qx, Uint32 regAddr);
    void W25QXX_SPI_ProgramSecurityReg(W25QXX_HandlerType *W25Qx, Uint32 regAddr, Uint8 *pVal, Uint16 length);
    void W25QXX_SPI_ReadSecurityReg(W25QXX_HandlerType *W25Qx, Uint32 regAddr, Uint8 *pVal, Uint16 length);
    void W25QXX_SPI_EnableReset(W25QXX_HandlerType *W25Qx);
    void W25QXX_SPI_Reset(W25QXX_HandlerType *W25Qx);
    void W25QXX_SPI_WriteNoCheck(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8* pVal, Uint16 length);
    void W25QXX_SPI_WriteAndCheck(W25QXX_HandlerType *W25Qx, Uint32 addr, Uint8* pVal, Uint16 length);

    //
#ifdef __cplusplus
}
#endif
//
#endif /*__SPI_FLASH_H__ */
