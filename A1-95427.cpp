#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <string>
using namespace std;

typedef struct pixel Pixel;
typedef struct image Image;
typedef struct unit Unit;
/*in this part i declare pixel and image and unit structures.*/
struct pixel {
	int red;
	int green;
	int blue;
};
struct image {
	int width;
	int heigh;
	int offset;
	vector<Pixel> pixels;
	int correct_process;
};
struct unit {
	vector<int> pixels;
	string color;
	double var;
};
/*this function calculate average of colors.*/
double avrg(Unit table, Image image, string color) {
	double sum = 0;
	double result = 0;
	int net_color = 0;
	if (color.compare("red") == 0) {
		for (int i = 0; i < 64; i++) {
			net_color = (image.pixels[table.pixels[i]]).red;
			if (net_color % 2 == 1) {
				net_color--;
			}
			sum += net_color;
		}
		result = sum / 64;
	}
	if (color.compare("green") == 0) {
		for (int i = 0; i < 64; i++) {
			net_color = (image.pixels[table.pixels[i]]).green;
			if (net_color % 2 == 1) {
				net_color--;
			}
			sum += net_color;
		}
		result = sum / 64;
	}
	if (color.compare("blue") == 0) {
		for (int i = 0; i < 64; i++) {
			net_color = (image.pixels[table.pixels[i]]).blue;
			if (net_color % 2 == 1) {
				net_color--;
			}
			sum += net_color;
		}
		result = sum / 64;
	}
	return result;
}
/*this function calculate variance of colors.*/
double variance_table(Unit table, string color, Image image) {
	double avr = 0;
	double result = 0;
	int net_color = 0;
	avr = avrg(table, image, color);
	if (color.compare("red") == 0) {
		for (int i = 0; i < 64; i++) {
			net_color = (image.pixels[table.pixels[i]]).red;
			if (net_color % 2 == 1) {
				net_color--;
			}
			result += (net_color - avr) * (net_color - avr);
		}
	}
	if (color.compare("green") == 0) {
		for (int i = 0; i < 64; i++) {
			net_color = (image.pixels[table.pixels[i]]).green;
			if (net_color % 2 == 1) {
				net_color--;
			}
			result += (net_color - avr) * (net_color - avr);
		}
	}
	if (color.compare("blue") == 0) {
		for (int i = 0; i < 64; i++) {
			net_color = (image.pixels[table.pixels[i]]).blue;
			if (net_color % 2 == 1) {
				net_color--;
			}
			result += (net_color - avr) * (net_color - avr);
		}
	}
	return result / 64;
}
/*this function use selection_sort to sort 8*8 chunks. */
void sort_tables(vector<Unit>& tables) {
	for (int i = tables.size() - 1; i >= 0; i--) {
		int min_index = i;
		for (int j = i; j >= 0; j--) {
			if (tables[j].var < tables[min_index].var) {
				min_index = j;
			}
		}
		swap(tables[i], tables[min_index]);
	}

}
/*this function converts message to bits then save these bits in LSB bits of random pixels.*/
void encrypt(int key, string message, vector<Unit>& tables, vector<char>& img,
		int offset,int heigh) {
	/*in this part i convert string to bits.*/
	vector<int> message_bits;
	message = message + "\n";
	for (int i = 0; i < message.size(); i++) {
		vector<int> bits;
		int temp = (int) message[i];
		while (temp > 0) {
			bits.push_back(temp % 2);
			temp /= 2;
		}
		int start = bits.size();
		for (int k = start; k < 8; k++) {
			message_bits.push_back(0);
		}
		for (int j = bits.size(); j > 0; j--) {
			message_bits.push_back(bits[j - 1]);
		}

	}
	/*in this part i find random bytes then change LSB of them. */
	srand(key);
	int rand_pixel;
	for (int i = 0; i < message_bits.size(); i++) {

		rand_pixel = rand() % 64;
		string color = tables[i].color;
		int rand_byte_index;
		int base;
		base = (tables[i].pixels[rand_pixel]);
		int div=heigh;
		if((3*heigh)%4!=0){
			rand_byte_index = ((3*(div)+(4-((3*div)%4)))*(base/div))+((base%div)*3)+offset+1;
		}
		if ((3*heigh)%4==0)
		{
			rand_byte_index= ((3*div)*(base/div))+((base%div)*3)+offset+1;
		}

		if (color == "red") {
			rand_byte_index += 2;
		}
		if (color == "green") {
			rand_byte_index += 1;
		}
		if (color == "blue") {
			rand_byte_index += 0;
		}

		char temp = img[rand_byte_index];
		temp = img[rand_byte_index] & 254;
		if (message_bits[i] == 0) {
			img[rand_byte_index] = (temp | 0);
		}
		if (message_bits[i] == 1) {
			img[rand_byte_index] = (temp | 1);
		}

	}

}
/*this function use "output.bmp" data and read image's message and write that message.*/
void decrypt(int key, vector<Unit> tables, vector<char> img, int offset,int heigh) {

	vector<int> message_bits;
	int start_bit = 0;
	int ascii = 0;
	srand(key);
	int rand_pixel;
	for (int i = 0; i < tables.size(); i++) {
		/*in this part find random pixels and random bytes then find message bits.*/
		rand_pixel = rand() % 64;
		string color = tables[i].color;
		int rand_byte_index;
		int base;
		base = (tables[i].pixels[rand_pixel]);
		int div=heigh;
		if((3*heigh)%4!=0){
			rand_byte_index = ((3*(div)+(4-((3*div)%4)))*(base/div))+((base%div)*3)+offset+1;
		}
		if ((3*heigh)%4==0)
		{
			rand_byte_index= ((3*div)*(base/div))+((base%div)*3)+offset+1;
		}
		if (color == "red") {
			rand_byte_index += 2;
		}
		if (color == "green") {
			rand_byte_index += 1;
		}
		if (color == "blue") {
			rand_byte_index += 0;
		}
		/*in this part i convert bits to char.*/
		if (message_bits.size() % 8 == 0 && message_bits.size() != 0) {
			int exp = 128;
			ascii = 0;
			for (int i = start_bit; i < 8 + start_bit; i++) {
				ascii += message_bits[i] * (exp);
				exp /= 2;
			}
			message_bits.erase(message_bits.begin(), message_bits.end());
			cout << (char) ascii;
			if (ascii == 10) {
				break;
			}
		}
		char temp = img[rand_byte_index];
		message_bits.push_back(img[rand_byte_index] & 1);

	}

}
/*this function open image and save its information about size and pixels in Image structure.*/
Image process(string image_name, vector<char>& img) {
	ifstream file(image_name);
	int c = 1;
	int base = 1;
	int offset = 0;
	int ic = 0;
	int start = 0;
	int width = 0;
	int heigh = 0;
	Image image;
	int correct_process = 1;
	/*check file existance and read file bytes.*/
	if (file) {
		/*get length of file.*/
		file.seekg(0, std::ios::end);
		long length = file.tellg();
		file.seekg(0, std::ios::beg);
		/*read data as a block .*/
		img.resize(length);
		file.read(&(img[0]), length);
		file.close();
	} else {
		cout << "I can not open this image." << endl << "good bye!" << endl;
		correct_process = 0;
		image.correct_process = correct_process;
		return image;
	}
	/*set width and heigh and offset.*/
	for (int i = 18; i < 22; ++i) {
		c = (int) img[i];
		if (c < 0) {
			c += 256;
		}
		width += c * pow(256,  (double)(i - 18));

	}
	for (int i = 22; i < 26; ++i) {
		c = (int) img[i];
		if (c < 0) {
			c += 256;
		}
		heigh += c * pow(256, (double) (i - 22));

	}
	for (int i = 10; i < 14; ++i) {
		c = (int) img[i];
		if (c < 0) {
			c += 256;
		}
		offset += c * pow(256, (double)(i - 10));

	}
	image.width = width;
	image.heigh = heigh;
	image.offset = offset;
	/*save image pixels in a structure.*/
	for (int i = 0; i <= (width * heigh * 3) - 3; i += 3) {
		
		Pixel p = { 0, 0, 0 };
		for (int j = 0; j < 3; j++) {
			c = (int) img[i + offset + j];
			if (c < 0) {
				c += 256;
			}
			if (j == 0)
				p.blue = c;
			if (j == 1)
				p.green = c;
			if (j == 2)
				p.red = c;

		}
		image.pixels.push_back(p);
		if (((image.pixels.size())%heigh)==0 && image.pixels.size()!=0 )
		{
			
			if((3*heigh)%4!=0){
				i+=(4-((3*heigh)%4));
			}
			
		}

	}
	image.correct_process = correct_process;
	return image;
}
/*this function splite image to 8*8 chunks.function save chunks data in Unit structures.*/
vector<Unit> create_chunk(int width, int heigh, Image image) {
	int start_x = 0;
	int start_y = 0;
	double var_table = 0;
	bool next_block_y = false;
	bool change_start_x = true;
	vector<Unit> tables;
	while (start_y < (width - (width % 8))) {

		if (start_x == (heigh - (heigh % 8))) {
			start_x = 0;
			next_block_y = true;
			change_start_x = false;
		}
		if((heigh - (heigh % 8))==0){
			break;
		}
		if (next_block_y) {
			start_y += 8;
			next_block_y = false;
			change_start_x = true;
		}
		if (start_y == (width - (width % 8)))
			break;

		for (int k = 0; k < 3; k++) {
			Unit temp;
			for (int i = start_y; i < 8 + start_y; i++) {
				for (int j = start_x; j < 8 + start_x; j++) {
					temp.pixels.push_back(i * (heigh) + j);
				}
			}
			if (k == 0) {
				temp.color = "red";
			}
			if (k == 1) {
				temp.color = "green";
			}
			if (k == 2) {
				temp.color = "blue";
			}
			var_table = variance_table(temp, temp.color, image);
			temp.var = var_table;
			tables.push_back(temp);
		}

		if (change_start_x)
			start_x += 8;
		else
			change_start_x = true;

	}
	
	return tables;
}
/*this function check user key.if user key digits is between 4 and 6 function return "true".*/
bool valid_key(int key) {
	int key_temp = key;
	int counter = 0;
	bool valid = true;
	while (key_temp > 0) {
		counter++;
		key_temp /= 10;
	}
	if (counter < 4 || counter > 6) {
		cout << "your key has " << counter
				<< " digits. you need to choose a number that its digits is between 4 and 6 ."
				<< endl;
		valid = false;
		return valid;
	}
	return valid;
}
/*this function check user message.if user message's bits is more than image capacity function return "false".*/
bool valid_message(string message, int size) {
	bool valid = true;
	if ((message.size() + 1) * 8 > size) {
		cout << "your message is too long!" << endl;
		valid = false;
		return valid;
	}
	return valid;
}
int main() {
	string state;
	string image_name;
	int width, heigh, offset;
	vector<char> img;
	/*set state,kye and message and image_name in main function.*/
	cout << "Which command do you want to do ? (encrypt , decrypt)" << endl;
	cin >> state;
	if (state == "encrypt") {
		cout << "Write your image_name :";
		cin >> image_name;
		cout << "Please waite..." << endl;
		Image image = process(image_name, img);
		if (!image.correct_process) {
			return 0;
		}
		width = image.width;
		heigh = image.heigh;
		offset = image.offset;
		vector<Unit> tables = create_chunk(width, heigh, image);
		sort_tables(tables);
		int key = 0;
		string message;
		cout << "Write your key number :";
		cin >> key;
		if (!valid_key(key)) {
			cout << "good bye" << endl;
			return 0;
		}
		/*i use cin.ignore() to get a line with spaces save it in message. */
		cout << "Write your message :";
		cin.ignore();
		getline(cin, message);
		if (!valid_message(message, tables.size())) {
			cout << "I can not use this image to encrypt your message." << endl;
			return 0;
		}
		encrypt(key, message, tables, img, offset,heigh);
		ofstream file2;
		file2.open("output.bmp");
		file2.write(&img[0], img.size());
		file2.close();
		cout << "encrypting has been successful." << endl;
	}

	if (state == "decrypt") {
		cout << "Write your image_name :";
		cin >> image_name;
		cout << "Please waite..." << endl;
		Image image = process(image_name, img);
		if (!image.correct_process) {
			return 0;
		}
		width = image.width;
		heigh = image.heigh;
		offset = image.offset;
		vector<Unit> tables = create_chunk(width, heigh, image);
		sort_tables(tables);
		int key = 0;
		cout << "Write your key number :";
		cin >> key;
		cout << "Hidden message is : ";
		decrypt(key, tables, img, offset,heigh);
	}

	return 0;
}
