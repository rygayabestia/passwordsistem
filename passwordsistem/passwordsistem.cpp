#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
using namespace std;

class Profile {
private:
	string name;
	string pass;
	std::unordered_map<std::string, std::string> passdata;
public:
	Profile(string name, string pass) {
		this->name = name;
		this->pass = pass;
	}


	void profile_info() {
		cout << "Profile:" << endl;
		cout << "your name: " << name << endl;
		cout << "your pass: " << pass << endl;

	}

	string hashString(const string& pass) { //функция хеширование пароля профиля
		unsigned long hash = 5381;
		for (char c : pass) {
			hash = ((hash << 5) + hash) + c;
		}
		return to_string(hash);
	}

	void data_reg(string name, string password) { //функция регистрации нового пользователя
		ofstream out;
		out.open("TextFile1.txt", ios::app);
		if (out.is_open()) {
			out << name << ":" << hashString(password) << endl;
			for (const auto& item : passdata) {
				out << "#SITE:" << name << ":" << item.first << ":" << item.second << endl;
			}
			out.close();
		}
	}

	bool data_log(string name, string pass) { //функция входа в профиль
		string line;
		bool userFound = false;
		ifstream file("TextFile1.txt");

		if (!file.is_open()) return false;

		while (getline(file, line)) {
			if (line.rfind("#SITE:", 0) == 0) {
				if (userFound) { 
					size_t firstColon = line.find(':', 6);
					size_t secondColon = line.find(':', firstColon + 1);
					string siteOwner = line.substr(6, firstColon - 6);

					if (siteOwner == name) {
						string siteName = line.substr(firstColon + 1, secondColon - firstColon - 1);
						string encryptedPass = line.substr(secondColon + 1);
						passdata[siteName] = encryptedPass;
					}
				}
				continue;
			}
			size_t colonPos = line.find(':');
			if (colonPos == string::npos) continue;

			string nameFromFile = line.substr(0, colonPos);
			string hashFromFile = line.substr(colonPos + 1);

			if (nameFromFile == name) {
				if (hashFromFile == hashString(pass)) {
					userFound = true;
				}
				else {
					file.close();
					return false; 
				}
			}
		}

		file.close(); 
		return userFound;
	}

	string xorStrings(const string & sitepass, const string & encryption_key) { // функция шифрования паролей от сайтов через XOR. как ключ здесь пароль от входа в аккаунт
			string result = sitepass;
			if (encryption_key.empty()) return result;
			for (size_t i = 0; i < sitepass.size(); i++) {
				result[i] = sitepass[i] ^ encryption_key[i % encryption_key.size()];
			}
			return result;
		}

	void addsitedata(string& url, string& sitepass, string pass) { //добавление сайта с паролем в словарь сайтов
		passdata[url] = xorStrings(sitepass, pass);
		data_reg(this->name, this->pass);
	}

	string sites_info(string & url) { //проверка словаря
	if (passdata.find(url) != passdata.end()) {
			return passdata[url];}
	return "site not found";
	}

	void site_info(string& pass) { //функция выводы сайтов с паролями
		cout << "your sites:" << endl;
		for (const auto& item : passdata) {
			cout << "sitename: " << item.first << " | site password: " << xorStrings(item.second, pass) << endl;
			}
	}
};


	int main() {
		string username;
		string password;
		char choose;
		char choose1;
		string sitename;
		string pass_site;
		cout << "Registration or log in? [r/l]" << endl;
		cin >> choose;

		if (choose == 'r') { //механика на регистрацию
			cout << "enter your name" << endl;
			cin >> username;
			cout << "enter your password" << endl;
			cin >> password;
			Profile one(username, password);
			one.data_reg(username, password);
			one.profile_info();
			one.site_info(password);
		}


		else if (choose == 'l') { //механика на логин
			cout << "enter your name" << endl;
			cin >> username;
			cout << "enter your password" << endl;
			cin >> password;
			Profile one(username, password);
			if (one.data_log(username, password) == true) {
				cout << "all is ok" << endl;
				one.profile_info();
				one.site_info(password);
				cout << "add sites? [y/n]" << endl;
				cin >> choose1;
				if (choose1 == 'y') {
					cout << "enter new sitename and password:" << endl;
					cin >> sitename;
					cin >> pass_site;
					one.addsitedata(sitename, pass_site, password);
					one.site_info(password);
				}
				else if (choose1 == 'n') { one.site_info(password); }
			}
		}
		else {
			cout << "Login failed. Wrong username or password." << endl;
		}
		return 0;
	}