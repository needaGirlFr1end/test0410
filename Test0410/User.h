class User {

	int fdNumber;

	wstring name = "<NULL>";

public:
	User(int targetFD) {
		fdNumber = targetFD;
	}

	void SetName(wstring wantName) { name = wantName };

};