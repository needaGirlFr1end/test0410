//����! �̶�� �ϴ� ������!
//Struct�� ����ϰ� ���� �� �ֽ��ϴ�!
//�������� �����̳�? �ϳ��� �޸𸮸� �������� �ڷ����� �����ؿ�!

//[0][0][0][10]
//          10  int
//          \n  char[3]
union ConvertionBase
{
	unsigned int uInteger;
	int integer;
	float floating;
	char character[4];
	short shortInteger[2];
	unsigned short uShortInteger[2];
};
ConvertionBase byteConvertor;

//�׻� Ŭ���̾�Ʈ�� enum���� ��ġ�����ּ���!
enum class MessageType
{
	EndOfLine, //���ڿ��� ���� �Ǵ��� �� \0�̶�� �ϴ� �� �ľ��߽��ϴ�! �޽����� �ڿ� �� ���� ��� �ϴ� �� EndOfLine���� ǥ��!
	LogIn,
	LogOut,
	Chat,

	Unknown,  //���� ���� �𸣰ڴ� ����!
	Length
	//���� ���� �޽��� Ÿ���� �������� �� ���� ������ ������ ����!
};

class MessageInfo
{
public :
	MessageType type;
	int length;
};

class MessageInfo_Login : public MessageInfo
{
public :
	int userIndex;
	string name;
	MessageInfo_Login(char* message, int targetUser)
	{
		type = MessageType::LogIn;

		userIndex = targetUser;
		//�޽����� 4��°���� ������ �־��ֵ��� �սô�!
		name = &(message[4]);
	}
};

class MessageInfo_Chat : public MessageInfo
{
public:
	int userIndex;
	string value;

	MessageInfo_Chat(char* message, int targetUser)
	{
		type = MessageType::Chat;

		userIndex = targetUser;
		//�޽����� 4��°���� ������ �־��ֵ��� �սô�!
		value = &(message[4]);
	}
};