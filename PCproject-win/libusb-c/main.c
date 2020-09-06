#include"lusb0_usb.h"
#include <stdio.h>
#include <time.h>

#define VENDOR_ID 1122
#define PRODUCT_ID 6677

#define EP1_OUT_SIZE	512	      //�ɸ����豸�޸Ĵ�С
#define EP1_IN_SIZE		512

//�ο���ַ��https://blog.csdn.net/Huihong_Vitality/article/details/65661144



int main() {
	// ����usb�豸�ṹ��
	struct usb_device * m_dev = NULL;
	// ����usb�豸�����ṹ��
	struct usb_dev_handle * m_dev_handle;


	// ������ʼ��
	usb_init();
	// Ѱ��usb����
	usb_find_busses();
	// ���������ϵ��豸
	usb_find_devices();

	// �����豸�ҵ���Ҫ���豸
	struct usb_bus *bus;
	for (bus = usb_get_busses(); bus; bus = bus->next)
	{
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next)
		{
			//Ѱ��ָ����PID,VID�豸
			if (dev->descriptor.idVendor == VENDOR_ID && dev->descriptor.idProduct == PRODUCT_ID) {
				m_dev = dev;
			}
		}
	}

	if (!m_dev)
	{
		printf("ָ���豸δ�ҵ�\n");
		return 1;
	}

	m_dev_handle = usb_open(m_dev);
	if (!m_dev_handle)
	{
		printf("�豸�޷���\n");
		return 1;
	}
	else
	{
		printf("�豸�򿪳ɹ�!\n");
	}

	//USB���������

	char str[64];

	usb_get_string_simple(m_dev_handle, m_dev->descriptor.iManufacturer, str, sizeof(str));
	printf("Device Manufacturer : %s\n", str);
	usb_get_string_simple(m_dev_handle, m_dev->descriptor.iProduct, str, sizeof(str));
	printf("Product Name : %s\n", str);




	// �����豸����
	if (usb_set_configuration(m_dev_handle, 1) < 0)
	{
		printf("�޷������豸����\n");
		usb_close(m_dev_handle);
		return 1;
	}

	//claim_interface 0 ע�������ϵͳͨ�ŵĽӿ� 0
	if (usb_claim_interface(m_dev_handle, 0) < 0)
	{
		printf("�޷�ע�����ϵͳ�ӿ�\n");
		usb_close(m_dev_handle);
		return 1;
	}


	// ��ȡ�˵�ĵ�ַ
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

	// ���������ַ���
	char RxBuffer[512];
	memset(RxBuffer, 0, sizeof(RxBuffer));

	char TxBuffer[512];
	memset(TxBuffer, 0x01, sizeof(TxBuffer));

	char c[10];
	int ret;
	while (1)
	{
		printf("����ָ�");
		gets(c);

		if (c[0] == 'w') {
			printf("���\n");
			ret = usb_bulk_write(m_dev_handle, EP_OUT, (char *)"goooo", 5, 500);
			//�˵�1��ȡ����
			if (ret != 5) {
				printf("����д��ʧ�ܣ�\n");
				break;
			}
			ret = usb_bulk_read(m_dev_handle, EP_IN, (char *)RxBuffer, 512, 500);
			printf("%s\n", RxBuffer);
			printf("%d\n", ret);

		}

		else if (c[0] == 't')

		{
			printf("�����ٶ�\n");
			int start = clock();

			//�˵�1д������
			for (size_t i = 0; i < 2048; i++)
			{
				ret = usb_bulk_write(m_dev_handle, EP_OUT, (char *)TxBuffer, 512, 0);

				//�˵�1��ȡ����
				ret = usb_bulk_read(m_dev_handle, EP_IN, (char *)RxBuffer, 512, 500);

				if (ret != 512)
				{
					printf("�˵��ȡ����ʧ��! %d\n", ret);
					return 1;
				}

			}
			int end = clock();

			printf("��ʱ%dms\n", end - start);
			float speed = 1000.0 / (end - start);
			printf("�ٶȣ�%.3fMB/s\n", speed);

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


	//ע���ӿڣ��ͷ���Դ����usb_claim_interface����ʹ�á�
	usb_release_interface(m_dev_handle, 0);
	usb_close(m_dev_handle);
	printf("\n�豸�ر�\n");


	return 0;
}