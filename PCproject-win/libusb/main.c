#include"lusb0_usb.h"
#include <stdio.h>
#include <time.h>

#define VENDOR_ID 1122
#define PRODUCT_ID 6677

#define EP1_OUT_SIZE	512	      //可根据设备修改大小
#define EP1_IN_SIZE		512

//参考网址：https://blog.csdn.net/Huihong_Vitality/article/details/65661144



int main() {
	// 创建usb设备结构体
	struct usb_device * m_dev = NULL;
	// 创建usb设备操作结构体
	struct usb_dev_handle * m_dev_handle;


	// 环境初始化
	usb_init();
	// 寻找usb总线
	usb_find_busses();
	// 遍历总线上的设备
	usb_find_devices();

	// 遍历设备找到需要的设备
	struct usb_bus *bus;
	for (bus = usb_get_busses(); bus; bus = bus->next)
	{
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next)
		{
			//寻找指定的PID,VID设备
			if (dev->descriptor.idVendor == VENDOR_ID && dev->descriptor.idProduct == PRODUCT_ID) {
				m_dev = dev;
			}
		}
	}

	if (!m_dev)
	{
		printf("指定设备未找到\n");
		return 1;
	}

	m_dev_handle = usb_open(m_dev);
	if (!m_dev_handle)
	{
		printf("设备无法打开\n");
		return 1;
	}
	else
	{
		printf("设备打开成功!\n");
	}

	//USB的相关配置

	char str[64];

	usb_get_string_simple(m_dev_handle, m_dev->descriptor.iManufacturer, str, sizeof(str));
	printf("Device Manufacturer : %s\n", str);
	usb_get_string_simple(m_dev_handle, m_dev->descriptor.iProduct, str, sizeof(str));
	printf("Product Name : %s\n", str);




	// 进行设备配置
	if (usb_set_configuration(m_dev_handle, 1) < 0)
	{
		printf("无法进行设备配置\n");
		usb_close(m_dev_handle);
		return 1;
	}

	//claim_interface 0 注册与操作系统通信的接口 0
	if (usb_claim_interface(m_dev_handle, 0) < 0)
	{
		printf("无法注册操作系统接口\n");
		usb_close(m_dev_handle);
		return 1;
	}


	// 获取端点的地址
	int ep = m_dev->config->interface->altsetting->endpoint->bEndpointAddress;
	int EP_IN = 0;
	int EP_OUT = 0;
	if (ep > 0x0f)
	{
		EP_IN = ep;
		EP_OUT = ep - 0x80;
	}
	else
	{
		EP_OUT = ep;
		EP_IN = ep + 0x80;
	}

	printf("EP_IN: 0x%02x , EP_OUT: 0x%02x \n", EP_IN, EP_OUT);

	// 创建操作字符串
	char RxBuffer[512];
	memset(RxBuffer, 0, sizeof(RxBuffer));

	char TxBuffer[512];
	memset(TxBuffer, 0x01, sizeof(TxBuffer));

	char c[10];
	int ret;
	while (1)
	{
		printf("输入指令：");
		gets(c);

		if (c[0] == 'w') {
			printf("输出\n");
			ret = usb_bulk_write(m_dev_handle, EP_OUT, (char *)"goooo", 5, 500);
			//端点1读取数据
			if (ret != 5) {
				printf("数据写入失败！\n");
				break;
			}
			ret = usb_bulk_read(m_dev_handle, EP_IN, (char *)RxBuffer, 512, 500);
			printf("%s\n", RxBuffer);
			printf("%d\n", ret);

		}

		else if (c[0] == 't')

		{
			printf("测试速度\n");
			int start = clock();

			//端点1写入数据
			for (size_t i = 0; i < 2048; i++)
			{
				ret = usb_bulk_write(m_dev_handle, EP_OUT, (char *)TxBuffer, 512, 0);

				//端点1读取数据
				ret = usb_bulk_read(m_dev_handle, EP_IN, (char *)RxBuffer, 512, 500);

				if (ret != 512)
				{
					printf("端点读取数据失败! %d\n", ret);
					return 1;
				}

			}
			int end = clock();

			printf("用时%dms\n", end - start);
			float speed = 1000.0 / (end - start);
			printf("速度：%.3fMB/s\n", speed);

		}
		else if (c[0] == 'e')
		{
			break;
		}
		else if (c[0] == 'r') {

			ret = usb_bulk_read(m_dev_handle, EP_IN, (char *)RxBuffer, 1, 500);
			printf("%d\n", ret);

		}
	}


	//注销接口，释放资源，和usb_claim_interface搭配使用。
	usb_release_interface(m_dev_handle, 0);
	usb_close(m_dev_handle);
	printf("\n设备关闭\n");


	return 0;
}