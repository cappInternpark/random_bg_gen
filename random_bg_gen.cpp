#include "curl/curl.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <cstdio>
#include <ctime>
#include <fstream>
using namespace std;

#include <opencv2/highgui/highgui.hpp>
using namespace cv;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    vector<uchar> *stream = (vector<uchar>*)userdata;
    size_t count = size * nmemb;
    stream->insert(stream->end(), ptr, ptr + count);
    return count;
}

cv::Mat curlImg(const char *img_url, int timeout=10)
{
    vector<uchar> stream;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, img_url); 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream); 
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); 
    CURLcode res = curl_easy_perform(curl); 
    curl_easy_cleanup(curl); 
    return imdecode(stream, -1);
}

cv::Mat gen_random_bg(cv::Size image_size){

	char buffer[50];

    srand(time(NULL));
	int src_num = std::rand() % 1080;

	sprintf(buffer, "https://picsum.photos/%d/%d/?image=%d",image_size.width,image_size.height,src_num);

	std::string url = buffer;

	cv::Mat image = curlImg(buffer);
    
	if (image.empty()) {
		cout << "WARNING: Image Empty" << std::endl;
	}

//	For testing, uncomment below to save the acquired image
//	imwrite( "../data/test_image.jpg", image);

	return image;
}


string img2label_path(const string img_path){
	string label_path = img_path;
	string img_ext = "jpg";
	string label_ext = "txt";
	int location = img_path.find(img_ext);
	int length = img_ext.length();
	label_path.replace(location, length, label_ext);
	
	return label_path;
}

string get_result_path(const string input_path, const string output_dir){
	string output_path = output_dir;
	//cout<<"output_dir : "<<output_dir<<endl;
	std::size_t location= input_path.find_last_of("/");
	string file_name = input_path.substr(location);
	output_path.append(file_name);
	//cout<<"output_path : "<<output_path<<endl;

	return output_path;
}


int get_label_rect(const string label_path, const Size img_size, vector<Rect> &box_vec){
	ifstream labelFile(label_path);
	if( labelFile.is_open() ){
		string line;
		getline(labelFile, line);	//for number of boxes
		int numBox=stoi(line);
		//cout<<"num Box : "<<numBox<<endl;

		while(numBox){
			Rect box;
			string name;
			int x1, x2, y1, y2, width, height;			
			getline(labelFile, line);
			//cout << line << endl;
			stringstream iss;
			iss.str(line);
			//cout<<"iss : "<<iss<<endl;
			iss >> name >> x1 >> y1 >> x2 >> y2;
			//cout<<"name : "<<name<<endl;
			//cout<<"x1 : "<<x1<<endl;

			x1 = MAX(0, x1);
			x2 = MIN(img_size.width-1, x2);
			y1 = MAX(0, y1);
			y2 = MIN(img_size.height-1, y2); 

			width = x2 - x1 + 1;
			height = y2 - y1 + 1;

			box = Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
			//cout<<"box : "<< box<<endl;
			box_vec.push_back(box);

			numBox--;
 		}
		labelFile.close();
	}

	return 1;
}

int crop_img(const Mat &src_img, const vector<Rect> &box_vec, vector<Mat> &crop_vec){
	for(int i=0; i<box_vec.size(); i++){
		//cout<<"box_vec : "<<box_vec[i]<<endl;
		Mat cropped = src_img(box_vec[i]);
		crop_vec.push_back(cropped);		
	}
	return 1;
}

int save_img(const string save_path, const Mat& img){
	imwrite(save_path, img);
	return 1;
}

int comb_img(const Mat bg, const vector<Mat> &crop_vec, const vector<Rect> &box_vec, Mat &result_img){
	//cout<<"crop_vec.size() : "<< crop_vec.size()<<endl;
	bg.copyTo(result_img);
	//save_img("bg.jpg", bg);
	for(int i=0; i<crop_vec.size(); i++){
		//cout<<"for init"<<endl;
		Mat crop_img = crop_vec[i];
		Rect roi= box_vec[i];
		//cout<<"roi : "<<roi<<endl;

		//save_img("crop_img.jpg", crop_img);
		crop_img.copyTo(result_img(roi));	
		//save_img("result.jpg", result_img);
		//cout<<"crop_img.copyTo(result_img(roi))	done "<<endl;
	}
	//cout<<"comb_img done"<<endl;
	return 1;
	
}



int main(){
	cv::Size img_size = cv::Size(640,360);
	string img_list_file_path = "PaperCup_random_bg.txt";
	//string img_list_file_path = "PaperCup_random_bg.txt";
	string output_img_dir = "PaperCup_random_bg";
	//string output_img_dir = "PaperCup_random_bg";
	ifstream img_list(img_list_file_path);

	if(!img_list.is_open()){
		cout<<"can't open img_list_file : "<<img_list_file_path<<endl;
		return -1;
	}

	int count = 0;
	string img_path, label_path;
	while(getline(img_list, img_path)){
		//cout<<"count : "<<count++ <<"  img_path :"<<img_path<<endl;
		//if(count <922 ) continue;
		Mat raw_img = imread(img_path);
		//imshow("raw_img", raw_img);
		if(raw_img.empty()){
			cout<<"raw_img.empty()"<<endl;
			continue;
		}
		
		Mat random_bg;
		while(random_bg.empty()){
			//cout<<"random_bg.empty()"<<endl;
			random_bg= gen_random_bg(img_size);
		}


		label_path = img2label_path(img_path);
		//cout<<"label_path : "<<label_path<<endl;

		Mat result_img ;
		vector<Rect> box_vec;
		vector<Mat> crop_img_vec;

		get_label_rect(label_path, img_size , box_vec);
		if(box_vec.size()==0) {
			cout<<"box_vec.size()==0)" <<endl;
			continue;
		}
		crop_img(raw_img, box_vec, crop_img_vec);
		comb_img(random_bg, crop_img_vec,box_vec, result_img);
		
		//cv::imshow("result_img", result_img);

		string output_path = get_result_path(img_path, output_img_dir);
		//cout<<"output_path : "<<output_path<<endl;
		save_img(output_path, result_img);
		
		raw_img.release();
	}
	return 0;
}

// RUN : g++ gen_img.cpp -o gen_img `pkg-config --cflags --libs opencv` -lcurl

